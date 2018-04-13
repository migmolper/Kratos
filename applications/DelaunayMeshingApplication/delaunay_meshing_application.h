//
//   Project Name:        KratosDelaunayMeshingApplication $
//   Created by:          $Author:             JMCarbonell $
//   Last modified by:    $Co-Author:                      $
//   Date:                $Date:                April 2018 $
//   Revision:            $Revision:                   0.0 $
//
//


#if !defined(KRATOS_DELAUNAY_MESHING_APPLICATION_H_INCLUDED )
#define  KRATOS_DELAUNAY_MESHING_APPLICATION_H_INCLUDED


// System includes
#include <string>
#include <iostream>


// External includes


// Project includes
#include "includes/define.h"
#include "includes/kratos_application.h"
#include "includes/variables.h"


namespace Kratos {

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
class KratosDelaunayMeshingApplication : public KratosApplication {
 public:
  ///@name Type Definitions
  ///@{


  /// Pointer definition of KratosDelaunayMeshingApplication
  KRATOS_CLASS_POINTER_DEFINITION(KratosDelaunayMeshingApplication);

  ///@}
  ///@name Life Cycle
  ///@{

  /// Default constructor.
  KratosDelaunayMeshingApplication();

  /// Destructor.
  virtual ~KratosDelaunayMeshingApplication(){}


  ///@}
  ///@name Operators
  ///@{


  ///@}
  ///@name Operations
  ///@{

  virtual void Register();



  ///@}
  ///@name Access
  ///@{


  ///@}
  ///@name Inquiry
  ///@{


  ///@}
  ///@name Input and output
  ///@{

  /// Turn back information as a string.
  virtual std::string Info() const {
    return "KratosDelaunayMeshingApplication";
  }

  /// Print information about this object.
  virtual void PrintInfo(std::ostream& rOStream) const {
    rOStream << Info();
    PrintData(rOStream);
  }

  ///// Print object's data.
  virtual void PrintData(std::ostream& rOStream) const {
    KRATOS_WATCH("in my application");
    KRATOS_WATCH(KratosComponents<VariableData>::GetComponents().size() );

    rOStream << "Variables:" << std::endl;
    KratosComponents<VariableData>().PrintData(rOStream);
    rOStream << std::endl;
    rOStream << "Elements:" << std::endl;
    KratosComponents<Element>().PrintData(rOStream);
    rOStream << std::endl;
    rOStream << "Conditions:" << std::endl;
    KratosComponents<Condition>().PrintData(rOStream);
  }


  ///@}
  ///@name Friends
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

  // static const ApplicationCondition  msApplicationCondition;

  ///@}
  ///@name Member Variables
  ///@{

  // const Elem2D   mElem2D;
  // const Elem3D   mElem3D;

  ///@}
  ///@name Private Operators
  ///@{


  ///@}
  ///@name Private Operations
  ///@{


  ///@}
  ///@name Private  Access
  ///@{


  ///@}
  ///@name Private Inquiry
  ///@{


  ///@}
  ///@name Un accessible methods
  ///@{

  /// Assignment operator.
  KratosDelaunayMeshingApplication& operator=(KratosDelaunayMeshingApplication const& rOther);

  /// Copy constructor.
  KratosDelaunayMeshingApplication(KratosDelaunayMeshingApplication const& rOther);


  ///@}

}; // Class KratosDelaunayMeshingApplication

///@}


///@name Type Definitions
///@{


///@}
///@name Input and output
///@{

///@}


}  // namespace Kratos.

#endif // KRATOS_DELAUNAY_MESHING_APPLICATION_H_INCLUDED  defined
