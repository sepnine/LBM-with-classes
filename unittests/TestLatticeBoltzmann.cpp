#include <iostream>
#include <stdexcept>  // runtime_error
#include "CollisionCD.hpp"
#include "CollisionNS.hpp"
#include "LatticeBoltzmann.hpp"
#include "LatticeD2Q9.hpp"
#include "LatticeModel.hpp"
#include "UnitTest++.h"

SUITE(TestException)
{
TEST(ConstructorException)
{
//  CHECK_THROW(LatticeBoltzmann lbm, std::runtime_error);
}
}

SUITE(TestFunctionality)
{
  enum Directions {
    E = 1,
    N,
    W,
    S,
    NE,
    NW,
    SW,
    SE
  };
  static const double zero_tol = 1e-20;
  static const double loose_tol = 1e-5;
  static const std::size_t g_ny = 6;
  static const std::size_t g_nx = 7;
  static const double g_dx = 0.0316;
  static const double g_dt = 0.001;
  static const double g_t_total = 1.0;
  static const double g_d_coeff = 0.2;
  static const double g_k_visco = 0.2;
  static const double g_rho0_f = 1.1;
  static const double g_rho0_g = 1.2;
  static const std::vector<double> g_u0{1.3, 1.4};
  static const std::vector<std::vector<std::size_t>> g_src_pos_f{{1, 1},
      {2, 3}};
  static const std::vector<std::vector<double>> g_src_str_f{{1.5, 1.6},
      {1.7, 1.8}};
  static const std::vector<std::vector<std::size_t>> g_src_pos_g{{2, 2},
      {3, 4}};
  static const std::vector<double> g_src_str_g{1.9, 2.0};
  static const std::vector<std::vector<std::size_t>> g_obs_pos;
  static const bool g_is_ns = true;
  static const bool g_is_cd = true;
  static const bool g_is_instant = true;
  static const bool g_no_obstacles = false;

TEST(Constructor)
{
  // formatting function calling like this because input parameters may still
  // change and it's easier to edit this way
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
//  cd.Collide();
//  cd.ApplyForce();
//  lbm.Print(cd.source);
//  lbm.Print(1, ns.source);
}

TEST(InitObstacles)
{
  std::vector<bool> obstacle(g_nx * g_ny, false);
  std::vector<std::vector<std::size_t>> obs_pos = {{1, 2}, {3, 4}};
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , !g_no_obstacles
    , lm
    , ns
    , cd);
  std::size_t index = 0;
  std::size_t counter = 0;
  for (auto obs : lbm.obstacles) {
    if (counter == obs_pos[index][1] * g_nx + obs_pos[index][0]) {
      CHECK_EQUAL(true, obs);
      if (index < obs_pos.size() - 1) ++index;
    }
    else {
      CHECK_EQUAL(false, obs);
    }
    ++counter;
  }  // obs
}

TEST(InitDensity)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  for (auto n = 0u; n < ns.rho.size(); ++n) {
    CHECK_CLOSE(g_rho0_f, ns.rho[n], zero_tol);
    CHECK_CLOSE(g_rho0_g, cd.rho[n], zero_tol);
  }  // n
}

TEST(InitVelocity)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  LatticeBoltzmann lbm2(g_t_total
    , g_obs_pos
    , !g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  for (auto n = 0u; n < ns.u.size(); ++n) {
    CHECK_CLOSE(g_u0[0], ns.u[n][0], zero_tol);
    CHECK_CLOSE(g_u0[1], ns.u[n][1], zero_tol);
    CHECK_CLOSE(g_u0[0], cd.u[n][0], zero_tol);
    CHECK_CLOSE(g_u0[1], cd.u[n][1], zero_tol);
  }  // n
}

TEST(ComputeEq)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  std::vector<double> expected_cd = {0.53041,
                                     0.15007, 0.15150, 0.11716, 0.11606,
                                     0.04279, 0.03347, 0.02570, 0.03284};
  std::vector<double> expected_ns = {0.48621,
                                     0.13757, 0.13888, 0.10740, 0.10639,
                                     0.03922, 0.03068, 0.02356, 0.03010};
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    for (auto i = 0; i < 9; ++i) {
      // can just check the member in cd/ns since it's using reference
      // instead of creating a new copy
      CHECK_CLOSE(expected_ns[i], ns.lattice_eq[n][i], loose_tol);
      CHECK_CLOSE(expected_cd[i], cd.lattice_eq[n][i], loose_tol);
    }  // i
  }  // n
}

TEST(InitDistributionFunctionLattice)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  std::vector<double> expected_cd = {0.53041,
                                     0.15007, 0.15150, 0.11716, 0.11606,
                                     0.04279, 0.03347, 0.02570, 0.03284};
  std::vector<double> expected_ns = {0.48621,
                                     0.13757, 0.13888, 0.10740, 0.10639,
                                     0.03922, 0.03068, 0.02356, 0.03010};
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    for (auto i = 0; i < 9; ++i) {
      CHECK_CLOSE(expected_ns[i], lbm.f[n][i], loose_tol);
      CHECK_CLOSE(expected_cd[i], lbm.g[n][i], loose_tol);
    }  // i
  }  // n
}

TEST(InitSourceMultiplePosition)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  std::size_t i_ns = 0;
  std::size_t i_cd = 0;
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    if (n == g_src_pos_f[i_ns][1] * g_nx + g_src_pos_f[i_ns][0]) {
      CHECK_CLOSE(g_src_str_f[i_ns][0], ns.source[n][0], zero_tol);
      CHECK_CLOSE(g_src_str_f[i_ns][1], ns.source[n][1], zero_tol);
      if (i_ns < g_src_pos_f.size() - 1) ++i_ns;
    }
    else {
      CHECK_CLOSE(0.0, ns.source[n][0], zero_tol);
      CHECK_CLOSE(0.0, ns.source[n][1], zero_tol);
    }
    if (n == g_src_pos_g[i_cd][1] * g_nx + g_src_pos_g[i_cd][0]) {
      CHECK_CLOSE(g_src_str_g[i_cd], cd.source[n], zero_tol);
      if (i_cd < g_src_pos_g.size() - 1) ++i_cd;
    }
    else {
      CHECK_CLOSE(0.0, cd.source[n], zero_tol);
    }
  }  // n
}

TEST(CollideWithMixedSource)
{
  // uses global source lattice, get different values at different nodes due to
  // presence of source at that node, nodes without source covers case for
  // CollideNoSource as well
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  // checks collision result for both cd and ns
  // in expected, index 0 for first source strength, 1 for second source
  // strength (since global pos has 2 positions), 2 for no source (so that it
  // matches source index)
  std::vector<std::vector<double>> expected_cd1 =
      {{0.57428,
        0.22817, 0.22947, 0.19825, 0.19724,
        0.13055, 0.12208, 0.11502, 0.12150},
       {0.57432,
        0.22818, 0.22948, 0.19826, 0.19725,
        0.13056, 0.12208, 0.11502, 0.12151},
       {0.57343,
        0.22795, 0.22924, 0.19805, 0.19705,
        0.13049, 0.12203, 0.11497, 0.12145}};
  std::vector<std::vector<double>> expected_ns1 =
      {{0.53328,
        0.21660, 0.21779, 0.18917, 0.18825,
        0.12726, 0.11949, 0.11302, 0.11896},
       {0.53328,
        0.21660, 0.21779, 0.18917, 0.18825,
        0.12726, 0.11949, 0.11302, 0.11896},
       {0.53328,
        0.21659, 0.21778, 0.18918, 0.18826,
        0.12725, 0.11949, 0.11302, 0.11897}};
  std::vector<std::vector<double>> expected_cd2 =
      {{0.53527,
        0.15745, 0.15887, 0.12479, 0.12369,
        0.05089, 0.04164, 0.03393, 0.04101},
       {0.53532,
        0.15747, 0.15888, 0.12480, 0.12370,
        0.05089, 0.04164, 0.03393, 0.04102},
       {0.53435,
        0.15721, 0.15862, 0.12457, 0.12348,
        0.05083, 0.04158, 0.03388, 0.04095}};
  std::vector<std::vector<double>> expected_ns2 =
      {{0.49052,
        0.14482, 0.14612, 0.11488, 0.11388,
        0.04730, 0.03882, 0.03175, 0.03824},
       {0.49052,
        0.14482, 0.14612, 0.11488, 0.11388,
        0.04730, 0.03882, 0.03175, 0.03824},
       {0.49052,
        0.14481, 0.14611, 0.11489, 0.11389,
        0.04729, 0.03882, 0.03176, 0.03824}};
  // Set distribution function to have different value from equilibrium
  // distribution function so Collide can produce changed result
  lbm.g.assign(g_nx * g_ny, std::vector<double>(9, 1.0));
  lbm.f.assign(g_nx * g_ny, std::vector<double>(9, 1.0));
  // First collision
  cd.Collide(lbm.g);
  cd.ApplyForce(lbm.g);
  ns.Collide(lbm.f);
  ns.ApplyForce(lbm.f);

  for (auto i = 0; i < 9; ++i) {
    std::size_t i_ns = 0;
    std::size_t i_cd = 0;
    for (auto n = 0u; n < g_nx * g_ny; ++n) {
      if (n == g_src_pos_f[i_ns][1] * g_nx + g_src_pos_f[i_ns][0]) {
        CHECK_CLOSE(expected_ns1[i_ns][i], lbm.f[n][i], loose_tol);
        if (i_ns < g_src_pos_f.size() - 1) ++i_ns;
      }
      else {
        CHECK_CLOSE(expected_ns1[2][i], lbm.f[n][i], loose_tol);
      }
      if (n == g_src_pos_g[i_cd][1] * g_nx + g_src_pos_g[i_cd][0]) {
        CHECK_CLOSE(expected_cd1[i_cd][i], lbm.g[n][i], loose_tol);
        if (i_cd < g_src_pos_g.size() - 1) ++i_cd;
      }
      else {
        CHECK_CLOSE(expected_cd1[2][i], lbm.g[n][i], loose_tol);
      }
    }  // n
  }  // i
  // Second collision
  cd.Collide(lbm.g);
  cd.ApplyForce(lbm.g);
  ns.Collide(lbm.f);
  ns.ApplyForce(lbm.f);
  for (auto i = 0; i < 9; ++i) {
    std::size_t i_ns = 0;
    std::size_t i_cd = 0;
    for (auto n = 0u; n < g_nx * g_ny; ++n) {
      if (n == g_src_pos_f[i_ns][1] * g_nx + g_src_pos_f[i_ns][0]) {
        CHECK_CLOSE(expected_ns2[i_ns][i], lbm.f[n][i], loose_tol);
        if (i_ns < g_src_pos_f.size() - 1) ++i_ns;
      }
      else {
        CHECK_CLOSE(expected_ns2[2][i], lbm.f[n][i], loose_tol);
      }
      if (n == g_src_pos_g[i_cd][1] * g_nx + g_src_pos_g[i_cd][0]) {
        CHECK_CLOSE(expected_cd2[i_cd][i], lbm.g[n][i], loose_tol);
        if (i_cd < g_src_pos_g.size() - 1) ++i_cd;
      }
      else {
        CHECK_CLOSE(expected_cd2[2][i], lbm.g[n][i], loose_tol);
      }
    }  // n
  }  // i
}

TEST(ComputeU)
{
  // uses global source lattice, get different values at different nodes due to
  // presence of source at that node, nodes without source covers case for
  // ComputeUNoSource as well
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  lbm.f.assign(g_nx * g_ny, {0, 1, 2, 3, 4, 5, 6, 7, 8});
  ns.u = lm.ComputeU(lbm.f, ns.rho, ns.source);
  std::size_t i_ns = 0;
  std::vector<std::vector<double>> expected{{-57.45380, -172.36284},
      {-57.45370, -172.36274}, {-57.45455, -172.36364}};
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    if (n == g_src_pos_f[i_ns][1] * g_nx + g_src_pos_f[i_ns][0]) {
      CHECK_CLOSE(expected[i_ns][0], ns.u[n][0], loose_tol);
      CHECK_CLOSE(expected[i_ns][1], ns.u[n][1], loose_tol);
      if (i_ns < g_src_pos_f.size() - 1) ++i_ns;
    }
    else {
      CHECK_CLOSE(expected[2][0], ns.u[n][0], loose_tol);
      CHECK_CLOSE(expected[2][1], ns.u[n][1], loose_tol);
    }
  }  // n
}

TEST(BoundaryPeriodic)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  lbm.f.assign(g_nx * g_ny, {0, 1, 2, 3, 4, 5, 6, 7, 8});
  lbm.boundary_f = lbm.BoundaryCondition(lbm.f);
  for (auto y = 0u; y < g_ny; ++y) {
    auto n = y * (g_nx);
    CHECK_CLOSE(lbm.f[n + g_nx - 1][E], lbm.boundary_f[y][E], zero_tol);
    CHECK_CLOSE(lbm.f[n + g_nx - 1][NE], lbm.boundary_f[y][NE],
        zero_tol);
    CHECK_CLOSE(lbm.f[n + g_nx - 1][SE], lbm.boundary_f[y][SE],
        zero_tol);
    CHECK_CLOSE(lbm.f[n][W], lbm.boundary_f[y + g_ny][W], zero_tol);
    CHECK_CLOSE(lbm.f[n][NW], lbm.boundary_f[y + g_ny][NW], zero_tol);
    CHECK_CLOSE(lbm.f[n][SW], lbm.boundary_f[y + g_ny][SW], zero_tol);
  }  // y
}

TEST(BoundaryBounceback)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  double value = 0;
  for (auto &node : lbm.f) node = std::vector<double>(9, value++);
  lbm.boundary_f = lbm.BoundaryCondition(lbm.f);
  auto top = 2 * g_ny;
  auto bottom = 2 * g_ny + g_nx;
  for (auto x = 0u; x < g_nx; ++x) {
    auto n = (g_ny - 1) * g_nx;
    CHECK_CLOSE(lbm.f[x + n][N], lbm.boundary_f[top + x][S], zero_tol);
    CHECK_CLOSE(lbm.f[x][S], lbm.boundary_f[bottom + x][N], zero_tol);
    if (x == 0) {
      CHECK_CLOSE(lbm.f[n + g_nx - 1][NE], lbm.boundary_f[top + x][SW],
          zero_tol);
      CHECK_CLOSE(lbm.f[g_nx - 1][SE], lbm.boundary_f[bottom + x][NW],
          zero_tol);
    }
    else {
      CHECK_CLOSE(lbm.f[x + n - 1][NE], lbm.boundary_f[top + x][SW], zero_tol);
      CHECK_CLOSE(lbm.f[x - 1][SE], lbm.boundary_f[bottom + x][NW], zero_tol);
    }
    if (x == g_nx - 1) {
      CHECK_CLOSE(lbm.f[n][NW], lbm.boundary_f[top + x][SE], zero_tol);
      CHECK_CLOSE(lbm.f[0][SW], lbm.boundary_f[bottom + x][NE], zero_tol);
    }
    else {
      CHECK_CLOSE(lbm.f[x + n + 1][NW], lbm.boundary_f[top + x][SE], zero_tol);
      CHECK_CLOSE(lbm.f[x + 1][SW], lbm.boundary_f[bottom + x][NE], zero_tol);
    }
  }  // x
}

TEST(BoundaryCorner)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  lbm.f.assign(g_nx * g_ny, {0, 1, 2, 3, 4, 5, 6, 7, 8});
  lbm.boundary_f = lbm.BoundaryCondition(lbm.f);
  auto corner = 2 * g_nx + 2 * g_ny;
  CHECK_CLOSE(lbm.f[0][SW], lbm.boundary_f[corner][NE], zero_tol);
  CHECK_CLOSE(lbm.f[g_nx - 1][SE], lbm.boundary_f[corner + 1][NW], zero_tol);
  CHECK_CLOSE(lbm.f[(g_ny - 1) * g_nx][NW], lbm.boundary_f[corner + 2][SE],
      zero_tol);
  CHECK_CLOSE(lbm.f[g_ny * g_nx - 1][NE], lbm.boundary_f[corner + 3][SW],
      zero_tol);
}

TEST(StreamHorizontal)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  std::vector<double> first_three = {1, 2, 1, 0, 1, 2, 0, 0, 2};
  std::vector<double> second_three = {4, 5, 4, 3, 4, 5, 3, 3, 5};
  std::vector<std::vector<double>> nodes = {first_three, second_three};
  std::vector<double> first_result = {1, 5, 1, 3, 1, 5, 3, 3, 5};
  std::vector<double> second_result = {4, 2, 4, 0, 4, 2, 0, 0, 2};
  int counter = 0;
  for (auto &node : lbm.f) {
    node = nodes[(counter / g_nx + counter) % 2];
    ++counter;
  }  // node
  lbm.boundary_f = lbm.BoundaryCondition(lbm.f);
  // assign boundary values in opposition direction as appending them as
  // conditional is "less than" so it will overwrite previous conditional
  // if satisfied
  for (auto n = 0u; n < lbm.boundary_f.size(); ++n) {
    // corner boundary
    lbm.boundary_f[n] = nodes[1];
    // bottom boundary
    if (n < 2 * g_ny + 2 * g_nx) lbm.boundary_f[n] = nodes[(n + 1) % 2];
    // top boundary
    if (n < 2 * g_ny + g_nx) lbm.boundary_f[n] = nodes[n % 2];
    // right boundary
    if (n < 2 * g_ny) lbm.boundary_f[n] = nodes[(g_ny + 1) % 2];
    // left boundary
    if (n < g_ny) lbm.boundary_f[n] = nodes[1];
  }  // n
  lbm.f = lbm.Stream(lbm.f, lbm.boundary_f);
  for (auto node : lbm.f) {
    for (auto i = 0u; i < 9; ++i) {
      if ((node[0] - 1) < zero_tol) {
      CHECK_CLOSE(first_result[i], node[i], zero_tol);
      }
      else {
        CHECK_CLOSE(second_result[i], node[i], zero_tol);
      }
    }  // i
  }  // lat
}

TEST(StreamVertical)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  std::vector<double> first_three = {1, 1, 0, 1, 2, 0, 0, 2, 2};
  std::vector<double> second_three = {4, 4, 3, 4, 5, 3, 3, 5, 5};
  std::vector<std::vector<double>> nodes = {first_three, second_three};
  std::vector<double> first_result = {1, 1, 3, 1, 5, 3, 3, 5, 5};
  std::vector<double> second_result = {4, 4, 0, 4, 2, 0, 0, 2, 2};
  int counter = 0;
  for (auto &node : lbm.f) node = nodes[(counter++ / g_nx) % 2];
  lbm.boundary_f = lbm.BoundaryCondition(lbm.f);
  for (auto n = 0u; n < lbm.boundary_f.size(); ++n) {
    // corner boundary
    lbm.boundary_f[n] = nodes[0];
    if (n < 2 * g_ny + 2 * g_nx + 2) lbm.boundary_f[n] = nodes[1];
    // bottom boundary
    if (n < 2 * g_ny + 2 * g_nx) lbm.boundary_f[n] = nodes[1];
    // top boundary
    if (n < 2 * g_ny + g_nx) lbm.boundary_f[n] = nodes[0];
    // right boundary
    if (n < 2 * g_ny) lbm.boundary_f[n] = nodes[n % 2];
    // left boundary
    if (n < g_ny) lbm.boundary_f[n] = nodes[n % 2];
  }  // n
  lbm.f = lbm.Stream(lbm.f, lbm.boundary_f);
  for (auto node : lbm.f) {
    for (auto i = 0u; i < 9; ++i) {
      if ((node[0] - 1) < zero_tol) {
      CHECK_CLOSE(first_result[i], node[i], zero_tol);
      }
      else {
        CHECK_CLOSE(second_result[i], node[i], zero_tol);
      }
    }  // i
  }  // lat
}

TEST(StreamDiagonalNESW)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  std::vector<double> ones(9, 1);
  std::vector<double> twos(9, 2);
  std::vector<double> threes(9, 3);
  std::vector<std::vector<double>> nodes = {ones, twos, threes};
  std::vector<double> result = {1, 2, 3};
  for (auto y = 0u; y < g_ny; ++y) {
    for (auto x = 0u; x < g_nx; ++x) {
      auto n = y * g_nx + x;
      lbm.f[n] = nodes[(x % 3 + (y + 2) % 3) % 3];
    }  // x
  }  // y
  lbm.boundary_f = lbm.BoundaryCondition(lbm.f);
  for (auto n = 0u; n < lbm.boundary_f.size(); ++n) {
    // corner boundary
    lbm.boundary_f[n] = nodes[(2 - n % 3) % 3];
    // bottom boundary
    if (n < 2 * g_ny + 2 * g_nx) lbm.boundary_f[n] = nodes[n % 3];
    // top boundary
    if (n < 2 * g_ny + g_nx) lbm.boundary_f[n] = nodes[(n - 1) % 3];
    // right boundary
    if (n < 2 * g_ny) lbm.boundary_f[n] = nodes[n % 3];
    // left boundary
    if (n < g_ny) lbm.boundary_f[n] = nodes[(n + 1) % 3];
  }  // n
  lbm.f = lbm.Stream(lbm.f, lbm.boundary_f);
  for (auto y = 0u; y < g_ny; ++y) {
    for (auto x = 0u; x < g_nx; ++x) {
      auto n = y * g_nx + x;
      CHECK_CLOSE(result[(x % 3 + y % 3) % 3], lbm.f[n][NE], zero_tol);
      CHECK_CLOSE(result[(x % 3 + y % 3 + 1) % 3], lbm.f[n][SW], zero_tol);
    }  // x
  }  // y
}

TEST(StreamDiagonalNWSE)
{
  LatticeD2Q9 lm(g_ny
    , g_nx
    , g_dx
    , g_dt);
  CollisionNS ns(lm
    , g_src_pos_f
    , g_src_str_f
    , g_k_visco
    , g_rho0_f
    , g_u0);
  CollisionCD cd(lm
    , g_src_pos_g
    , g_src_str_g
    , g_d_coeff
    , g_rho0_g
    , g_u0);
  LatticeBoltzmann lbm(g_t_total
    , g_obs_pos
    , g_is_ns
    , g_is_cd
    , g_is_instant
    , g_no_obstacles
    , lm
    , ns
    , cd);
  std::vector<double> ones(9, 1);
  std::vector<double> twos(9, 2);
  std::vector<double> threes(9, 3);
  std::vector<std::vector<double>> nodes = {ones, twos, threes};
  std::vector<double> result = {1, 2, 3};
  for (auto y = 0u; y < g_ny; ++y) {
    for (auto x = 0u; x < g_nx; ++x) {
      auto n = y * g_nx + x;
      lbm.f[n] = nodes[(2 - x % 3 + (y + 2) % 3) % 3];
    }  // x
  }  // y
  lbm.boundary_f = lbm.BoundaryCondition(lbm.f);
  for (auto n = 0u; n < lbm.boundary_f.size(); ++n) {
    // corner boundary
    lbm.boundary_f[n] = nodes[(n + 1) % 3];
    if (n < 2 * g_ny + 2 * g_nx + 2) lbm.boundary_f[n] = nodes[(n - 1) % 3];
    // bottom boundary
    if (n < 2 * g_ny + 2 * g_nx) lbm.boundary_f[n] = nodes[(4 - n % 3) % 3];
    // top boundary
    if (n < 2 * g_ny + g_nx) lbm.boundary_f[n] = nodes[(4 - n % 3) % 3];
    // right boundary
    if (n < 2 * g_ny) lbm.boundary_f[n] = nodes[n % 3];
    // left boundary
    if (n < g_ny) lbm.boundary_f[n] = nodes[(n + 2) % 3];
  }  // n
  lbm.f = lbm.Stream(lbm.f, lbm.boundary_f);
  for (auto y = 0u; y < g_ny; ++y) {
    for (auto x = 0u; x < g_nx; ++x) {
      auto n = y * g_nx + x;
      CHECK_CLOSE(result[(2 - x % 3 + y % 3) % 3], lbm.f[n][NW], zero_tol);
      CHECK_CLOSE(result[(3 - x % 3 + y % 3) % 3], lbm.f[n][SE], zero_tol);
    }  // x
  }  // y
}
}
