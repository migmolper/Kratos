/*
==============================================================================
KratosMultiScaleApplication
A library based on:
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi, Janosch Stascheit, Felix Nagel
pooyan@cimne.upc.edu
rrossi@cimne.upc.edu
janosch.stascheit@rub.de
nagel@sd.rub.de
- CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain
- Ruhr-University Bochum, Institute for Structural Mechanics, Germany


Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

The  above  copyright  notice  and  this permission  notice  shall  be
included in all copies or substantial portions of the Software.

THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==============================================================================
*/
//
//   Project Name:        Kratos
//   Last Modified by:    $Author: Zhiming $
//   Date:                $Date: 2014-6-05 $
//   Revision:            $Revision: 1.00 $
//
//


#if !defined(KRATOS_LAGRANGE_MULTIPLIER_CONDITION_2D_H_INCLUDED )
#define  KRATOS_LAGRANGE_MULTIPLIER_CONDITION_2D_H_INCLUDED



// System includes


// External includes
#include "boost/smart_ptr.hpp"


// Project includes
#include "includes/define.h"
#include "includes/serializer.h"
#include "includes/condition.h"
#include "includes/ublas_interface.h"
#include "includes/variables.h"
#include "includes/model_part.h"


#include "includes/node.h"
#include "includes/dof.h"

namespace Kratos
{

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/// Short class definition.
/** Detail class definition.
*/
class LagrangeMultiplierCondition2D
    : public Condition
{
public:
    ///@name Type Definitions
    ///@{

    /// Counted pointer of LagrangeMultiplierCondition2D
    KRATOS_CLASS_POINTER_DEFINITION(LagrangeMultiplierCondition2D);



    ///@}
	
    ///@name Life Cycle
    ///@{

    LagrangeMultiplierCondition2D(){}

    LagrangeMultiplierCondition2D(IndexType NewId, GeometryType::Pointer pGeometry);

    LagrangeMultiplierCondition2D(IndexType NewId, GeometryType::Pointer pGeometry,  PropertiesType::Pointer pProperties);
	

	
    virtual ~LagrangeMultiplierCondition2D();

    ///@}
	
    ///@name Operators
    ///@{
    ///@}
	
    ///@name Operations
    ///@{

	/// methods derived from Condition
	
    Condition::Pointer Create(IndexType NewId, NodesArrayType const& ThisNodes,  PropertiesType::Pointer pProperties) const;

    void CalculateLocalSystem(MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo);

    void CalculateRightHandSide(VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo);

    void EquationIdVector(EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo);

    void GetDofList(DofsVectorType& ConditionalDofList,ProcessInfo& CurrentProcessInfo);

	int Check(const ProcessInfo& rCurrentProcessInfo);

    void FinalizeSolutionStep(ProcessInfo& CurrentProcessInfo);

    ///@}
	
    ///@name Access
    ///@{
    ///@}
	
    ///@name Inquiry
    ///@{
    ///@}
	
    ///@name Input and output
    ///@{
    ///@}
	
    ///@name Static
    ///@{
    ///@}

protected:
    ///@name Protected static Member Variables
    ///@{
    ///@}
	
    ///@name Protected member Variables
    ///@{
    ///@}
	
    ///@name Protected Operators
    ///@{
    ///@}
	
    ///@name Protected Operations
    ///@{
    ///@}
	
    ///@name Protected  Access
    ///@{
    ///@}
	
    ///@name Protected Inquiry
    ///@{
    ///@}
	
    ///@name Protected LifeCycle
    ///@{
    ///@}

private:
    ///@name Static Member Variables
    ///@{
    ///@}
	
    ///@name Member Variables
    ///@{	
    ///@}
	
    ///@name Private Operators
    ///@{
    ///@}
	
    ///@name Private Operations
    ///@{
    double incre_dis;


	friend class Serializer;

    virtual void save(Serializer& rSerializer) const
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Condition );
    }

    virtual void load(Serializer& rSerializer)
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Condition );
    }

    ///@}
	
    ///@name Private  Access
    ///@{
    ///@}
	
    ///@name Private Inquiry
    ///@{
    ///@}
	
    ///@name Un accessible methods
    ///@{
    ///@}

}; // Class LagrangeMultiplierCondition2D

///@}

///@name Type Definitions
///@{
///@}

///@name Input and output
///@{
///@}

}  // namespace Kratos.

#endif // KRATOS_LAGRANGE_MULTIPLIER_CONDITION_2D_H_INCLUDED


