#include "../../../../Positive_Matrix_With_Prefactor_State.hxx"

#include <algorithm>
#include <iterator>
#include <string>

std::vector<char>::const_iterator
parse_damped_rational(const std::vector<char>::const_iterator &begin,
                      const std::vector<char>::const_iterator &end,
                      Damped_Rational &damped_rational);

std::vector<char>::const_iterator
parse_matrix(const std::vector<char>::const_iterator &begin,
             const std::vector<char>::const_iterator &end,
             Positive_Matrix_With_Prefactor &matrix)
{
  const std::string matrix_literal("PositiveMatrixWithPrefactor[");
  auto matrix_start(
    std::search(begin, end, matrix_literal.begin(), matrix_literal.end()));
  if(matrix_start == end)
    {
      throw std::runtime_error("Could not find '" + matrix_literal + "'");
    }

  auto start_damped_rational(std::next(matrix_start, matrix_literal.size()));
  auto end_damped_rational(
    parse_damped_rational(start_damped_rational, end, matrix.damped_rational));

  auto comma(std::find(end_damped_rational, end, ','));
  if(comma == end)
    {
      throw std::runtime_error("Missing comma after DampedRational");
    }

  // std::cout << "damped: " << matrix.damped_rational.constant << "\n\t"
  //           << matrix.damped_rational.base << "\n";
  // for(auto &p: matrix.damped_rational.poles)
  //   std::cout << " " << p;
  // std::cout << "\n";
  
  // auto end_polynomials(
  //   parse_polynomials(std::next(comma, 1), end, matrix.polynomials));

  // const auto close_bracket(std::find(end_polynomials, end, ']'));
  // if(close_bracket == end)
  //   {
  //     throw std::runtime_error("Missing ']' at end of SDP");
  //   }
  // return std::next(close_bracket, 1);

  return end;
}
