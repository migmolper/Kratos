# Importing the Kratos Library
import KratosMultiphysics
import KratosMultiphysics.mpi as KratosMPI                          # MPI-python interface

# Import applications
import KratosMultiphysics.MetisApplication                          # Mesh artitioning (always import before Trilinos)
import KratosMultiphysics.TrilinosApplication as KratosTrilinos     # MPI solvers
from KratosMultiphysics.FluidDynamicsApplication import TrilinosExtension as TrilinosFluid
from KratosMultiphysics.TrilinosApplication import trilinos_linear_solver_factory

# Import base class file
from KratosMultiphysics.FluidDynamicsApplication.navier_stokes_solver_fractionalstep import NavierStokesSolverFractionalStep
from KratosMultiphysics.mpi.distributed_import_model_part_utility import DistributedImportModelPartUtility

def CreateSolver(model, custom_settings):
    return TrilinosNavierStokesSolverFractionalStep(model, custom_settings)

class TrilinosNavierStokesSolverFractionalStep(NavierStokesSolverFractionalStep):

    @classmethod
    def GetDefaultSettings(cls):
        ## Default settings string in Json format
        default_settings = KratosMultiphysics.Parameters("""
        {
            "solver_type": "FractionalStep",
            "model_part_name": "",
            "domain_size": -1,
            "model_import_settings": {
                    "input_type": "mdpa",
                    "input_filename": "unknown_name"
            },
            "material_import_settings": {
                "materials_filename": ""
            },
            "predictor_corrector": false,
            "maximum_velocity_iterations": 3,
            "maximum_pressure_iterations": 3,
            "velocity_tolerance": 1e-3,
            "pressure_tolerance": 1e-2,
            "dynamic_tau": 0.01,
            "oss_switch": 0,
            "echo_level": 0,
            "consider_periodic_conditions": false,
            "time_order": 2,
            "compute_reactions": false,
            "reform_dofs_at_each_step": false,
            "pressure_linear_solver_settings": {
                "solver_type": "amgcl"
            },
            "velocity_linear_solver_settings": {
                "solver_type": "amgcl"
            },
            "volume_model_part_name" : "volume_model_part",
            "skin_parts":[""],
            "no_skin_parts":[""],
            "time_stepping": {
                "automatic_time_step" : false,
                "CFL_number"          : 1,
                "minimum_delta_time"  : 1e-4,
                "maximum_delta_time"  : 0.01
            },
            "move_mesh_flag": false,
            "use_slip_conditions": true
        }""")

        default_settings.AddMissingParameters(super(TrilinosNavierStokesSolverFractionalStep, cls).GetDefaultSettings())
        return default_settings

    def __init__(self, model, custom_settings):
        self._validate_settings_in_baseclass=True # To be removed eventually
        # Note: deliberately calling the constructor of the base python solver (the parent of my parent)
        super(NavierStokesSolverFractionalStep,self).__init__(model,custom_settings)

        self.element_name = "FractionalStep"
        self.condition_name = "WallCondition"
        self.min_buffer_size = 3
        self.element_has_nodal_properties = True

        self.compute_reactions = self.settings["compute_reactions"].GetBool()

        self.main_model_part.ProcessInfo.SetValue(KratosMultiphysics.OSS_SWITCH, self.settings["oss_switch"].GetInt())
        self.main_model_part.ProcessInfo.SetValue(KratosMultiphysics.DYNAMIC_TAU, self.settings["dynamic_tau"].GetDouble())

        KratosMultiphysics.Logger.PrintInfo(self.__class__.__name__,"Construction of TrilinosNavierStokesSolverFractionalStep solver finished.")


    def AddVariables(self):
        ## Add variables from the base class
        super(TrilinosNavierStokesSolverFractionalStep, self).AddVariables()

        ## Add specific MPI variables
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.PARTITION_INDEX)

        KratosMultiphysics.Logger.PrintInfo(self.__class__.__name__,"variables for the trilinos fractional step solver added correctly")


    def ImportModelPart(self):
        ## Construct the MPI import model part utility
        self.distributed_model_part_importer = DistributedImportModelPartUtility(self.main_model_part, self.settings)
        ## Execute the Metis partitioning and reading
        self.distributed_model_part_importer.ImportModelPart()

        KratosMultiphysics.Logger.PrintInfo(self.__class__.__name__,"MPI model reading finished.")

    def PrepareModelPart(self):
        super(TrilinosNavierStokesSolverFractionalStep,self).PrepareModelPart()
        ## Construct MPI communicators
        self.distributed_model_part_importer.CreateCommunicators()


    def AddDofs(self):
        ## Base class DOFs addition
        super(TrilinosNavierStokesSolverFractionalStep, self).AddDofs()

        KratosMultiphysics.Logger.PrintInfo(self.__class__.__name__,"DOFs for the VMS Trilinos fluid solver added correctly in all processors.")


    def Initialize(self):
        ## Base class solver intiialization
        super(TrilinosNavierStokesSolverFractionalStep, self).Initialize()

        KratosMultiphysics.Logger.PrintInfo(self.__class__.__name__, "Solver initialization finished.")

    def Finalize(self):
        self._GetSolutionStrategy().Clear()

    def _GetEpetraCommunicator(self):
        if not hasattr(self, '_epetra_communicator'):
            self._epetra_communicator = KratosTrilinos.CreateCommunicator()
        return self._epetra_communicator

    def _CreateScheme(self):
        pass

    def _CreateLinearSolver(self):
        # Create the pressure linear solver
        pressure_linear_solver_configuration = self.settings["pressure_linear_solver_settings"]
        pressure_linear_solver = trilinos_linear_solver_factory.ConstructSolver(pressure_linear_solver_configuration)
        # Create the velocity linear solver
        velocity_linear_solver_configuration = self.settings["velocity_linear_solver_settings"]
        velocity_linear_solver = trilinos_linear_solver_factory.ConstructSolver(velocity_linear_solver_configuration)
        # Return a tuple containing both linear solvers
        return (pressure_linear_solver, velocity_linear_solver)

    def _CreateConvergenceCriterion(self):
        pass

    def _CreateBuilderAndSolver(self):
        pass

    def _CreateSolutionStrategy(self):
        computing_model_part = self.GetComputingModelPart()
        epetra_communicator = self._GetEpetraCommunicator()
        domain_size = computing_model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE]

        # Create the pressure and velocity linear solvers
        # Note that linear_solvers is a tuple. The first item is the pressure
        # linear solver. The second item is the velocity linear solver.
        linear_solvers = self._GetLinearSolver()

        # Create the fractional step settings instance
        # TODO: next part would be much cleaner if we passed directly the parameters to the c++
        if self.settings["consider_periodic_conditions"].GetBool():
            fractional_step_settings = TrilinosFluid.TrilinosFractionalStepSettingsPeriodic(
                epetra_communicator,
                computing_model_part,
                domain_size,
                self.settings["time_order"].GetInt(),
                self.settings["use_slip_conditions"].GetBool(),
                self.settings["move_mesh_flag"].GetBool(),
                self.settings["reform_dofs_at_each_step"].GetBool(),
                KratosCFD.PATCH_INDEX)
        else:
            fractional_step_settings = TrilinosFluid.TrilinosFractionalStepSettings(
                epetra_communicator,
                computing_model_part,
                domain_size,
                self.settings["time_order"].GetInt(),
                self.settings["use_slip_conditions"].GetBool(),
                self.settings["move_mesh_flag"].GetBool(),
                self.settings["reform_dofs_at_each_step"].GetBool())

        # Set the strategy echo level
        fractional_step_settings.SetEchoLevel(self.settings["echo_level"].GetInt())

        # Set the velocity and pressure fractional step strategy settings
        fractional_step_settings.SetStrategy(TrilinosFluid.TrilinosStrategyLabel.Pressure,
            linear_solvers[0],
            self.settings["pressure_tolerance"].GetDouble(),
            self.settings["maximum_pressure_iterations"].GetInt())

        fractional_step_settings.SetStrategy(TrilinosFluid.TrilinosStrategyLabel.Velocity,
            linear_solvers[1],
            self.settings["velocity_tolerance"].GetDouble(),
            self.settings["maximum_velocity_iterations"].GetInt())

        # Create the fractional step strategy
        if self.settings["consider_periodic_conditions"].GetBool():
            solution_strategy = TrilinosFluid.TrilinosFSStrategy(
                computing_model_part,
                fractional_step_settings,
                self.settings["predictor_corrector"].GetBool(),
                KratosCFD.PATCH_INDEX)
        else:
            solution_strategy = TrilinosFluid.TrilinosFSStrategy(
                computing_model_part,
                fractional_step_settings,
                self.settings["predictor_corrector"].GetBool())

        return solution_strategy
