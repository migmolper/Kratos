//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Thomas Oberbichler
//
//  Ported from the ANurbs library (https://github.com/oberbichler/ANurbs)
//

#if !defined(KRATOS_NURBS_CURVE_SHAPE_FUNCTIONS_H_INCLUDED )
#define  KRATOS_NURBS_CURVE_SHAPE_FUNCTIONS_H_INCLUDED

// System includes

// External includes

// Project includes
#include "nurbs_utilities.h"

namespace Kratos {
///@name Kratos Classes
///@{

/// NurbsCurveShapeFunction
/* Container with memory management for an efficient computation
* of NURBS and B-Spline - 1 dimensional curve functions.
*/
class NurbsCurveShapeFunction
{
public:
    ///@name Type Definitions
    ///@{

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    NurbsCurveShapeFunction()
    {
    };

    /* 
    * Constructor using the degree u, degree v and the order of shape functions.
    *   This information is required to make an optimized memory management.
    */
    NurbsCurveShapeFunction(
        const int PolynomialDegree,
        const int DerivativeOrder)
    {
        ResizeDataContainers(PolynomialDegree, DerivativeOrder);
    }

    ///@}
    ///@name Operators
    ///@{

    double operator()(const int NonzeroControlPointIndex, const int DerivativeRow) const
    {
        return ShapeFunctionValue(NonzeroControlPointIndex, DerivativeRow);
    }

    ///@}
    ///@name Operations
    ///@{

    /* 
    * @brief Resizes the data containers which are needed to compute the
    *        values of the shape functions.
    */
    void ResizeDataContainers(
        const int PolynomialDegree,
        const int DerivativeOrder)
    {
        // Not possible to compute shape functions where derivative order
        // is higher than the polynomial degree. Those are returned as zero
        // within the access function ShapeFunctionValue
        // mDerivativeOrder = std::min(DerivativeOrder, PolynomialDegree);
        mDerivativeOrder = DerivativeOrder;

        mValues.resize((PolynomialDegree + 1) * (mDerivativeOrder + 1));
        mLeft.resize(PolynomialDegree);
        mRight.resize(PolynomialDegree);
        mNdu.resize((PolynomialDegree + 1) * (PolynomialDegree + 1));
        mA.resize(PolynomialDegree + 1);
        mB.resize(PolynomialDegree + 1);

        mPolynomialDegree = PolynomialDegree;
    }

    /*
    * @return provides the polynomial degree of this shape function generator.
    */
    int PolynomialDegree() const
    {
        return mPolynomialDegree;
    }

    /*
    * @brief the number of nonzero control points for one point on the curve is
    *        given by polynomial degree + 1.
    * @return the number of nonzero control points of this shape function generator.
    */
    int NumberOfNonzeroControlPoints() const
    {
        return PolynomialDegree() + 1;
    }

    /*
    * @return the number of shape function rows. This is the derivative order + 1.
    *         rows are defined as: N | dN/de | dN^2/de^2 | ...
    */
    int NumberOfShapeFunctionRows() const
    {
        return mDerivativeOrder + 1;
    }

    /* 
    * @brief Provides the shape function value depending to the DerivativeOrder and
    *        the index of the control point.
    */
    double ShapeFunctionValue(
        const int ControlPointIndex,
        const int DerivativeRow) const
    {
        int index = NurbsUtilities::GetVectorIndexFromMatrixIndices(
            NumberOfNonzeroControlPoints(), NumberOfShapeFunctionRows(),
            ControlPointIndex, DerivativeRow);

        if (index > mValues.size())
            return 0.0;

        return mValues[index];
    }

    /*
    * @brief the first nonzero control point index within the list.
    * @return the index of the first nonzero control point.
    */
    int GetFirstNonzeroControlPoint() const
    {
        return mFirstNonzeroControlPoint;
    }

    /*
    * @return the indices of all nonzero control points.
    */
    std::vector<int> GetNonzeroControlPointIndices() const
    {
        std::vector<int> indices(NumberOfNonzeroControlPoints());

        for (int i = 0; i < NumberOfNonzeroControlPoints(); i++) {
            indices[i] = GetFirstNonzeroControlPoint() + i;
        }

        return indices;
    }

    ///@}
    ///@name Shape Function Computation
    ///@{

    /*
    * @brief Computation of B-Spline shape function values, use this function
    *        if span of ParameterT is unknown.
    * From Piegl and Tiller, The NURBS Book, Algorithm A3.1
    * @param rKnots, the full knot span.
    * @param ParameterT, the parameter at which the shape functions
    *        are evaluated.
    */
    void ComputeBSplineShapeFunctionValues(const Vector& rKnots, const double ParameterT)
    {
        const int span = NurbsUtilities::GetUpperSpan(
            PolynomialDegree(), rKnots, ParameterT);

        ComputeBSplineShapeFunctionValuesAtSpan(rKnots, span, ParameterT);
    }

    /*
    * @brief Computation of B-Spline shape function values.
    * From Piegl and Tiller, The NURBS Book, Algorithm A2.2 and A2.3
    * @param rKnots, the full knot span.
    * @param Span, the index of the span of interest.
    * @param ParameterT, the parameter at which the shape functions
    *        are evaluated.
    */
    void ComputeBSplineShapeFunctionValuesAtSpan(
        const Vector& rKnots,
        const int Span,
        const double ParameterT)
    {
        ClearValues();

        mFirstNonzeroControlPoint = Span - PolynomialDegree() + 1;

        // compute B-Spline shape
        Ndu(0, 0) = 1.0;

        for (int j = 0; j < PolynomialDegree(); j++) {
            mLeft[j] = ParameterT - rKnots[Span - j];
            mRight[j] = rKnots[Span + j + 1] - ParameterT;

            double saved = 0.0;

            for (int r = 0; r <= j; r++) {
                Ndu(j + 1, r) = mRight[r] + mLeft[j - r];

                double temp = Ndu(r, j) / Ndu(j + 1, r);

                Ndu(r, j + 1) = saved + mRight[r] * temp;

                saved = mLeft[j - r] * temp;
            }
            Ndu(j + 1, j + 1) = saved;
        }

        for (int j = 0; j < NumberOfNonzeroControlPoints(); j++) {
            ShapeFunctionValue(j, 0) = Ndu(j, PolynomialDegree());
        }

        auto& a = mA;
        auto& b = mB;

        for (int r = 0; r < NumberOfNonzeroControlPoints(); r++) {
            a[0] = 1.0;

            for (int k = 1; k < NumberOfShapeFunctionRows(); k++) {
                double& value = ShapeFunctionValue(r, k);

                int rk = r - k;
                int pk = PolynomialDegree() - k;

                if (r >= k) {
                    b[0] = a[0] / Ndu(pk + 1, rk);
                    value = b[0] * Ndu(rk, pk);
                }

                int j1 = r >= k - 1 ? 1 : k - r;
                int j2 = r <= pk + 1 ? k : NumberOfNonzeroControlPoints() - r;

                for (int j = j1; j < j2; j++) {
                    b[j] = (a[j] - a[j - 1]) / Ndu(pk + 1, rk + j);
                    value += b[j] * Ndu(rk + j, pk);
                }

                if (r <= pk) {
                    b[k] = -a[k - 1] / Ndu(pk + 1, r);
                    value += b[k] * Ndu(r, pk);
                }

                std::swap(a, b);
            }
        }

        int s = PolynomialDegree();

        for (int k = 1; k < NumberOfShapeFunctionRows(); k++) {
            for (int j = 0; j < NumberOfNonzeroControlPoints(); j++) {
                ShapeFunctionValue(j, k) *= s;
            }
            s *= PolynomialDegree() - k;
        }
    }

    /*
    * @brief Computation of NURBS shape function values, use this function
    *        if span of ParameterT is unknown.
    * From Piegl and Tiller, The NURBS Book, Algorithm A4.2
    * @param rKnots, the full knot span.
    * @param rWeights, the complete weights of the curve.
    * @param ParameterT, the parameter at which the shape functions
    *        are evaluated.
    */
    void ComputeNurbsShapeFunctionValues(
        const Vector& rKnots,
        const Vector& rWeights,
        const double ParameterT)
    {
        const int span = NurbsUtilities::GetUpperSpan(
            PolynomialDegree(), rKnots, ParameterT);

        ComputeNurbsShapeFunctionValuesAtSpan(rKnots, span, rWeights, ParameterT);
    }

    /*
    * @brief Computation of NURBS shape function values
    * From Piegl and Tiller, The NURBS Book, Algorithm A4.2
    * @param rKnots, the full knot span.
    * @param rWeights, the complete weights of the curve.
    * @param ParameterT, the parameter at which the shape functions
    *        are evaluated.
    */
    void ComputeNurbsShapeFunctionValuesAtSpan(
        const Vector& rKnots,
        const int Span,
        const Vector& rWeights,
        const double ParameterT)
    {
        // compute B-Spline shape
        ComputeBSplineShapeFunctionValuesAtSpan(rKnots, Span, ParameterT);

        // apply weights
        
        std::vector<double> weightedSums(NumberOfShapeFunctionRows());

        for (int k = 0; k < NumberOfShapeFunctionRows(); k++) {
            weightedSums[k] = 0;

            for (int i = 0; i < NumberOfNonzeroControlPoints(); i++) {
                const size_t poleIndex = GetFirstNonzeroControlPoint() + i;
                ShapeFunctionValue(i, k) *= rWeights(poleIndex);
                weightedSums[k] += ShapeFunctionValue(i, k);
            }
        }

        for (int k = 0; k < NumberOfShapeFunctionRows(); k++) {
            for (int i = 1; i <= k; i++) {
                const double a = NurbsUtilities::GetBinomCoefficient(k, i) *
                    weightedSums[i];

                for (int p = 0; p < NumberOfNonzeroControlPoints(); p++) {
                    ShapeFunctionValue(p, k) -=
                        a * ShapeFunctionValue(p, k - i);
                }
            }

            for (int p = 0; p < NumberOfNonzeroControlPoints(); p++) {
                ShapeFunctionValue(p, k) /= weightedSums[0];
            }
        }
    }

    ///@}

private:
    ///@name Private Operations
    ///@{

    /*
    * @brief Provides the shape function value depending to the derivative row and
    *        the index of the control point.
    *        For curves the indices for derivatives are as following:
    *        0-> N | 1-> dN/de | 2-> ddN/dde | ...
    */
    double& ShapeFunctionValue(const int NonzeroControlPointIndex, const int DerivativeRow)
    {
        const int index = NurbsUtilities::GetVectorIndexFromMatrixIndices(
            NumberOfNonzeroControlPoints(), NumberOfShapeFunctionRows(),
            NonzeroControlPointIndex, DerivativeRow);

        return mValues[index];
    }

    double& Ndu(const int DerivativeRow, const int NonzeroControlPointIndex)
    {
        const int index = NurbsUtilities::GetVectorIndexFromMatrixIndices(
            NumberOfNonzeroControlPoints(), NumberOfShapeFunctionRows(), NonzeroControlPointIndex, DerivativeRow);

        return mNdu[index];
    }

    /*
    * @brief resets the shape function container.
    */
    void ClearValues()
    {
        const int number_of_values = NumberOfNonzeroControlPoints() * NumberOfShapeFunctionRows();

        mValues = ZeroVector(number_of_values);
    }

    ///@}
    ///@name Member Variables
    ///@{

    int mPolynomialDegree;
    int mDerivativeOrder;
    Vector mValues;
    Vector mLeft;
    Vector mRight;
    Vector mNdu;
    Vector mA;
    Vector mB;
    int mFirstNonzeroControlPoint;

    ///@}
    ///@name Private Serialization
    ///@{

    friend class Serializer;

    void save(Serializer& rSerializer) const
    {
        rSerializer.save("PolynomialDegree", mPolynomialDegree);
        rSerializer.save("DerivativeOrder", mDerivativeOrder);
        rSerializer.save("Values", mValues);
        rSerializer.save("Left", mLeft);
        rSerializer.save("Right", mRight);
        rSerializer.save("Ndu", mNdu);
        rSerializer.save("A", mA);
        rSerializer.save("B", mB);
        rSerializer.save("FirstNonzeroControlPoint", mFirstNonzeroControlPoint);
    }

    void load(Serializer& rSerializer)
    {
        rSerializer.load("PolynomialDegree", mPolynomialDegree);
        rSerializer.load("DerivativeOrder", mDerivativeOrder);
        rSerializer.load("Values", mValues);
        rSerializer.load("Left", mLeft);
        rSerializer.load("Right", mRight);
        rSerializer.load("Ndu", mNdu);
        rSerializer.load("A", mA);
        rSerializer.load("B", mB);
        rSerializer.load("FirstNonzeroControlPoint", mFirstNonzeroControlPoint);
    }

    ///@}
};
///@}

} // namespace Kratos

#endif // KRATOS_NURBS_CURVE_SHAPE_FUNCTIONS_H_INCLUDED defined