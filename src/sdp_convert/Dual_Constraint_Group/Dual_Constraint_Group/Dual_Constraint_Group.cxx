//=======================================================================
// Copyright 2014-2015 David Simmons-Duffin.
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include "../../Dual_Constraint_Group.hxx"

#include "../../../sdp2input/Boost_Float.hxx"
#include <boost/math/special_functions.hpp>

El::Matrix<El::BigFloat>
sample_bilinear_basis(const int max_degree, const int num_samples,
                      const std::vector<Boost_Float> &sample_points,
                      const std::vector<Boost_Float> &sample_scalings);

// Construct a Dual_Constraint_Group from a Polynomial_Vector_Matrix by
// sampling the matrix at the appropriate number of points, as
// described in SDP.h:
//
//   (1,y) . M(x) is positive semidefinite
//
// is equivalent to
//
//   Tr(A_p Y) + (B y)_p = c_p
//
// for tuples p = (r,s,k).
//
Dual_Constraint_Group::Dual_Constraint_Group(
  const Polynomial_Vector_Matrix &pvm)
    : dim(pvm.rows), degree(pvm.degree())
{
  assert(pvm.rows == pvm.cols);

  size_t num_samples = degree + 1;
  size_t numConstraints = num_samples * dim * (dim + 1) / 2;
  size_t vectorDim = pvm.element(0, 0).size();

  const size_t num_points(pvm.sample_points.size());
  Boost_Float::default_precision(El::gmp::Precision() * log(2) / log(10));

  const double x_scale(pvm.sample_points.back());

  std::vector<Boost_Float> cheb_points(num_points);
  for(size_t point = 0; point < cheb_points.size(); ++point)
    {
      cheb_points[point] = boost::math::cos_pi(
        Boost_Float(2 * (num_points - point) - 1) / (2 * num_points));
    }

  std::vector<Boost_Float> points(num_points);
  for(size_t point = 0; point < points.size(); ++point)
    {
      points[point] = x_scale * (Boost_Float(1) + cheb_points[point]);
    }

  // Form the constraint_matrix B and constraint_constants c from the
  // polynomials (1,y) . \vec P^{rs}(x)

  // The first element of each vector \vec P^{rs}(x) multiplies the constant 1
  constraint_constants.resize(numConstraints);
  // The rest multiply decision variables y
  constraint_matrix.Resize(numConstraints, vectorDim - 1);

  // Populate B and c by sampling the polynomial matrix

  int p = 0;
  for(size_t c = 0; c < dim; c++)
    {
      for(size_t r = 0; r <= c; r++)
        {
          for(size_t k = 0; k < num_samples; k++)
            {
              El::BigFloat x(to_string(points[k]));
              constraint_constants[p] = pvm.element(r, c)[0](x);
              for(size_t n = 1; n < vectorDim; ++n)
                {
                  constraint_matrix.Set(p, n - 1, -pvm.element(r, c)[n](x));
                }
              ++p;
            }
        }
    }

  // The matrix Y has two blocks Y_1, Y_2.  The bilinear_bases for the
  // constraint matrices A_p are given by sampling the following
  // vectors for each block:
  //
  //   Y_1: {q_0(x), ..., q_delta1(x)}
  //   Y_2: {\sqrt(x) q_0(x), ..., \sqrt(x) q_delta2(x)
  //
  const size_t delta1(degree / 2);

  bilinear_bases[0] = sample_bilinear_basis(
    delta1, num_samples, cheb_points, std::vector<Boost_Float>(num_points, 1));

  const size_t delta2(degree == 0 ? 0 : (degree - 1) / 2);
  // a degree-0 Polynomial_Vector_Matrix should only need one block,
  // but this duplicates the block.

  // Scale the second second of bases by the coordinate 'x'
  bilinear_bases[1]
    = sample_bilinear_basis(delta2, num_samples, cheb_points, points);
}
