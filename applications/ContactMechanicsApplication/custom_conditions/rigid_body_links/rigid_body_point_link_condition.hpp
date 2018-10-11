//
//   Project Name:        KratosContactMechanicsApplication $
//   Created by:          $Author:              JMCarbonell $
//   Last modified by:    $Co-Author:                       $
//   Date:                $Date:             September 2018 $
//   Revision:            $Revision:                    0.0 $
//
//

#if !defined(KRATOS_RIGID_BODY_POINT_LINK_CONDITION_H_INCLUDED )
#define  KRATOS_RIGID_BODY_POINT_LINK_CONDITION_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/condition.h"
#include "includes/variables.h"
#include "includes/element.h"

#include "utilities/quaternion.h"

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

/// Rigid Body Point Rigid Contact Condition for 3D and 2D geometries. (base class)

/**
 * Implements a Contact Point Load definition for structural analysis.
 * This works for arbitrary geometries in 3D and 2D (base class)
 */
class RigidBodyPointLinkCondition
    : public Condition
{
 public:

  ///@name Type Definitions

  ///Tensor order 1 definition
  typedef Vector                        VectorType;
  typedef Element                      ElementType;
  typedef Node<3>::Pointer        PointPointerType;
  typedef Quaternion<double>        QuaternionType;

  ///@{
  // Counted pointer of RigidBodyPointLinkCondition
  KRATOS_CLASS_POINTER_DEFINITION( RigidBodyPointLinkCondition );
  ///@}

 protected:


  /**
   * Flags related to the condition computation
   */
  KRATOS_DEFINE_LOCAL_FLAG(COMPUTE_RHS_VECTOR);
  KRATOS_DEFINE_LOCAL_FLAG(COMPUTE_LHS_MATRIX);

  /**
   * Parameters to be used in the Condition as they are.
   */
  typedef struct
  {
    Flags             Options;               //calculation options
    int               SlaveNode;

    Vector            Distance;
    Matrix            SkewSymDistance;

    Vector            LagrangeMultipliers;

    ElementType::Pointer  pSlaveElement;

  } GeneralVariables;

  /**
   * This struct is used in the component wise calculation only
   * is defined here and is used to declare a member variable in the component wise condition
   * private pointers can only be accessed by means of set and get functions
   * this allows to set and not copy the local system variables
   */

  struct LocalSystemComponents
  {
   private:

    //for calculation local system with compacted LHS and RHS
    MatrixType *mpLeftHandSideMatrix;
    VectorType *mpRightHandSideVector;

   public:

    //calculation flags
    Flags  CalculationFlags;

    /**
     * sets the value of a specified pointer variable
     */
    void SetLeftHandSideMatrix( MatrixType& rLeftHandSideMatrix ) { mpLeftHandSideMatrix = &rLeftHandSideMatrix; };
    void SetRightHandSideVector( VectorType& rRightHandSideVector ) { mpRightHandSideVector = &rRightHandSideVector; };

    /**
     * returns the value of a specified pointer variable
     */
    MatrixType& GetLeftHandSideMatrix() { return *mpLeftHandSideMatrix; };
    VectorType& GetRightHandSideVector() { return *mpRightHandSideVector; };

  };


 public:


  ///@name Life Cycle
  ///@{

  /// Default constructor.
  RigidBodyPointLinkCondition( IndexType NewId, GeometryType::Pointer pGeometry );

  RigidBodyPointLinkCondition( IndexType NewId, GeometryType::Pointer pGeometry, PropertiesType::Pointer pProperties );

  /// Copy constructor
  RigidBodyPointLinkCondition( RigidBodyPointLinkCondition const& rOther);

  /// Destructor
  virtual ~RigidBodyPointLinkCondition();

  ///@}
  ///@name Operators
  ///@{


  ///@}
  ///@name Operations
  ///@{

  /**
   * creates a new condition pointer
   * @param NewId: the ID of the new condition
   * @param ThisNodes: the nodes of the new condition
   * @param pProperties: the properties assigned to the new condition
   * @return a Pointer to the new condition
   */
  Condition::Pointer Create(IndexType NewId,
                            NodesArrayType const& ThisNodes,
                            PropertiesType::Pointer pProperties ) const override;
  /**
   * clones the selected condition variables, creating a new one
   * @param NewId: the ID of the new condition
   * @param ThisNodes: the nodes of the new condition
   * @param pProperties: the properties assigned to the new condition
   * @return a Pointer to the new condition
   */
  Condition::Pointer Clone(IndexType NewId,
                           NodesArrayType const& ThisNodes) const override;

  //************* STARTING - ENDING  METHODS

  /**
   * is called to initialize the condition
   * if the condition needs to perform any operation before any calculation is done
   * the condition variables will be initialized and set using this method
   */
  void Initialize();

  /**
   * Called at the beginning of each iteration
   */
  void InitializeNonLinearIteration(ProcessInfo& rCurrentProcessInfo) override;

  /**
   * Called at the end of each iteration
   */
  void FinalizeNonLinearIteration(ProcessInfo& rCurrentProcessInfo) override;

  /**
   * Called at the beginning of each solution step
   */
  void InitializeSolutionStep(ProcessInfo& rCurrentProcessInfo) override;

  /**
   * Called at the end of each solution step
   */
  void FinalizeSolutionStep(ProcessInfo& rCurrentProcessInfo) override;

  //************* GETTING METHODS

  /**
   * Sets on rConditionDofList the degrees of freedom of the considered element geometry
   */
  void GetDofList(DofsVectorType& rConditionDofList,
                  ProcessInfo& rCurrentProcessInfo ) override;

  /**
   * Sets on rResult the ID's of the element degrees of freedom
   */
  void EquationIdVector(EquationIdVectorType& rResult,
                        ProcessInfo& rCurrentProcessInfo ) override;

  /**
   * Sets on rValues the nodal displacements
   */
  void GetValuesVector(Vector& rValues,
                       int Step = 0 ) override;

  /**
   * Sets on rValues the nodal velocities
   */
  void GetFirstDerivativesVector(Vector& rValues,
                                 int Step = 0 ) override;

  /**
   * Sets on rValues the nodal accelerations
   */
  void GetSecondDerivativesVector(Vector& rValues,
                                  int Step = 0 ) override;


  //************* COMPUTING  METHODS

  /**
   * this is called during the assembling process in order
   * to calculate all condition contributions to the global system
   * matrix and the right hand side
   * @param rLeftHandSideMatrix: the condition left hand side matrix
   * @param rRightHandSideVector: the condition right hand side
   * @param rCurrentProcessInfo: the current process info instance
   */
  void CalculateLocalSystem(MatrixType& rLeftHandSideMatrix,
                            VectorType& rRightHandSideVector,
                            ProcessInfo& rCurrentProcessInfo) override;

  /**
   * this is called during the assembling process in order
   * to calculate the condition right hand side vector only
   * @param rRightHandSideVector: the condition right hand side vector
   * @param rCurrentProcessInfo: the current process info instance
   */
  void CalculateRightHandSide(VectorType& rRightHandSideVector,
                              ProcessInfo& rCurrentProcessInfo) override;

  /**
   * this is called during the assembling process in order
   * to calculate the second derivatives contributions for the LHS and RHS
   * @param rLeftHandSideMatrix: the condition left hand side matrix
   * @param rRightHandSideVector: the condition right hand side
   * @param rCurrentProcessInfo: the current process info instance
   */
  void CalculateSecondDerivativesContributions(MatrixType& rLeftHandSideMatrix,
                                               VectorType& rRightHandSideVector,
                                               ProcessInfo& rCurrentProcessInfo) override;

  /**
   * this is called during the assembling process in order
   * to calculate the condition left hand side matrix for the second derivatives constributions
   * @param rLeftHandSideMatrix: the condition left hand side matrix
   * @param rCurrentProcessInfo: the current process info instance
   */
  void CalculateSecondDerivativesLHS(MatrixType& rLeftHandSideMatrix,
                                     ProcessInfo& rCurrentProcessInfo) override;


  /**
   * this is called during the assembling process in order
   * to calculate the condition right hand side vector for the second derivatives constributions
   * @param rRightHandSideVector: the condition right hand side vector
   * @param rCurrentProcessInfo: the current process info instance
   */
  void CalculateSecondDerivativesRHS(VectorType& rRightHandSideVector,
                                     ProcessInfo& rCurrentProcessInfo) override;

  /**
   * this is called during the assembling process in order
   * to calculate the condition mass matrix
   * @param rMassMatrix: the condition mass matrix
   * @param rCurrentProcessInfo: the current process info instance
   */
  void CalculateMassMatrix(MatrixType& rMassMatrix,
                           ProcessInfo& rCurrentProcessInfo) override;

  /**
   * this is called during the assembling process in order
   * to calculate the condition damping matrix
   * @param rDampingMatrix: the condition damping matrix
   * @param rCurrentProcessInfo: the current process info instance
   */
  void CalculateDampingMatrix(MatrixType& rDampingMatrix,
                              ProcessInfo& rCurrentProcessInfo) override;

  /**
   * this function is designed to make the element to assemble an rRHS vector
   * identified by a variable rRHSVariable by assembling it to the nodes on the variable
   * rDestinationVariable.
   * @param rRHSVector: input variable containing the RHS vector to be assembled
   * @param rRHSVariable: variable describing the type of the RHS vector to be assembled
   * @param rDestinationVariable: variable in the database to which the rRHSvector will be assembled
   * @param rCurrentProcessInfo: the current process info instance
   */
  virtual void AddExplicitContribution(const VectorType& rRHSVector,
                                       const Variable<VectorType>& rRHSVariable,
                                       Variable<array_1d<double,3> >& rDestinationVariable,
                                       const ProcessInfo& rCurrentProcessInfo) override;

  //************************************************************************************
  //************************************************************************************
  /**
   * This function provides the place to perform checks on the completeness of the input.
   * It is designed to be called only once (or anyway, not often) typically at the beginning
   * of the calculations, so to verify that nothing is missing from the input
   * or that no common error is found.
   * @param rCurrentProcessInfo
   */
  virtual int Check(const ProcessInfo& rCurrentProcessInfo) override;


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
  ///@name Friends
  ///@{
  ///@}

 protected:
  ///@name Protected static Member Variables
  ///@{
  ///@}
  ///@name Protected member Variables
  ///@{

  RigidBodyPointLinkCondition() {};

  ///@}
  ///@name Protected Operators
  ///@{

  ///@}
  ///@name Protected Operations
  ///@{

  /**
   * Initialize System Matrices
   */
  virtual void InitializeSystemMatrices(MatrixType& rLeftHandSideMatrix,
                                        VectorType& rRightHandSideVector,
                                        const unsigned int& rSlaveElementSize,
                                        Flags& rCalculationFlags);
  /**
   * Initialize General Variables
   */
  virtual void InitializeGeneralVariables(GeneralVariables& rVariables,
                                          const ProcessInfo& rCurrentProcessInfo);
  /**
   * Calculates the condition contributions
   */
  virtual void CalculateConditionSystem(LocalSystemComponents& rLocalSystem,
                                        LocalSystemComponents& rLinkedSystem,
                                        ElementType::Pointer& rSlaveElement,
                                        ProcessInfo& rCurrentProcessInfo);
  /**
   * Calculation and addition of the matrices of the LHS
   */
  virtual void CalculateAndAddLHS(LocalSystemComponents& rLocalSystem,
                                  LocalSystemComponents& rLinkedSystem,
                                  GeneralVariables& rVariables);
  /**
   * Calculation and addition of the vectors of the RHS
   */
  virtual void CalculateAndAddRHS(LocalSystemComponents& rLocalSystem,
                                  LocalSystemComponents& rLinkedSystem,
                                  GeneralVariables& rVariables);
  /**
   * Calculation of the Link Stiffness Matrix
   */
  virtual void CalculateAndAddStiffness(MatrixType& rLeftHandSideMatrix,
                                        MatrixType& rLinkedLeftHandSideMatrix,
                                        GeneralVariables& rVariables);
  /**
   * Calculation of the Link Force Vector
   */
  virtual void CalculateAndAddForces(VectorType& rRightHandSideVector,
                                     VectorType& rLinkedRightHandSideVector,
                                     GeneralVariables& rVariables);
  /**
   * Calculation of an SkewSymmetricTensor from a vector
   */
  void VectorToSkewSymmetricTensor(const Vector& rVector,
                                   Matrix& rSkewSymmetricTensor);
  /**
   * Write Matrix usint rows
   */
  void WriteMatrixInRows(std::string MatrixName,
                         const MatrixType& rMatrix);
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

  ///@}
  ///@name Private  Access
  ///@{

  ///@}
  ///@name Private Inquiry
  ///@{

  ///@}
  ///@name Serialization
  ///@{

  friend class Serializer;

  virtual void save( Serializer& rSerializer ) const
  {
    KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, Condition )
  }

  virtual void load( Serializer& rSerializer )
  {
    KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, Condition )
  }


}; // class RigidBodyPointLinkCondition.

} // namespace Kratos.

#endif // KRATOS_RIGID_BODY_POINT_LINK_CONDITION_H_INCLUDED defined
