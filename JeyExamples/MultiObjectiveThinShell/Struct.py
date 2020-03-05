from __future__ import print_function, absolute_import, division #makes KratosMultiphysics backward compatible with python 2.6 and 2.7

import KratosMultiphysics as KM
from KratosMultiphysics.StructuralMechanicsApplication.structural_mechanics_analysis import StructuralMechanicsAnalysis
from KratosMultiphysics.ShapeOptimizationApplication import optimizer_factory
from KratosMultiphysics.ShapeOptimizationApplication.analyzer_base import AnalyzerBaseClass
import KratosMultiphysics.StructuralMechanicsApplication as StructuralMechanicsApplication
from KratosMultiphysics.StructuralMechanicsApplication import structural_response
from KratosMultiphysics.StructuralMechanicsApplication.structural_mechanics_analysis import StructuralMechanicsAnalysis
from KratosMultiphysics import Parameters, Logger
from KratosMultiphysics.analysis_stage import AnalysisStage
import KratosMultiphysics.kratos_utilities as kratos_utilities
import time as timer
import shutil
import glob, os

if __name__ == "__main__":

    #Read parameter (VERTICAL LOAD [0, 0, -1]) Optimized MDPA (plateR)
    with open("ProjectParameters5.json",'r') as parameter_file:
        parameters = KM.Parameters(parameter_file.read())

    model = KM.Model()
    simulation = StructuralMechanicsAnalysis(model,parameters)
    simulation.Run()

    # Cleaning
    kratos_utilities.DeleteDirectoryIfExisting("__pycache__")
    response_combination_filename = "response_combination.csv"
    kratos_utilities.DeleteFileIfExisting(response_combination_filename)
