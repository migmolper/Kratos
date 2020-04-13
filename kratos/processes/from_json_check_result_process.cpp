//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Vicente Mataix Ferrandiz
//

// System includes

// External includes

// Project includes
#include "containers/model.h"
#include "processes/from_json_check_result_process.h"

namespace Kratos
{
/// Local Flags
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, CORRECT_RESULT,                 0 );
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, HISTORICAL_VALUE,               1 );
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, USE_NODE_COORDINATES,           2 );
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, CHECK_ONLY_LOCAL_ENTITIES,      3 );
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, NODES_CONTAINER_INITIALIZED,    4 );
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, ELEMENTS_CONTAINER_INITIALIZED, 5 );
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, NODES_DATABASE_INITIALIZED,     6 );
KRATOS_CREATE_LOCAL_FLAG( FromJSONCheckResultProcess, ELEMENTS_DATABASE_INITIALIZED,  7 );

/***********************************************************************************/
/***********************************************************************************/

FromJSONCheckResultProcess::FromJSONCheckResultProcess(
    Model& rModel,
    Parameters ThisParameters
    ) : mrModelPart(GetModelPartFromModelAndSettings(rModel, ThisParameters)),
        mThisParameters(ThisParameters)
{
    mThisParameters.ValidateAndAssignDefaults(this->GetDefaultParameters());
}

/***********************************************************************************/
/***********************************************************************************/

FromJSONCheckResultProcess::FromJSONCheckResultProcess(
    ModelPart& rModelPart,
    Parameters ThisParameters
    ) : mrModelPart(rModelPart),
        mThisParameters(ThisParameters)
{
    mThisParameters.ValidateAndAssignDefaults(this->GetDefaultParameters());
}

/***********************************************************************************/
/***********************************************************************************/

void FromJSONCheckResultProcess::Execute()
{
    KRATOS_TRY;

    ExecuteInitialize();
    ExecuteFinalizeSolutionStep();
    ExecuteFinalize();

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void FromJSONCheckResultProcess::ExecuteInitialize()
{
    KRATOS_TRY;

    // We initialize the databases
    InitializeDatabases();

    // Additional values
    mFrequency = mThisParameters["time_frequency"].GetDouble();
    mAbsoluteTolerance = mThisParameters["tolerance"].GetDouble();
    mRelativeTolerance = mThisParameters["relative_tolerance"].GetDouble();
    const std::size_t absolute_tolerance_digits = ComputeRelevantDigits(mAbsoluteTolerance);
    const std::size_t relative_tolerance_digits = ComputeRelevantDigits(mRelativeTolerance);
    mRelevantDigits = static_cast<std::size_t>(std::max(absolute_tolerance_digits, relative_tolerance_digits)) + 1;

    // Set some flags
    this->Set(HISTORICAL_VALUE, mThisParameters["historical_value"].GetBool());
    this->Set(USE_NODE_COORDINATES, mThisParameters["use_node_coordinates"].GetBool());
    this->Set(CHECK_ONLY_LOCAL_ENTITIES, mThisParameters["check_only_local_entities"].GetBool());

    // Auxiliar flag
    this->Set(NODES_CONTAINER_INITIALIZED, false);
    this->Set(ELEMENTS_CONTAINER_INITIALIZED, false);
    this->Set(NODES_DATABASE_INITIALIZED, false);
    this->Set(ELEMENTS_DATABASE_INITIALIZED, false);

    // Set the flag CORRECT_RESULT to false
    this->Set(CORRECT_RESULT, false);

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void FromJSONCheckResultProcess::ExecuteFinalizeSolutionStep()
{
    KRATOS_TRY;

    const double time = mrModelPart.GetProcessInfo().GetValue(TIME);
    const double dt = mrModelPart.GetProcessInfo().GetValue(DELTA_TIME);
//     const double step = mrModelPart.GetProcessInfo().GetValue(STEP);

    mTimeCounter += dt;

    IndexType check_counter = 0;

    const auto& r_node_database = GetNodeDatabase();
    const auto& r_gp_database = GetGPDatabase();

    if (mTimeCounter > mFrequency) {
        mTimeCounter = 0.0;

        // Iterate over nodes
        const auto& r_nodes_array = GetNodes();
        const auto it_node_begin = r_nodes_array.begin();

        if (this->Is(HISTORICAL_VALUE)) {
            for (auto& p_var_double : mpNodalVariableDoubleList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_double);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const double result = it_node->FastGetSolutionStepValue(*p_var_double);
                    const double reference = r_var_database.GetValue(i, time);
                    if (!CheckValues(result, reference)) {
                        FailMessage(it_node->Id(), "Node", result, reference, p_var_double->Name());
                        check_counter += 1;
                    }
                }
            }
            for (auto& p_var_component : mpNodalVariableComponentsList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_component);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const double result = it_node->FastGetSolutionStepValue(*p_var_component);
                    const double reference = r_var_database.GetValue(i, time);
                    if (!CheckValues(result, reference)) {
                        FailMessage(it_node->Id(), "Node", result, reference, p_var_component->Name());
                        check_counter += 1;
                    }
                }
            }
            for (auto& p_var_array : mpNodalVariableArrayList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_array);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const auto& r_entity_database = r_var_database.GetEntityData(i);
                    const array_1d<double, 3>& r_result = it_node->FastGetSolutionStepValue(*p_var_array);
                    for (IndexType i_comp = 0; i_comp < 3; ++i_comp) {
                        const double reference = r_entity_database.GetValue(time, i_comp);
                        if (!CheckValues(r_result[i_comp], reference)) {
                            FailMessage(it_node->Id(), "Node", r_result[i_comp], reference, p_var_array->Name());
                            check_counter += 1;
                        }
                    }
                }
            }
            for (auto& p_var_vector : mpNodalVariableVectorList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_vector);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const auto& r_entity_database = r_var_database.GetEntityData(i);
                    const Vector& r_result = it_node->FastGetSolutionStepValue(*p_var_vector);
                    for (IndexType i_comp = 0; i_comp < r_result.size(); ++i_comp) {
                        const double reference = r_entity_database.GetValue(time, i_comp);
                        if (!CheckValues(r_result[i_comp], reference)) {
                            FailMessage(it_node->Id(), "Node", r_result[i_comp], reference, p_var_vector->Name());
                            check_counter += 1;
                        }
                    }
                }
            }
        } else { // Non-historical values
            for (auto& p_var_double : mpNodalVariableDoubleList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_double);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const double result = it_node->GetValue(*p_var_double);
                    const double reference = r_var_database.GetValue(i, time);
                    if (!CheckValues(result, reference)) {
                        FailMessage(it_node->Id(), "Node", result, reference, p_var_double->Name());
                        check_counter += 1;
                    }
                }
            }
            for (auto& p_var_component : mpNodalVariableComponentsList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_component);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const double result = it_node->GetValue(*p_var_component);
                    const double reference = r_var_database.GetValue(i, time);
                    if (!CheckValues(result, reference)) {
                        FailMessage(it_node->Id(), "Node", result, reference, p_var_component->Name());
                        check_counter += 1;
                    }
                }
            }
            for (auto& p_var_array : mpNodalVariableArrayList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_array);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const auto& r_entity_database = r_var_database.GetEntityData(i);
                    const array_1d<double, 3>& r_result = it_node->GetValue(*p_var_array);
                    for (IndexType i_comp = 0; i_comp < 3; ++i_comp) {
                        const double reference = r_entity_database.GetValue(time, i_comp);
                        if (!CheckValues(r_result[i_comp], reference)) {
                            FailMessage(it_node->Id(), "Node", r_result[i_comp], reference, p_var_array->Name());
                            check_counter += 1;
                        }
                    }
                }
            }
            for (auto& p_var_vector : mpNodalVariableVectorList) {
                const auto& r_var_database = r_node_database.GetVariableData(*p_var_vector);

//                 #pragma omp parallel for reduction(+:check_counter)
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;

                    const auto& r_entity_database = r_var_database.GetEntityData(i);
                    const Vector& r_result = it_node->GetValue(*p_var_vector);
                    for (IndexType i_comp = 0; i_comp < r_result.size(); ++i_comp) {
                        const double reference = r_entity_database.GetValue(time, i_comp);
                        if (!CheckValues(r_result[i_comp], reference)) {
                            FailMessage(it_node->Id(), "Node", r_result[i_comp], reference, p_var_vector->Name());
                            check_counter += 1;
                        }
                    }
                }
            }
        }

        // Getting process info
        const auto& r_process_info = mrModelPart.GetProcessInfo();

        // Iterate over elements
        const auto& r_elements_array = GetElements();
        const auto it_elem_begin = r_elements_array.begin();

        // Result vectors
        std::vector<double> result_double;
        std::vector<array_1d<double,3>> result_array;
        std::vector<Vector> result_vector;

        for (auto& p_var_double : mpGPVariableDoubleList) {
            const auto& r_var_database = r_gp_database.GetVariableData(*p_var_double);

//             #pragma omp parallel for reduction(+:check_counter) firstprivate(result_double)
            for (int i = 0; i < static_cast<int>(r_elements_array.size()); ++i) {
                auto it_elem = it_elem_begin + i;

                const auto& r_entity_database = r_var_database.GetEntityData(i);
                it_elem->CalculateOnIntegrationPoints(*p_var_double, result_double, r_process_info);
                for (IndexType i_gp = 0; i_gp < result_double.size(); ++i_gp) {
                    const double result = result_double[i_gp];
                    const double reference = r_entity_database.GetValue(time, 0, i_gp);
                    if (!CheckValues(result, reference)) {
                        FailMessage(it_elem->Id(), "Element", result, reference, p_var_double->Name(), -1, i_gp);
                        check_counter += 1;
                    }
                }
            }
        }
        for (auto& p_var_array : mpGPVariableArrayList) {
            const auto& r_var_database = r_gp_database.GetVariableData(*p_var_array);

//             #pragma omp parallel for reduction(+:check_counter) firstprivate(result_array)
            for (int i = 0; i < static_cast<int>(r_elements_array.size()); ++i) {
                auto it_elem = it_elem_begin + i;

                const auto& r_entity_database = r_var_database.GetEntityData(i);
                it_elem->CalculateOnIntegrationPoints(*p_var_array, result_array, r_process_info);
                for (IndexType i_gp = 0; i_gp < result_array.size(); ++i_gp) {
                    for (IndexType i_comp = 0; i_comp < 3; ++i_comp) {
                        const double reference = r_entity_database.GetValue(time, i_comp, i_gp);
                        if (!CheckValues(result_array[i_gp][i_comp], reference)) {
                            FailMessage(it_elem->Id(), "Element", result_array[i_gp][i_comp], reference, p_var_array->Name(), i_comp, i_gp);
                            check_counter += 1;
                        }
                    }
                }
            }
        }
        for (auto& p_var_vector : mpGPVariableVectorList) {
            const auto& r_var_database = r_gp_database.GetVariableData(*p_var_vector);

//             #pragma omp parallel for reduction(+:check_counter) firstprivate(result_vector)
            for (int i = 0; i < static_cast<int>(r_elements_array.size()); ++i) {
                auto it_elem = it_elem_begin + i;

                const auto& r_entity_database = r_var_database.GetEntityData(i);
                it_elem->CalculateOnIntegrationPoints(*p_var_vector, result_vector, r_process_info);
                for (IndexType i_gp = 0; i_gp < result_vector.size(); ++i_gp) {
                    for (IndexType i_comp = 0; i_comp < result_vector[i_gp].size(); ++i_comp) {
                        const double reference = r_entity_database.GetValue(time, i_comp, i_gp);
                        if (!CheckValues(result_vector[i_gp][i_comp], reference)) {
                            FailMessage(it_elem->Id(), "Element", result_vector[i_gp][i_comp], reference, p_var_vector->Name(), i_comp, i_gp);
                            check_counter += 1;
                        }
                    }
                }
            }
        }
    }

    // Final check
    if (check_counter > 0) {
        this->Set(CORRECT_RESULT, false);
    } else {
        this->Set(CORRECT_RESULT, true);
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void FromJSONCheckResultProcess::ExecuteFinalize()
{
    KRATOS_TRY;

    // This clears the databse of the result information
    mDatabaseNodes.Clear();
    mDatabaseGP.Clear();

    // Set the flag to false
    this->Set(CORRECT_RESULT, false);

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

int FromJSONCheckResultProcess::Check()
{
    KRATOS_TRY;

    const int value_nodes = mDatabaseNodes.Check();
    const int value_gp = mDatabaseGP.Check();

    return value_nodes + value_gp;

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void FromJSONCheckResultProcess::InitializeDatabases()
{
    KRATOS_TRY;

    // This clears the databse of the result information in first place
    mDatabaseNodes.Clear();
    mDatabaseGP.Clear();

    // If we consider any flag
    const auto& r_flag_name = mThisParameters["check_for_flag"].GetString();
    const Flags* p_flag = (r_flag_name != "") ? KratosComponents<Flags>::Has(r_flag_name) ? &KratosComponents<Flags>::Get(r_flag_name) : nullptr : nullptr;

    // Initialize the nodes and elements
    const auto& r_nodes_array = GetNodes(p_flag);
    const auto it_node_begin = r_nodes_array.begin();
    const auto& r_elements_array = GetElements(p_flag);
    const auto it_elem_begin = r_elements_array.begin();

    // Read JSON string in results file, create Parameters
    const std::string& r_input_filename = mThisParameters["input_file_name"].GetString();
    std::ifstream infile(r_input_filename);
    KRATOS_ERROR_IF_NOT(infile.good()) << "Results file: " << r_input_filename << " cannot be found" << std::endl;
    std::stringstream buffer;
    buffer << infile.rdbuf();
    Parameters results(buffer.str());

    // The auxiliar vectors for the variables
    const auto& r_check_variables_names = mThisParameters["check_variables"].GetStringArray();
    const auto& r_gauss_points_check_variables = mThisParameters["gauss_points_check_variables"].GetStringArray();

    // We fill the list of variables
    FillVariablesList(r_check_variables_names, r_gauss_points_check_variables);

    // Nodal
    std::vector<IndexType> nodal_variables_ids(r_check_variables_names.size());
    std::vector<IndexType> nodal_values_sizes(r_check_variables_names.size(), 1);
    SizeType aux_size = 0;
    for (IndexType i = 0; i < mpNodalVariableDoubleList.size(); ++i) {
        nodal_variables_ids[aux_size + i] = mpNodalVariableDoubleList[i]->Key();
    }
    aux_size += mpNodalVariableDoubleList.size();
    for (IndexType i = 0; i < mpNodalVariableComponentsList.size(); ++i) {
        nodal_variables_ids[aux_size + i] = mpNodalVariableComponentsList[i]->Key();
    }
    aux_size += mpNodalVariableComponentsList.size();
    for (IndexType i = 0; i < mpNodalVariableArrayList.size(); ++i) {
        nodal_variables_ids[aux_size + i] = mpNodalVariableArrayList[i]->Key();
        nodal_values_sizes[aux_size + i] = 3;
    }
    aux_size += mpNodalVariableArrayList.size();
    for (IndexType i = 0; i < mpNodalVariableVectorList.size(); ++i) {
        nodal_variables_ids[aux_size + i] = mpNodalVariableVectorList[i]->Key();
        const Vector& r_vector = results[mpNodalVariableVectorList[i]->Name()]["NODE_" + GetNodeIdentifier(*it_node_begin)][0].GetVector();
        nodal_values_sizes[aux_size + i] = r_vector.size();
    }

    // Set the flags if possible to initialize
    if ((r_nodes_array.size() > 0 && aux_size > 0) || aux_size == 0) {
        this->Set(NODES_DATABASE_INITIALIZED, true);
    }

    // GP
    std::vector<IndexType> gp_variables_ids(r_gauss_points_check_variables.size());
    std::vector<IndexType> gp_values_sizes(r_gauss_points_check_variables.size(), 1);
    aux_size = 0;
    for (IndexType i = 0; i < mpGPVariableDoubleList.size(); ++i) {
        gp_variables_ids[aux_size + i] = mpGPVariableDoubleList[i]->Key();
    }
    aux_size += mpGPVariableDoubleList.size();
    for (IndexType i = 0; i < mpGPVariableArrayList.size(); ++i) {
        gp_variables_ids[aux_size + i] = mpGPVariableArrayList[i]->Key();
        gp_values_sizes[aux_size + i] = 3;
    }
    aux_size += mpGPVariableArrayList.size();
    SizeType number_of_gp = 0;
    for (IndexType i = 0; i < mpGPVariableVectorList.size(); ++i) {
        gp_variables_ids[aux_size + i] = mpGPVariableVectorList[i]->Key();
        const auto& r_data = results[mpGPVariableVectorList[i]->Name()]["ELEMENT_" + std::to_string(it_elem_begin->Id())];
        number_of_gp = r_data.size();
        const Vector& r_vector = r_data["0"][0].GetVector();
        gp_values_sizes[aux_size + i] = r_vector.size();
    }

    // Set the flags if possible to initialize
    if ((r_elements_array.size() > 0 && aux_size > 0) || aux_size == 0) {
        this->Set(ELEMENTS_DATABASE_INITIALIZED, true);
    }

    // Skip if not possible to initialize
    if (this->IsNot(NODES_DATABASE_INITIALIZED) && this->IsNot(ELEMENTS_DATABASE_INITIALIZED)) {
        return void();
    }

    // Initialize the databases
    mDatabaseNodes.Initialize(nodal_variables_ids, nodal_values_sizes, r_nodes_array.size());
    mDatabaseGP.Initialize(gp_variables_ids, gp_values_sizes, r_elements_array.size());

    // Get the time vector
    const Vector& r_time = results["TIME"].GetVector();
    const SizeType time_size = r_time.size();
    mDatabaseNodes.SetCommonColumn(r_time);
    mDatabaseGP.SetCommonColumn(r_time);

    // Fill database
    for (auto& p_var_double : mpNodalVariableDoubleList) {
        auto& r_var_database = mDatabaseNodes.GetVariableData(*p_var_double);
        const std::string& r_variable_name = p_var_double->Name();
        for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
            auto it_node = it_node_begin + i;
            const std::string node_identifier = "NODE_" + GetNodeIdentifier(*it_node);
            const auto& r_vector = results[node_identifier][r_variable_name].GetVector();
            r_var_database.SetValues(r_time, r_vector, i);
        }
    }
    for (auto& p_var_component : mpNodalVariableComponentsList) {
        auto& r_var_database = mDatabaseNodes.GetVariableData(*p_var_component);
        const std::string& r_variable_name = p_var_component->Name();
        for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
            auto it_node = it_node_begin + i;
            const std::string node_identifier = "NODE_" + GetNodeIdentifier(*it_node);
            const auto& r_vector = results[node_identifier][r_variable_name].GetVector();
            r_var_database.SetValues(r_time, r_vector, i);
        }
    }
    for (auto& p_var_array : mpNodalVariableArrayList) {
        auto& r_var_database = mDatabaseNodes.GetVariableData(*p_var_array);
        const std::string& r_variable_name = p_var_array->Name();
        const bool is_saved_as_components = KratosComponents<VariableComponent<ComponentType>>::Has(r_variable_name + "_X");
        if (is_saved_as_components) {
            std::vector<std::string> components_name = {{"_X", "_Y", "_Z"}};
            for (IndexType i_comp = 0; i_comp < 3; ++i_comp) {
                const auto& r_comp_name = components_name[i_comp];
                for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                    auto it_node = it_node_begin + i;
                    const std::string node_identifier = "NODE_" + GetNodeIdentifier(*it_node);
                    const auto& r_vector = results[node_identifier][r_variable_name + r_comp_name].GetVector();
                    r_var_database.SetValues(r_time, r_vector, i, i_comp);
                }
            }
        } else {
            Vector aux_vector_1(time_size);
            Vector aux_vector_2(time_size);
            Vector aux_vector_3(time_size);
            for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
                auto it_node = it_node_begin + i;
                const std::string node_identifier = "NODE_" + GetNodeIdentifier(*it_node);
                const auto& r_database = results[node_identifier][r_variable_name];
                for (IndexType i_step = 0; i_step < time_size; ++i_step) {
                    const Vector& r_vector = r_database[i_step].GetVector();
                    aux_vector_1[i_step] = r_vector[0];
                    aux_vector_2[i_step] = r_vector[1];
                    aux_vector_3[i_step] = r_vector[2];
                }
                r_var_database.SetValues(r_time, aux_vector_1, i, 0);
                r_var_database.SetValues(r_time, aux_vector_2, i, 1);
                r_var_database.SetValues(r_time, aux_vector_3, i, 2);
            }
        }
    }
    for (auto& p_var_vector : mpNodalVariableVectorList) {
        auto& r_var_database = mDatabaseNodes.GetVariableData(*p_var_vector);
        const std::string& r_variable_name = p_var_vector->Name();

        const Vector& r_vector = results[r_variable_name]["NODE_" + GetNodeIdentifier(*it_node_begin)][0].GetVector();
        const SizeType size_vector = r_vector.size();

        Vector aux_vector(time_size);
        std::vector<Vector> components_vector(size_vector, aux_vector);
        for (int i = 0; i < static_cast<int>(r_nodes_array.size()); ++i) {
            auto it_node = it_node_begin + i;
            const std::string node_identifier = "NODE_" + GetNodeIdentifier(*it_node);
            const auto& r_database = results[node_identifier][r_variable_name];
            for (IndexType i_step = 0; i_step < time_size; ++i_step) {
                const Vector& r_vector = r_database[i_step].GetVector();
                for (IndexType i_vector = 0; i_vector < size_vector; ++i_vector) {
                    components_vector[i_vector][i_step] = r_vector[i_vector];
                }
            }
            for (IndexType i_vector = 0; i_vector < size_vector; ++i_vector) {
                r_var_database.SetValues(r_time,components_vector[i_vector], i, i_vector);
            }
        }
    }

    for (auto& p_var_double : mpNodalVariableDoubleList) {
        auto& r_var_database = mDatabaseGP.GetVariableData(*p_var_double);
        const std::string& r_variable_name = p_var_double->Name();
        for (int i = 0; i < static_cast<int>(r_elements_array.size()); ++i) {
            auto it_elem = it_elem_begin + i;
            const std::string element_identifier = "ELEMENT_" + std::to_string(it_elem->Id());
            const auto& r_data = results[element_identifier][r_variable_name];
            for (IndexType i_gp = 0; i_gp < number_of_gp; ++i_gp) {
                const auto& r_vector = r_data[std::to_string(i_gp)].GetVector();
                r_var_database.SetValues(r_time, r_vector, i, 0, i_gp);
            }
        }
    }
    for (auto& p_var_array : mpGPVariableArrayList) {
        auto& r_var_database = mDatabaseGP.GetVariableData(*p_var_array);
        const std::string& r_variable_name = p_var_array->Name();
        const bool is_saved_as_components = KratosComponents<VariableComponent<ComponentType>>::Has(r_variable_name + "_X");
        if (is_saved_as_components) {
            std::vector<std::string> components_name = {{"_X", "_Y", "_Z"}};
            for (IndexType i_comp = 0; i_comp < 3; ++i_comp) {
                const auto& r_comp_name = components_name[i_comp];
                for (int i = 0; i < static_cast<int>(r_elements_array.size()); ++i) {
                    auto it_elem = it_elem_begin + i;
                    const std::string element_identifier = "ELEMENT_" + std::to_string(it_elem->Id());
                    for (IndexType i_gp = 0; i_gp < number_of_gp; ++i_gp) {
                        const auto& r_vector = results[element_identifier][r_variable_name + r_comp_name][std::to_string(i_gp)].GetVector();
                        r_var_database.SetValues(r_time, r_vector, i, i_comp, i_gp);
                    }
                }
            }
        } else {
            Vector aux_vector_1(time_size);
            Vector aux_vector_2(time_size);
            Vector aux_vector_3(time_size);
            for (int i = 0; i < static_cast<int>(r_elements_array.size()); ++i) {
                auto it_elem = it_elem_begin + i;
                const std::string element_identifier = "ELEMENT_" + std::to_string(it_elem->Id());
                for (IndexType i_gp = 0; i_gp < number_of_gp; ++i_gp) {
                    const auto& r_database = results[element_identifier][r_variable_name][std::to_string(i_gp)];
                    for (IndexType i_step = 0; i_step < time_size; ++i_step) {
                        const Vector& r_vector = r_database[i_step].GetVector();
                        aux_vector_1[i_step] = r_vector[0];
                        aux_vector_2[i_step] = r_vector[1];
                        aux_vector_3[i_step] = r_vector[2];
                    }
                    r_var_database.SetValues(r_time, aux_vector_1, i, 0, i_gp);
                    r_var_database.SetValues(r_time, aux_vector_2, i, 1, i_gp);
                    r_var_database.SetValues(r_time, aux_vector_3, i, 2, i_gp);
                }
            }
        }
    }
    for (auto& p_var_vector : mpGPVariableVectorList) {
        auto& r_var_database = mDatabaseGP.GetVariableData(*p_var_vector);
        const std::string& r_variable_name = p_var_vector->Name();

        const Vector& r_vector = results[r_variable_name]["ELEMENT_" + std::to_string(it_elem_begin->Id())]["0"][0].GetVector();
        const SizeType size_vector = r_vector.size();

        Vector aux_vector(time_size);
        std::vector<Vector> components_vector(size_vector, aux_vector);
        for (int i = 0; i < static_cast<int>(r_elements_array.size()); ++i) {
            auto it_elem = it_elem_begin + i;
            const std::string element_identifier = "ELEMENT_" + std::to_string(it_elem->Id());
            for (IndexType i_gp = 0; i_gp < number_of_gp; ++i_gp) {
                const auto& r_database = results[element_identifier][r_variable_name][std::to_string(i_gp)];
                for (IndexType i_step = 0; i_step < time_size; ++i_step) {
                    const Vector& r_vector = r_database[i_step].GetVector();
                    for (IndexType i_vector = 0; i_vector < size_vector; ++i_vector) {
                        components_vector[i_vector][i_step] = r_vector[i_vector];
                    }
                }
                for (IndexType i_vector = 0; i_vector < size_vector; ++i_vector) {
                    r_var_database.SetValues(r_time,components_vector[i_vector], i, i_vector, i_gp);
                }
            }
        }
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void FromJSONCheckResultProcess::FillVariablesList(
    const std::vector<std::string>& rNodalVariablesNames,
    const std::vector<std::string>& rGPVariablesNames
    )
{
    KRATOS_TRY;

    // Nodal variables
    for (const std::string& r_variable_name : rNodalVariablesNames) {
        if (KratosComponents<Variable<double>>::Has(r_variable_name)){
            mpNodalVariableDoubleList.push_back(&(KratosComponents< Variable<double>>::Get(r_variable_name)));
        } else if (KratosComponents<VariableComponent<ComponentType>>::Has(r_variable_name)) {
            mpNodalVariableComponentsList.push_back(&(KratosComponents<VariableComponent<ComponentType>>::Get(r_variable_name)));
        } else if (KratosComponents<Variable<array_1d<double, 3> >>::Has(r_variable_name)) {
            mpNodalVariableArrayList.push_back(&(KratosComponents<Variable<array_1d<double, 3>>>::Get(r_variable_name)));
        } else if (KratosComponents<Variable<Vector>>::Has(r_variable_name)) {
            mpNodalVariableVectorList.push_back(&(KratosComponents<Variable<Vector>>::Get(r_variable_name)));
        } else {
            KRATOS_ERROR << "Only double, array, vector and component variables are allowed in the variables list." << std::endl;
        }
    }

    // GP variables
    for (const std::string& r_variable_name : rGPVariablesNames) {
        if (KratosComponents<Variable<double>>::Has(r_variable_name)){
            mpGPVariableDoubleList.push_back(&(KratosComponents< Variable<double>>::Get(r_variable_name)));
        } else if (KratosComponents<Variable<array_1d<double, 3> >>::Has(r_variable_name)) {
            mpGPVariableArrayList.push_back(&(KratosComponents<Variable<array_1d<double, 3>>>::Get(r_variable_name)));
        } else if (KratosComponents<Variable<Vector>>::Has(r_variable_name)) {
            mpGPVariableVectorList.push_back(&(KratosComponents<Variable<Vector>>::Get(r_variable_name)));
        } else {
            KRATOS_ERROR << "Only double, array and vector variables are allowed in the variables list." << std::endl;
        }
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

bool FromJSONCheckResultProcess::CheckValues(
    const double ValueEntity,
    const double ValueJSON
    )
{
    const bool check = std::abs(ValueEntity-ValueJSON) <= std::max(mRelativeTolerance * std::max(std::abs(ValueEntity), std::abs(ValueJSON)), mAbsoluteTolerance) ? true : false;
    return check;
}

/***********************************************************************************/
/***********************************************************************************/

void FromJSONCheckResultProcess::FailMessage(
    const IndexType EntityId,
    const std::string& rEntityType,
    const double ValueEntity,
    const double ValueJSON,
    const std::string& rVariableName,
    const int ComponentIndex,
    const int GPIndex
    )
{
    if (ComponentIndex > -1) {
        if (GPIndex > -1) {
            KRATOS_WARNING("FromJSONCheckResultProcess") << "Error checking for variable " << rVariableName << " Component " << ComponentIndex << " in GP " << GPIndex << ", results:\n" << rEntityType << " " << EntityId << " " << std::setprecision(mRelevantDigits) << ValueEntity << "!=" << std::setprecision(mRelevantDigits) << ValueJSON << "; rel_tol=" << mRelativeTolerance << ", abs_tol=" << mAbsoluteTolerance << std::endl;
        } else {
            KRATOS_WARNING("FromJSONCheckResultProcess") << "Error checking for variable " << rVariableName << " Component " << ComponentIndex << ", results:\n" << rEntityType << " " << EntityId << " " << std::setprecision(mRelevantDigits) << ValueEntity << "!=" << std::setprecision(mRelevantDigits) << ValueJSON << "; rel_tol=" << mRelativeTolerance << ", abs_tol=" << mAbsoluteTolerance << std::endl;
        }
    } else {
        if (GPIndex > -1) {
            KRATOS_WARNING("FromJSONCheckResultProcess") << "Error checking for variable " << rVariableName << " in GP " << GPIndex << ", results:\n" << rEntityType << " " << EntityId << " " << std::setprecision(mRelevantDigits) << ValueEntity << "!=" << std::setprecision(mRelevantDigits) << ValueJSON << "; rel_tol=" << mRelativeTolerance << ", abs_tol=" << mAbsoluteTolerance << std::endl;
        } else {
            KRATOS_WARNING("FromJSONCheckResultProcess") << "Error checking for variable " << rVariableName << ", results:\n" << rEntityType << " " << EntityId << " " << std::setprecision(mRelevantDigits) << ValueEntity << "!=" << std::setprecision(mRelevantDigits) << ValueJSON << "; rel_tol=" << mRelativeTolerance << ", abs_tol=" << mAbsoluteTolerance << std::endl;
        }
    }
}

/***********************************************************************************/
/***********************************************************************************/

std::string FromJSONCheckResultProcess::GetNodeIdentifier(NodeType& rNode)
{
    if (this->Is(USE_NODE_COORDINATES)) {
        const SizeType digits = 6;
        std::stringstream ss;
        ss.setf( std::ios::fixed );
        ss << "X_" << std::setprecision( digits ) << rNode.X0() << "_Y_" << std::setprecision( digits ) << rNode.Y0() << "_Z_" << std::setprecision( digits ) << rNode.Z0();
        return ss.str();
    } else {
        return std::to_string(rNode.Id());
    }
}

/***********************************************************************************/
/***********************************************************************************/

FromJSONCheckResultProcess::NodesArrayType& FromJSONCheckResultProcess::GetNodes(const Flags* pFlag)
{
    KRATOS_TRY;

    if (this->IsNot(NODES_CONTAINER_INITIALIZED)) {
        auto& r_nodes_array = this->Is(CHECK_ONLY_LOCAL_ENTITIES) ? mrModelPart.GetCommunicator().LocalMesh().Nodes() : mrModelPart.Nodes();
        const auto it_node_begin = r_nodes_array.begin();
        const int number_of_nodes = static_cast<int>(r_nodes_array.size());

        #pragma omp parallel firstprivate(number_of_nodes)
        {
            NodesArrayType nodes_buffer;

            # pragma omp for schedule(guided, 512) nowait
            for (int i = 0; i < number_of_nodes; ++i) {
                auto it_node = it_node_begin + i;
                if (CheckFlag(*it_node, pFlag)) nodes_buffer.push_back(*(it_node.base()));

            }

            // We merge all the sets in one thread
            #pragma omp critical
            {
                for (auto it_node = nodes_buffer.begin(); it_node < nodes_buffer.end(); ++it_node)
                    mNodesArray.push_back(*(it_node.base()));
            }
        }
        // Set flag
        this->Set(NODES_CONTAINER_INITIALIZED, true);
    }

    return mNodesArray;

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

FromJSONCheckResultProcess::ElementsArrayType& FromJSONCheckResultProcess::GetElements(const Flags* pFlag)
{
    KRATOS_TRY;

    if (this->IsNot(ELEMENTS_CONTAINER_INITIALIZED)) {
        auto& r_elements_array = this->Is(CHECK_ONLY_LOCAL_ENTITIES) ? mrModelPart.GetCommunicator().LocalMesh().Elements() : mrModelPart.Elements();
        const auto it_elem_begin = r_elements_array.begin();
        const int number_of_elements = static_cast<int>(r_elements_array.size());

        #pragma omp parallel firstprivate(number_of_elements)
        {
            ElementsArrayType elements_buffer;

            # pragma omp for schedule(guided, 512) nowait
            for (int i = 0; i < number_of_elements; ++i) {
                auto it_elem = it_elem_begin + i;
                if (CheckFlag(*it_elem, pFlag)) elements_buffer.push_back(*(it_elem.base()));

            }

            // We merge all the sets in one thread
            #pragma omp critical
            {
                for (auto it_elem = elements_buffer.begin(); it_elem < elements_buffer.end(); ++it_elem)
                    mElementsArray.push_back(*(it_elem.base()));
            }
        }
        // Set flag
        this->Set(ELEMENTS_CONTAINER_INITIALIZED, true);
    }

    return mElementsArray;

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

std::size_t FromJSONCheckResultProcess::ComputeRelevantDigits(const double Value)
{
    return static_cast<std::size_t>(std::ceil(std::abs(std::log10(Value))));
}

/***********************************************************************************/
/***********************************************************************************/

const Parameters FromJSONCheckResultProcess::GetDefaultParameters() const
{
    const Parameters default_parameters = Parameters(R"(
    {
        "help"                         : "This process checks the solution obtained from a given json file. It can be used for generating tests for a problem",
        "check_variables"              : [],
        "gauss_points_check_variables" : [],
        "input_file_name"              : "",
        "model_part_name"              : "",
        "sub_model_part_name"          : "",
        "check_for_flag"               : "",
        "historical_value"             : true,
        "tolerance"                    : 1e-3,
        "relative_tolerance"           : 1e-6,
        "time_frequency"               : 1.00,
        "use_node_coordinates"         : false,
        "check_only_local_entities"    : false
    })" );
    return default_parameters;
}

/***********************************************************************************/
/***********************************************************************************/

ModelPart& FromJSONCheckResultProcess::GetModelPart() const
{
    return this->mrModelPart;
}

/***********************************************************************************/
/***********************************************************************************/

Parameters FromJSONCheckResultProcess::GetSettings() const
{
    return this->mThisParameters;
}

} // namespace Kratos