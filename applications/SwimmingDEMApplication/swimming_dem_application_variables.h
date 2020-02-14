//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Carlos A. Roig
//
//

#ifndef KRATOS_SWIMMING_DEM_APPLICATION_VARIABLES_H
#define	KRATOS_SWIMMING_DEM_APPLICATION_VARIABLES_H

#include "includes/define.h"
#include "includes/variables.h"
#include "includes/kratos_application.h"
#include "includes/legacy_structural_app_vars.h"

namespace Kratos {

    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SWIMMING_DEM_APPLICATION, VECTORIAL_ERROR)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SWIMMING_DEM_APPLICATION, EXACT_VELOCITY)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SWIMMING_DEM_APPLICATION, AVERAGED_FLUID_VELOCITY)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, double, EXACT_PRESSURE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, double, SCALAR_ERROR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, double, ERROR_X)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, double, ERROR_Y)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, double, ERROR_Z)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, std::string, SDEM_HYDRODYNAMIC_INTERACTION_LAW_NAME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, std::string, SDEM_BUOYANCY_LAW_NAME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, std::string, SDEM_DRAG_LAW_NAME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, std::string, SDEM_INVISCID_FORCE_LAW_NAME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, std::string, SDEM_HISTORY_FORCE_LAW_NAME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, std::string, SDEM_VORTICITY_LIFT_LAW_NAME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SWIMMING_DEM_APPLICATION, std::string, SDEM_STEADY_VISCOUS_TORQUE_LAW_NAME)
}

#endif // KRATOS_SWIMMING_DEM_APPLICATION_VARIABLES_H  defined