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
import time as timer
import shutil
import glob, os

if __name__ == "__main__":

    # with open("ProjectParameters1.json",'r') as parameter_file:
    #     parameters = KM.Parameters(parameter_file.read())

    # model = KM.Model()
    # simulation = StructuralMechanicsAnalysis(model,parameters)
    # simulation.Run()

    # with open("ProjectParameters2.json",'r') as parameter_file:
    #     parameters = KM.Parameters(parameter_file.read())

    # model = KM.Model()
    # simulation = StructuralMechanicsAnalysis(model,parameters)
    # simulation.Run()

    # =====================Multi-Objective-Load 1=================================
    # Read parameters (Optimization)
    with open("optimization_parameters.json",'r') as parameter_file:
        parameters = KM.Parameters(parameter_file.read())

    model = KM.Model()

    # Create optimizer and perform optimization
    
    optimizer = optimizer_factory.CreateOptimizer(parameters["optimization_settings"], model)
    optimizer.Optimize()
