#include "Collision.hpp"
#include <iostream>
#include <stdexcept>
#include "LatticeModel.hpp"

Collision::Collision(LatticeModel &lm
  , double initial_density)
  : lattice_eq {},
    rho {},
    lm_ (lm),
    tau_ {0},
    c_ {lm.GetLatticeSpeed()}
{
  auto nx = lm_.GetNumberOfColumns();
  auto ny = lm_.GetNumberOfRows();
  auto nc = lm_.GetNumberOfDirections();
  auto lat_size = nx * ny;
  rho.assign(lat_size, initial_density);
  lattice_eq.assign(lat_size, std::vector<double>(nc, 0.0));
  ComputeEq();
}

void Collision::ComputeEq()
{
  auto nc = lm_.GetNumberOfDirections();
  auto nx = lm_.GetNumberOfColumns();
  auto ny = lm_.GetNumberOfRows();
  for (auto n = 0u; n < nx * ny; ++n) {
    double u_sqr = Collision::InnerProduct(lm_.u[n], lm_.u[n]);
    u_sqr /= 2.0 * cs_sqr_;
    for (auto i = 0u; i < nc; ++i) {
      double c_dot_u = Collision::InnerProduct(lm_.e[i], lm_.u[n]);
      c_dot_u /= cs_sqr_ / c_;
      lattice_eq[n][i] = lm_.omega[i] * rho[n] * (1.0 + c_dot_u *
          (1.0 + c_dot_u / 2.0) - u_sqr);
    }  // i
  }  // n
}

void Collision::Collide(std::vector<std::vector<double>> &lattice)
{
  auto nc = lm_.GetNumberOfDirections();
  auto nx = lm_.GetNumberOfColumns();
  auto ny = lm_.GetNumberOfRows();
  for (auto n = 0u; n < nx * ny; ++n) {
    for (auto i = 0u; i < nc; ++i) {
      lattice[n][i] += (lattice_eq[n][i] - lattice[n][i]) / tau_;
    }  // i
  }  // n
}

double Collision::InnerProduct(const std::vector<double> &a
  , const std::vector<double> &b)
{
  if (a.size() != b.size()) throw std::runtime_error("Size mismatch");
  double result = 0.0;
  auto it_b = begin(b);
  for (auto element_a : a) result += element_a * (*it_b++);
  return result;
}
