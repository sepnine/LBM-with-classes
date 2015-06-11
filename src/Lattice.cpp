#include "Lattice.hpp"
#include <iomanip> // std::setprecision
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

Lattice::Lattice()
  : number_of_rows_ {0},
    number_of_columns_ {0}
{}

Lattice::Lattice(int which_type
  , std::size_t num_rows
  , std::size_t num_cols)
  : lattice_type_ {which_type},
    number_of_rows_ {num_rows},
    number_of_columns_ {num_cols}
{
  // 0 for distribution functions, 1 for velocities,
  switch (which_type) {
    case 0: {
      values_.assign((num_rows + 2) * (num_cols + 2),
          std::vector<double>(number_of_discrete_velocities_));
      break;
    }
    case 1: {
      values_.assign((num_rows + 2) * (num_cols + 2),
          std::vector<double>(number_of_dimensions_));
      break;
    }
    default: {
      throw std::runtime_error("Incorrect lattice type");
    }
  }
}

Lattice::Lattice(int which_type
  , const double initial_density
  , std::size_t num_rows
  , std::size_t num_cols)
  : lattice_type_ {which_type},
    number_of_rows_ {num_rows},
    number_of_columns_ {num_cols}
{
  // 0 for distribution functions, 1 for velocities,
  switch (which_type) {
    case 0: {
      values_.assign((num_rows + 2) * (num_cols + 2),
          std::vector<double>(number_of_discrete_velocities_));
      rho_.assign((num_rows + 2) * (num_cols + 2), initial_density);
      break;
    }
    case 1: {
      values_.assign((num_rows + 2) * (num_cols + 2),
          std::vector<double>(number_of_dimensions_));
      break;
    }
    default: {
      throw std::runtime_error("Incorrect lattice type");
    }
  }
}

std::size_t Lattice::GetNumberOfDimensions() const
{
  return number_of_dimensions_;
}

std::size_t Lattice::GetNumberOfDiscreteVelocities() const
{
  return number_of_discrete_velocities_;
}

std::size_t Lattice::GetNumberOfRows() const
{
  return number_of_rows_;
}

std::size_t Lattice::GetNumberOfColumns() const
{
  return number_of_columns_;
}

void Lattice::Init(const std::vector<double> &initial_values)
{
  switch (lattice_type_) {
   case 0: {
    if (initial_values.size() != number_of_discrete_velocities_) {
      throw std::runtime_error("Dimension mismatch");
    }
    break;
   }
   case 1: {
    if (initial_values.size() != number_of_dimensions_) {
      throw std::runtime_error("Dimension mismatch");
    }
    break;
   }
   default:
    break;
  }
  auto nx = GetNumberOfColumns();
  auto ny = GetNumberOfRows();
  for (auto y = nx + 2; y < (nx + 2) * (ny + 1);y += nx + 2) {
    for (auto x = 1u; x < nx + 1; ++x) {
      values_[y + x] = initial_values;
    }  // x
  }  // y
}

void Lattice::ComputeEq(const Lattice &u
  , const double c)
{
  if (lattice_type_ != 0) {
    throw std::runtime_error("Not depth 9 lattice.");
  }
  else {
    auto nx = GetNumberOfColumns();
    auto ny = GetNumberOfRows();
    auto nc = GetNumberOfDiscreteVelocities();
    double cs_sqr = c * c / 3.;
    for (auto y = nx + 2; y < (nx + 2) * (ny + 1); y += nx + 2) {
      for (auto x = 1; x < nx + 1; ++x) {
        double u_sqr = u.values_[y + x][0] * u.values_[y + x][0] +
            u.values_[y + x][1] + u.values_[y + x][1];
        u_sqr /= 2. * cs_sqr;
        for (auto i = 0u; i < nc; ++i) {
          double c_dot_u = u.values_[y + x][0] * e_[i][0] +
              u.values_[y + x][1] * e_[i][1];
          c_dot_u /= cs_sqr / c;
          values_[y + x][i] = omega_[i] * rho_[y + x] *
              (1. + c_dot_u * (1. + c_dot_u / 2.) - u_sqr);
        }  // i
      }  // x
    }  // y
  }
}

void Lattice::Print1D(const int which_to_print)
{
  auto nx = GetNumberOfColumns();
  auto ny = GetNumberOfRows();
  switch (which_to_print) {
    case 0: {
      for (auto y = 0u; y < (nx + 2) * (ny + 2); y += nx + 2) {
        // column of lattice
        for (auto x = 0u; x < nx + 2; ++x) {
          std::cout << rho_[y + x] << " ";
        } // x
        std::cout << "\n";
      } // y
    }
  }
}

void Lattice::Print2D()
{
  auto nx = GetNumberOfColumns();
  auto ny = GetNumberOfRows();
  switch (lattice_type_) {
    case 0: {
      auto nc = GetNumberOfDiscreteVelocities();
      // row of lattice
      for (auto y = 0u; y < (nx + 2) * (ny + 2); y += nx + 2) {
        // rows in the Q9 square
        for (auto i = 0u; i < nc / 3; ++i) {
          // column of lattice
          for (auto x = 0u; x < nx + 2; ++x) {
            if (i == 0){
            std::cout << std::fixed << std::setprecision(2)
                      << values_[y + x][6] << " "
                      << values_[y + x][2] << " "
                      << values_[y + x][5] << " ";
            }
            else if (i == 1){
            std::cout << std::fixed << std::setprecision(2)
                      << values_[y + x][3] << " "
                      << values_[y + x][0] << " "
                      << values_[y + x][1] << " ";
            }
            else if (i == 2){
            std::cout << std::fixed << std::setprecision(2)
                      << values_[y + x][7] << " "
                      << values_[y + x][4] << " "
                      << values_[y + x][8] << " ";
            }
            std::cout << "  ";
          } // x
          std::cout << "\n";
        } // i
        std::cout << "\n";
      } // y
      break;
    }
    case 1: {
      auto nd = GetNumberOfDimensions();
      // row of lattice
      for (auto y = 0u; y < (nx + 2) * (ny + 2); y += nx + 2) {
        // column of lattice
        for (auto x = 0u; x < nx + 2; ++x) {
          // elements of one line of Q9 square
          for(auto d = 0u; d < nd; ++d) {
            std::cout << values_[y + x][d] << " ";
          } // d
          std::cout << "  ";
        } // x
        std::cout << "\n";
      } // y
      break;
    }
    default: {
      throw std::runtime_error("Not a 2D lattice");
      break;
    }
  }
}
