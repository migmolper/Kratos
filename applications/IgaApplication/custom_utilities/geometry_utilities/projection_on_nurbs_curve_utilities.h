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
//                   Tobias Teschemacher
//

#if !defined(KRATOS_PROJECTION_ON_NURBS_CURVE_UTILITIES_H_INCLUDED)
#define KRATOS_PROJECTION_ON_NURBS_CURVE_UTILITIES_H_INCLUDED

// System includes

// External includes

namespace Kratos
{
    namespace ProjectionOnNurbsCurveUtilities
    {

		static bool ClosestPointToLine(
			double& rPointParameter,
			const array_1d<double, 3>& rPoint,
			const array_1d<double, 3>& rStartPoint,
			const array_1d<double, 3>& rEndPoint,
			const double& rStartParameter,
			const double& rEndParameter,
			const double& rAccuracy
		)
		{
			array_1d<double, 3> segment_vector = rEndPoint - rStartPoint;
			double segment_length = norm_2(segment_vector);

			array_1d<double, 3> segment_normal = segment_vector * (1.0 / segment_length);
			array_1d<double, 3> point_start_vector = rPoint - rStartPoint;
			double dot_product = inner_prod(point_start_vector, segment_normal);

			if (dot_product < 0) {
				rPointParameter = rStartParameter;
				return false;
			}

			if (dot_product > segment_length) {
				rPointParameter = rEndParameter;
				return false;
			}

			rPointParameter = rStartParameter + (rEndParameter - rStartParameter) * dot_product / segment_length;
			return true;
		}

		//template <int TDimension>
		static bool NewtonRaphson(
			double& rParameter,
			const double rInitialGuessParameter,
			array_1d<double, 3>& rPoint,
			const std::shared_ptr<CurveOnSurface<3>>& pCurve,
			const int MaxIterations,
			const double ModelTolerance,
			const double Accuracy)
		{
			rParameter = rInitialGuessParameter;


			double min = pCurve->Domain().Min();
			double max = pCurve->Domain().Max();


			for (int i = 0; i < MaxIterations; i++)
			{
				auto derivatives = pCurve->DerivativesAt(rParameter, 2);


				array_1d<double, 3> distance_vector = derivatives[0] - rPoint;
				double distance = norm_2(distance_vector);
				double c2n = inner_prod(derivatives[1], distance_vector);
				double c2d = norm_2(derivatives[1]) * distance;
				double c2v = c2d != 0
					? c2n / c2d
					: 0;

				// Break condition
				if (distance < ModelTolerance
					&& std::abs(c2v) < Accuracy)
					return true;
				double delta_t = inner_prod(derivatives[1], distance_vector)
					/ (inner_prod(derivatives[2], distance_vector) + pow(norm_2(derivatives[1]), 2));

				rParameter -= delta_t;

				//Alternative if (rNurbsCurve.DomainInterval().IsInside())
				if (rParameter < min || rParameter > max)
				{
					return false;
				}
			}
			return false;
		}
	}
    //template <int TDimension>
    //static bool NewtonRaphsonSurface(
    //    const CoordinatesArrayType rInitialGuessParameter,
    //    const CoordinatesArrayType& rPoint,
    //    CoordinatesArrayType& rResult,
    //    const NurbsSurfaceGeometry<TDimension>& rNurbsSurface,
    //    const int MaxIterations,
    //    const double ModelTolerance,
    //    const double Accuracy)
    //{
    //    double rPoint[0] = rInitialGuessParameter[0];
    //    double rPoint[1] = rInitialGuessParameter[1];

    //    for (int i = 0; i < MaxIterations; i++) {
    //        const auto s = pSurface->GlobalDerivatives(rPoint[0], rPoint[1], 2);

    //        const array_1d<double, 3> distance = s[0] - rPoint;

    //        const double c1v = norm_2(distance);

    //        if (c1v < Accuracy) {
    //            rNewLocalCoordinates[0] = rPoint[0];
    //            rNewLocalCoordinates[1] = rPoint[1];

    //            return true;
    //        }

    //        double s1_l = norm_2(s[1]);
    //        double s2_l = norm_2(s[2]);

    //        double c2an = std::abs(inner_prod(s[1], distance));
    //        double c2ad = s1_l * c1v;

    //        double c2bn = std::abs(inner_prod(s[2], distance));
    //        double c2bd = s2_l * c1v;

    //        double c2av = c2an / c2ad;
    //        double c2bv = c2bn / c2bd;

    //        if (c2av < Tolerance && c2bv < Tolerance) {
    //            rNewLocalCoordinates[0] = rU;
    //            rNewLocalCoordinates[1] = rV;

    //            return true;
    //        }

    //        double f = inner_prod(s[1], distance);
    //        double g = inner_prod(s[2], distance);

    //        double J_00 = inner_prod(s[1], s[1]) + inner_prod(s[3], distance);
    //        double J_01 = inner_prod(s[1], s[2]) + inner_prod(s[4], distance);
    //        double J_11 = inner_prod(s[2], s[2]) + inner_prod(s[5], distance);

    //        double det_J = J_00 * J_11 - J_01 * J_01;

    //        double d_u = (g * J_01 - f * J_11) / det_J;
    //        double d_v = (f * J_01 - g * J_00) / det_J;

    //        rU += d_u;
    //        rV += d_v;
    //    }

    //    rNewLocalCoordinates[0] = rU;
    //    rNewLocalCoordinates[1] = rV;
    //    return false;
    //}

} // namespace Kratos

#endif // KRATOS_PROJECTION_ON_NURBS_CURVE_UTILITIES_H_INCLUDED