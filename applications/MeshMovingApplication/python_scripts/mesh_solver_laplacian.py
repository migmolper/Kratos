from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7

# Importing the Kratos Library
import KratosMultiphysics

# Import applications
import KratosMultiphysics.MeshMovingApplication as KratosMeshMoving

# Import baseclass
from KratosMultiphysics.MeshMovingApplication.mesh_solver_base import MeshSolverBase


def CreateSolver(mesh_model_part, custom_settings):
    return MeshSolverLaplacian(mesh_model_part, custom_settings)


class MeshSolverLaplacian(MeshSolverBase):
    def __init__(self, mesh_model_part, custom_settings):
        if custom_settings.Has("buffer_size"):
            buffer_size = custom_settings["buffer_size"].GetInt()
            if buffer_size < 2:
                raise Exception("A buffer_size of at least 2 is required!")
        else: # overwritting baseclass-default
            custom_settings.AddEmptyValue("buffer_size").SetInt(2)
        super(MeshSolverLaplacian, self).__init__(mesh_model_part, custom_settings)
        print("::[MeshSolverLaplacian]:: Construction finished")

    def ImportModelpart(self):
        super(MeshSolverLaplacian, self).ImportModelpart()

        # TODO I think this should be done in initialize, bcs of the ALE-solver ...?

        self.laplacian_elements_part = self.main_model_part.CreateSubModelpart("LaplacianMMElements")

        con_pres_mod_settings = KratosMultiphysics.Parameters("""
        {
            "element_name"              : "LaplacianMeshMovingElement",
            "duplicate_sub_model_parts" : false
        }""")

        modeler = KratosMultiphysics.ConnectivityPreserveModeler()
        modeler.GenerateModelPart(self.main_model_part,
                                  self.laplacian_elements_part,
                                  con_pres_mod_settings)

    # def Initialize(self):
    #     self.get_mesh_motion_solving_strategy().Initialize()
    #     #self.neighbour_search.Execute()
    #     self.print_on_rank_zero("::[MeshSolverBase]:: Finished initialization.")

    # def InitializeSolutionStep(self):
    #     self.get_mesh_motion_solving_strategy().InitializeSolutionStep()

    # def FinalizeSolutionStep(self):
    #     self.get_mesh_motion_solving_strategy().FinalizeSolutionStep()

    # def Predict(self):
    #     self.get_mesh_motion_solving_strategy().Predict()

    # def SolveSolutionStep(self):
    #     self.get_mesh_motion_solving_strategy().Solve() # Calling Solve bcs this is what is currently implemented in the MeshSolverStrategies

    # def SetEchoLevel(self, level):
    #     self.get_mesh_motion_solving_strategy().SetEchoLevel(level)

    # def GetEchoLevel(self):
    #     self.get_mesh_motion_solving_strategy().GetEchoLevel()

    # def Clear(self):
    #     self.get_mesh_motion_solving_strategy().Clear()

    def _create_mesh_motion_solving_strategy(self):
        linear_solver = self.get_linear_solver()
        time_order = self.settings["time_order"].GetInt()
        reform_dofs_each_step = self.settings["reform_dofs_each_step"].GetBool()
        compute_reactions = self.settings["compute_reactions"].GetBool()
        calculate_mesh_velocities = self.settings["calculate_mesh_velocities"].GetBool()
        echo_level = self.settings["echo_level"].GetInt()
        solving_strategy = KratosMeshMoving.LaplacianMeshMovingStrategy(self.mesh_model_part,
                                                            linear_solver,
                                                            time_order,
                                                            reform_dofs_each_step,
                                                            compute_reactions,
                                                             calculate_mesh_velocities,
                                                            echo_level)
        return solving_strategy



