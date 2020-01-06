//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
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
#include "utilities/specifications_utilities.h"
#include "utilities/variable_utils.h"
#include "utilities/compare_elements_and_conditions_utility.h"

namespace Kratos
{
namespace SpecificationsUtilities
{
void AddMissingVariables(ModelPart& rModelPart)
{
    KRATOS_TRY

    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        AddMissingVariablesFromSpecifications(rModelPart, specifications, element_name);
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                AddMissingVariablesFromSpecifications(rModelPart, specifications, element_name);
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        AddMissingVariablesFromSpecifications(rModelPart, specifications, condition_name);
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                AddMissingVariablesFromSpecifications(rModelPart, specifications, condition_name);
            }
        }
    }

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

void AddMissingVariablesFromSpecifications(
    ModelPart& rModelPart,
    const Parameters SpecificationsParameters,
    const std::string EntityName
    )
{
    KRATOS_TRY

    if (SpecificationsParameters.Has("required_variables")) {
        const std::vector<std::string>& r_variables = SpecificationsParameters["required_variables"].GetStringArray();
        for (auto& r_variable_name : r_variables) {
            bool variable_is_missing = false;
            if (KratosComponents<Variable<double> >::Has(r_variable_name)) {
                const Variable<double>& r_variable = KratosComponents<Variable<double>>().Get(r_variable_name);
                variable_is_missing = rModelPart.HasNodalSolutionStepVariable(r_variable);
                
                // If variable is missign is added to the model part
                if (variable_is_missing) {
                    rModelPart.AddNodalSolutionStepVariable(r_variable);
                }
            } else if(KratosComponents<Variable<bool> >::Has(r_variable_name)) {
                const Variable<bool>& r_variable = KratosComponents<Variable<bool>>().Get(r_variable_name);
                variable_is_missing = rModelPart.HasNodalSolutionStepVariable(r_variable);
                
                // If variable is missign is added to the model part
                if (variable_is_missing) {
                    rModelPart.AddNodalSolutionStepVariable(r_variable);
                }
            } else if(KratosComponents<Variable<int> >::Has(r_variable_name)) {
                const Variable<int>& r_variable = KratosComponents<Variable<int>>().Get(r_variable_name);
                variable_is_missing = rModelPart.HasNodalSolutionStepVariable(r_variable);
                
                // If variable is missign is added to the model part
                if (variable_is_missing) {
                    rModelPart.AddNodalSolutionStepVariable(r_variable);
                }
            } else if(KratosComponents<Variable<array_1d<double, 3> > >::Has(r_variable_name)) {
                const Variable<array_1d<double, 3>>& r_variable = KratosComponents<Variable<array_1d<double, 3>>>().Get(r_variable_name);
                variable_is_missing = rModelPart.HasNodalSolutionStepVariable(r_variable);
                
                // If variable is missign is added to the model part
                if (variable_is_missing) {
                    rModelPart.AddNodalSolutionStepVariable(r_variable);
                }
            } else if(KratosComponents<Variable<array_1d<double, 6> > >::Has(r_variable_name)) {
                const Variable<array_1d<double, 6>>& r_variable = KratosComponents<Variable<array_1d<double, 6>>>().Get(r_variable_name);
                variable_is_missing = rModelPart.HasNodalSolutionStepVariable(r_variable);
                
                // If variable is missign is added to the model part
                if (variable_is_missing) {
                    rModelPart.AddNodalSolutionStepVariable(r_variable);
                }
            } else if(KratosComponents<Variable<Vector > >::Has(r_variable_name)) {
                const Variable<Vector>& r_variable = KratosComponents<Variable<Vector>>().Get(r_variable_name);
                variable_is_missing = rModelPart.HasNodalSolutionStepVariable(r_variable);
                
                // If variable is missign is added to the model part
                if (variable_is_missing) {
                    rModelPart.AddNodalSolutionStepVariable(r_variable);
                }
            } else if(KratosComponents<Variable<Matrix> >::Has(r_variable_name)) {
                const Variable<Matrix>& r_variable = KratosComponents<Variable<Matrix>>().Get(r_variable_name);
                variable_is_missing = rModelPart.HasNodalSolutionStepVariable(r_variable);
                
                // If variable is missign is added to the model part
                if (variable_is_missing) {
                    rModelPart.AddNodalSolutionStepVariable(r_variable);
                }
            } else {
                KRATOS_ERROR << "Value type for \"" << r_variable_name << "\" not defined";
            }
            KRATOS_WARNING_IF("SpecificationsUtilities", variable_is_missing) << "Variable:" << r_variable_name << " is not in the model part. Required by entity: " << EntityName << "Added to the model part" << std::endl; 
        }
    }
    
    KRATOS_CATCH("")
}
    
/***********************************************************************************/
/***********************************************************************************/

void AddMissingDofs(ModelPart& rModelPart)
{
    KRATOS_TRY

    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        AddMissingDofsFromSpecifications(rModelPart, specifications, element_name);
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                AddMissingDofsFromSpecifications(rModelPart, specifications, element_name);
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        AddMissingDofsFromSpecifications(rModelPart, specifications, condition_name);
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                AddMissingDofsFromSpecifications(rModelPart, specifications, condition_name);
            }
        }
    }


    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

void AddMissingDofsFromSpecifications(
    ModelPart& rModelPart,
    const Parameters SpecificationsParameters,
    const std::string EntityName
    )
{
    KRATOS_TRY

    // First node iterator
    const auto it_node_begin = rModelPart.Nodes().begin();
    
    if (SpecificationsParameters.Has("required_dofs")) {
        const std::vector<std::string>& r_variables = SpecificationsParameters["required_dofs"].GetStringArray();
        for (auto& r_variable_name : r_variables) {
            bool dof_is_missing = false;
            if (KratosComponents<Variable<double> >::Has(r_variable_name)) {
                const Variable<double>& r_variable = KratosComponents<Variable<double>>().Get(r_variable_name);
                dof_is_missing = it_node_begin->HasDofFor(r_variable);
                
                // If variable is missign is added to the model part
                if (dof_is_missing) {
                    VariableUtils().AddDof(r_variable, rModelPart);
                }
            } else if (KratosComponents<Component3VarType>::Has(r_variable_name)) {
                const Component3VarType& r_variable = KratosComponents<Component3VarType>().Get(r_variable_name);
                dof_is_missing = it_node_begin->HasDofFor(r_variable);
                
                // If variable is missign is added to the model part
                if (dof_is_missing) {
                    VariableUtils().AddDof(r_variable, rModelPart);
                }
            } else if (KratosComponents<Component4VarType>::Has(r_variable_name)) {
                const Component4VarType& r_variable = KratosComponents<Component4VarType>().Get(r_variable_name);
                dof_is_missing = it_node_begin->HasDofFor(r_variable);
                
                // If variable is missign is added to the model part
                if (dof_is_missing) {
                    VariableUtils().AddDof(r_variable, rModelPart);
                }
            } else if (KratosComponents<Component6VarType>::Has(r_variable_name)) {
                const Component6VarType& r_variable = KratosComponents<Component6VarType>().Get(r_variable_name);
                dof_is_missing = it_node_begin->HasDofFor(r_variable);
                
                // If variable is missign is added to the model part
                if (dof_is_missing) {
                    VariableUtils().AddDof(r_variable, rModelPart);
                }
            } else if (KratosComponents<Component9VarType>::Has(r_variable_name)) {
                const Component9VarType& r_variable = KratosComponents<Component9VarType>().Get(r_variable_name);
                dof_is_missing = it_node_begin->HasDofFor(r_variable);
                
                // If variable is missign is added to the model part
                if (dof_is_missing) {
                    VariableUtils().AddDof(r_variable, rModelPart);
                }
            } else {
                KRATOS_ERROR << "Value type for \"" << r_variable_name << "\" not defined";
            }
            KRATOS_WARNING_IF("SpecificationsUtilities", dof_is_missing) << "Variable:" << r_variable_name << " is not in the model part nodes. Required by entity: " << EntityName << "Added to the model part nodes" << std::endl; 
        }
    }

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

void DetermineFlagsUsed(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        if (specifications.Has("flags_used")) {
            const std::vector<std::string>& r_flags_used = specifications["flags_used"].GetStringArray();
            for (auto& r_flag_name : r_flags_used) {
                KRATOS_INFO("SpecificationsUtilities") << "Flag:" << r_flag_name << " is used by the element: " << element_name << std::endl; 
            }
        }
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                if (specifications.Has("flags_used")) {
                    const std::vector<std::string>& r_flags_used = specifications["flags_used"].GetStringArray();
                    for (auto& r_flag_name : r_flags_used) {
                        KRATOS_INFO("SpecificationsUtilities") << "Flag:" << r_flag_name << " is used by the element: " << element_name << std::endl; 
                    }
                }
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        if (specifications.Has("flags_used")) {
            const std::vector<std::string>& r_flags_used = specifications["flags_used"].GetStringArray();
            for (auto& r_flag_name : r_flags_used) {
                KRATOS_INFO("SpecificationsUtilities") << "Flag:" << r_flag_name << " is used by the condition: " << condition_name << std::endl; 
            }
        }
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                if (specifications.Has("flags_used")) {
                    const std::vector<std::string>& r_flags_used = specifications["flags_used"].GetStringArray();
                    for (auto& r_flag_name : r_flags_used) {
                        KRATOS_INFO("SpecificationsUtilities") << "Flag:" << r_flag_name << " is used by the condition: " << condition_name << std::endl; 
                    }
                }
            }
        }
    }

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

std::string DetermineFramework(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    std::string framework = "NONE";
    
    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        if (specifications.Has("framework")) {
            const std::string& r_framework = specifications["framework"].GetString();
            if (framework == "NONE") {
                framework = r_framework;
            } else {
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                KRATOS_WARNING("SpecificationsUtilities") << "Framework:" << r_framework << " is used by the element: " << element_name << ". But also the framework: " << framework << " is used" << std::endl; 
            }
        }
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                if (specifications.Has("framework")) {
                    const std::string& r_framework = specifications["framework"].GetString();
                    if (framework == "NONE") {
                        framework = r_framework;
                    } else {
                        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                        KRATOS_WARNING("SpecificationsUtilities") << "Framework:" << r_framework << " is used by the element: " << element_name << ". But also the framework: " << framework << " is used" << std::endl; 
                    }
                }
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        if (specifications.Has("framework")) {
            const std::string& r_framework = specifications["framework"].GetString();
            if (framework == "NONE") {
                framework = r_framework;
            } else {
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                KRATOS_WARNING("SpecificationsUtilities") << "Framework:" << r_framework << " is used by the condition: " << condition_name << ". But also the framework: " << framework << " is used" << std::endl; 
            }
        }
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                if (specifications.Has("framework")) {
                    const std::string& r_framework = specifications["framework"].GetString();
                    if (framework == "NONE") {
                        framework = r_framework;
                    } else {
                        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                        KRATOS_WARNING("SpecificationsUtilities") << "Framework:" << r_framework << " is used by the condition: " << condition_name << ". But also the framework: " << framework << " is used" << std::endl; 
                    }
                }
            }
        }
    }
    
    return framework;
    
    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

bool DetermineSymmetricLHS(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    bool symmetric = true;
    
    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        if (specifications.Has("symmetric_lhs")) {
            const bool symmetric_lhs = specifications["symmetric_lhs"].GetBool();
            if (symmetric != symmetric_lhs) {
                if (symmetric) {
                    symmetric = symmetric_lhs;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines non-symmetric LHS" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines symmetric LHS, but at least one element requires non-symmetric LHS. Non-symmetric LHS will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                if (specifications.Has("symmetric_lhs")) {
                    const bool symmetric_lhs = specifications["symmetric_lhs"].GetBool();
                    if (symmetric != symmetric_lhs) {
                        if (symmetric) {
                            symmetric = symmetric_lhs;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines non-symmetric LHS" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines symmetric LHS, but at least one element requires non-symmetric LHS. Non-symmetric LHS will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        if (specifications.Has("symmetric_lhs")) {
            const bool symmetric_lhs = specifications["symmetric_lhs"].GetBool();
            if (symmetric != symmetric_lhs) {
                if (symmetric) {
                    symmetric = symmetric_lhs;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines non-symmetric LHS" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines symmetric LHS, but at least one condition requires non-symmetric LHS. Non-symmetric LHS will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                if (specifications.Has("symmetric_lhs")) {
                    const bool symmetric_lhs = specifications["symmetric_lhs"].GetBool();
                    if (symmetric != symmetric_lhs) {
                        if (symmetric) {
                            symmetric = symmetric_lhs;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines non-symmetric LHS" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines symmetric LHS, but at least one condition requires non-symmetric LHS. Non-symmetric LHS will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    return symmetric;
    
    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

bool DeterminePositiveDefiniteLHS(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    bool positive_definite = true;
    
    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        if (specifications.Has("positive_definite_lhs")) {
            const bool positive_definite_lhs = specifications["positive_definite_lhs"].GetBool();
            if (positive_definite != positive_definite_lhs) {
                if (positive_definite) {
                    positive_definite = positive_definite_lhs;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines non-positive definite" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines positive definite, but at least one element requires non-positive definite. Non-positive definite will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                if (specifications.Has("positive_definite_lhs")) {
                    const bool positive_definite_lhs = specifications["positive_definite_lhs"].GetBool();
                    if (positive_definite != positive_definite_lhs) {
                        if (positive_definite) {
                            positive_definite = positive_definite_lhs;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines non-positive definite" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines positive definite, but at least one element requires non-positive definite. Non-positive definite will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        if (specifications.Has("positive_definite_lhs")) {
            const bool positive_definite_lhs = specifications["positive_definite_lhs"].GetBool();
            if (positive_definite != positive_definite_lhs) {
                if (positive_definite) {
                    positive_definite = positive_definite_lhs;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines non-positive definite" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines positive definite, but at least one condition requires non-positive definite. Non-positive definite will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                if (specifications.Has("positive_definite_lhs")) {
                    const bool positive_definite_lhs = specifications["positive_definite_lhs"].GetBool();
                    if (positive_definite != positive_definite_lhs) {
                        if (positive_definite) {
                            positive_definite = positive_definite_lhs;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines non-positive definite" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines positive definite, but at least one condition requires non-positive definite. Non-positive definite will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    return positive_definite;
    
    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

bool DetermineIfCompatibleGeometries(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    bool compatible_geometries = true;
    
    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        // Getting geometry type
        const auto& r_geometry = it_elem_begin->GetGeometry();
        const auto& r_geometry_type = r_geometry.GetGeometryType();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        if (specifications.Has("compatible_geometries")) {
            const std::vector<std::string> compatible_geometries = specifications["compatible_geometries"].GetStringArray();
            for (auto& r_geometry_name : compatible_geometries) {
                if (r_geometry_type != string_geometry_map[r_geometry_name]) {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "is not compatible with the following geometry: " << r_geometry_name << std::endl; 
                    return false;
                }
            }
        }
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
              
                // Getting geometry type
                const auto& r_geometry = it_elem_current->GetGeometry();
                const auto& r_geometry_type = r_geometry.GetGeometryType();
                
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                if (specifications.Has("compatible_geometries")) {
                    const std::vector<std::string> compatible_geometries = specifications["compatible_geometries"].GetStringArray();
                    for (auto& r_geometry_name : compatible_geometries) {
                        if (r_geometry_type != string_geometry_map[r_geometry_name]) {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "is not compatible with the following geometry: " << r_geometry_name << std::endl; 
                            return false;
                        }
                    }
                }
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condition has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        // Getting geometry type
        const auto& r_geometry = it_cond_begin->GetGeometry();
        const auto& r_geometry_type = r_geometry.GetGeometryType();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        if (specifications.Has("compatible_geometries")) {
            const std::vector<std::string> compatible_geometries = specifications["compatible_geometries"].GetStringArray();
            for (auto& r_geometry_name : compatible_geometries) {
                if (r_geometry_type != string_geometry_map[r_geometry_name]) {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "is not compatible with the following geometry: " << r_geometry_name << std::endl; 
                    return false;
                }
            }
        }
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
              
                // Getting geometry type
                const auto& r_geometry = it_cond_current->GetGeometry();
                const auto& r_geometry_type = r_geometry.GetGeometryType();
                
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                if (specifications.Has("compatible_geometries")) {
                    const std::vector<std::string> compatible_geometries = specifications["compatible_geometries"].GetStringArray();
                    for (auto& r_geometry_name : compatible_geometries) {
                        if (r_geometry_type != string_geometry_map[r_geometry_name]) {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "is not compatible with the following geometry: " << r_geometry_name << std::endl; 
                            return false;
                        }
                    }
                }
            }
        }
    }
    
    return compatible_geometries;
    
    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

bool DetermineIfImplicitSimulation(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    bool implicit_formulation = true;
    
    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        if (specifications.Has("is_implicit")) {
            const bool is_implicit = specifications["is_implicit"].GetBool();
            if (implicit_formulation != is_implicit) {
                if (implicit_formulation) {
                    implicit_formulation = is_implicit;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines explicit formulation" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines implicit formulation, but at least one element requires explicit formulation. Explicit formulation will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                if (specifications.Has("is_implicit")) {
                    const bool is_implicit = specifications["is_implicit"].GetBool();
                    if (implicit_formulation != is_implicit) {
                        if (implicit_formulation) {
                            implicit_formulation = is_implicit;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines explicit formulation" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines implicit formulation, but at least one element requires explicit formulation. Explicit formulation will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        if (specifications.Has("is_implicit")) {
            const bool is_implicit = specifications["is_implicit"].GetBool();
            if (implicit_formulation != is_implicit) {
                if (implicit_formulation) {
                    implicit_formulation = is_implicit;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines explicit formulation" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines implicit formulation, but at least one condition requires explicit formulation. Explicit formulation will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                if (specifications.Has("is_implicit")) {
                    const bool is_implicit = specifications["is_implicit"].GetBool();
                    if (implicit_formulation != is_implicit) {
                        if (implicit_formulation) {
                            implicit_formulation = is_implicit;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines explicit formulation" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines implicit formulation, but at least one condition requires explicit formulation. Explicit formulation will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    return implicit_formulation;
    
    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

bool DetermineIfRequiresTimeIntegration(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    bool requires_time_integration = true;
    
    // Define specifications
    Parameters specifications;
    
    // We are going to procede like the following, we are going to iterate over all the elements and compare with the components, we will save the type and we will compare until we get that the type of element has changed
    const auto& r_elements_array = rModelPart.Elements();
    if (r_elements_array.size() > 0) {
        std::string element_name;
        const auto it_elem_begin = r_elements_array.begin();
        
        specifications = it_elem_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
        if (specifications.Has("element_integrates_in_time")) {
            const bool element_integrates_in_time = specifications["element_integrates_in_time"].GetBool();
            if (requires_time_integration != element_integrates_in_time) {
                if (requires_time_integration) {
                    requires_time_integration = element_integrates_in_time;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines does not require time integration" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines requires time integration, but at least one element does not require time integration. Explicit formulation will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the elements
        for(std::size_t i = 1; i < r_elements_array.size(); i++) {
            const auto it_elem_previous = it_elem_begin + i - 1;
            const auto it_elem_current = it_elem_begin + i;
            
            if(!GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                specifications = it_elem_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_begin, element_name);
                if (specifications.Has("element_integrates_in_time")) {
                    const bool element_integrates_in_time = specifications["element_integrates_in_time"].GetBool();
                    if (requires_time_integration != element_integrates_in_time) {
                        if (requires_time_integration) {
                            requires_time_integration = element_integrates_in_time;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines does not require time integration" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The element: " << element_name << "defines requires time integration, but at least one element does not require time integration. Explicit formulation will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    // We are going to procede like the following, we are going to iterate over all the conditions and compare with the components, we will save the type and we will compare until we get that the type of condent has changed
    const auto& r_conditions_array = rModelPart.Conditions();
    if (r_conditions_array.size() > 0) {
        std::string condition_name;
        const auto it_cond_begin = r_conditions_array.begin();
        
        specifications = it_cond_begin->GetSpecifications();
        CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
        if (specifications.Has("element_integrates_in_time")) {
            const bool element_integrates_in_time = specifications["element_integrates_in_time"].GetBool();
            if (requires_time_integration != element_integrates_in_time) {
                if (requires_time_integration) {
                    requires_time_integration = element_integrates_in_time;
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines does not require time integration" << std::endl; 
                } else {
                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                    KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines requires time integration, but at least one condition does not require time integration. Explicit formulation will be considered" << std::endl; 
                }
            }
        }
            
        // Now we iterate over all the conditions
        for(std::size_t i = 1; i < r_conditions_array.size(); i++) {
            const auto it_cond_previous = it_cond_begin + i - 1;
            const auto it_cond_current = it_cond_begin + i;
            
            if(!GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                specifications = it_cond_current->GetSpecifications();
                CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_begin, condition_name);
                if (specifications.Has("element_integrates_in_time")) {
                    const bool element_integrates_in_time = specifications["element_integrates_in_time"].GetBool();
                    if (requires_time_integration != element_integrates_in_time) {
                        if (requires_time_integration) {
                            requires_time_integration = element_integrates_in_time;
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines does not require time integration" << std::endl; 
                        } else {
                            CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);
                            KRATOS_WARNING("SpecificationsUtilities") << "The condition: " << condition_name << "defines requires time integration, but at least one condition does not require time integration. Explicit formulation will be considered" << std::endl; 
                        }
                    }
                }
            }
        }
    }
    
    return requires_time_integration;
    
    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

bool CheckCompatibleConstitutiveLaws(ModelPart& rModelPart)
{
    KRATOS_TRY
    
    bool compatible_cl = false;
    
    return compatible_cl;
    
    KRATOS_CATCH("")
}

} // namespace SpecificationsUtilities
} // namespace Kratos