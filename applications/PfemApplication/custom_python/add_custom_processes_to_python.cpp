//
//   Project Name:        KratosPfemApplication     $
//   Created by:          $Author:      JMCarbonell $
//   Last modified by:    $Co-Author:               $
//   Date:                $Date:      February 2016 $
//   Revision:            $Revision:            0.0 $
//
//

// System includes

// External includes

// Project includes
#include "custom_python/add_custom_processes_to_python.h"

// Properties
#include "includes/properties.h"

// Processes
//#include "custom_processes/adaptive_time_interval_process.hpp"
#include "custom_processes/assign_properties_to_nodes_process.hpp"
#include "custom_processes/manage_isolated_nodes_process.hpp"

// PreMeshing processes
#include "custom_processes/recover_volume_losses_mesher_process.hpp"
#include "custom_processes/inlet_management_mesher_process.hpp"
#include "custom_processes/insert_new_nodes_mesher_process.hpp"
#include "custom_processes/remove_fluid_nodes_mesher_process.hpp"
#include "custom_processes/refine_fluid_elements_in_edges_mesher_process.hpp"

// MiddleMeshing processes

// PostMeshing processes


namespace Kratos
{

namespace Python
{

void  AddCustomProcessesToPython(pybind11::module& m)
{

  using namespace pybind11;

  //**********MODEL PROPERTIES*********//

  /// Properties container. A vector set of properties with their Id's as key.
  typedef PointerVectorSet<Properties, IndexedObject> PropertiesContainerType;
  typedef typename PropertiesContainerType::Pointer   PropertiesContainerPointerType;

  //to define it as a variable
  class_<Variable<PropertiesContainerPointerType>, VariableData>(m,"PropertiesVectorPointerVariable")
      .def( "__repr__", &Variable<PropertiesContainerPointerType>::Info )
      ;


  //**********MESHER PROCESSES*********//

  class_<RemoveFluidNodesMesherProcess, RemoveFluidNodesMesherProcess::Pointer, RemoveNodesMesherProcess>
      (m, "RemoveFluidNodes")
      .def(init<ModelPart&, MesherUtilities::MeshingParameters&, int>());

  class_<InsertNewNodesMesherProcess, InsertNewNodesMesherProcess::Pointer, MesherProcess>
      (m, "InsertNewNodes")
      .def(init<ModelPart&, MesherUtilities::MeshingParameters&, int>());

  class_<InletManagementMesherProcess, InletManagementMesherProcess::Pointer, MesherProcess>
      (m, "InletManagement")
      .def(init<ModelPart&, MesherUtilities::MeshingParameters&, int>());

  class_<RefineFluidElementsInEdgesMesherProcess, RefineFluidElementsInEdgesMesherProcess::Pointer, RefineElementsInEdgesMesherProcess>
      (m,"RefineFluidElementsInEdges")
      .def(init<ModelPart&, MesherUtilities::MeshingParameters&, int>())
      ;

  //*********SET SOLVER PROCESSES*************//

  class_<AssignPropertiesToNodesProcess, AssignPropertiesToNodesProcess::Pointer, Process>
      (m, "AssignPropertiesToNodes")
      .def(init<ModelPart&, Parameters>())
      .def(init<ModelPart&, Parameters&>());

  //*********ADAPTIVE TIME STEP*************//

  // class_<AdaptiveTimeIntervalProcess, AdaptiveTimeIntervalProcess::Pointer, Process>
  //     (m, "AdaptiveTimeIntervalProcess")
  //     .def(init<ModelPart&, int>());


  //*********VOLUME RECOVETY PROCESS********//

  class_<RecoverVolumeLossesMesherProcess, RecoverVolumeLossesMesherProcess::Pointer, MesherProcess>
      (m, "RecoverVolumeLosses")
      .def(init<ModelPart&, MesherUtilities::MeshingParameters&, int>());


  //*********VOLUME RECOVETY PROCESS********//
  class_<ManageIsolatedNodesProcess, ManageIsolatedNodesProcess::Pointer, Process>
      (m, "ManageIsolatedNodesProcess")
      .def(init<ModelPart&>());

}

}  // namespace Python.

} // Namespace Kratos
