#include "BouncebackNodes.hpp"
#include <iostream>
#include <vector>
#include "BoundaryNodes.hpp"
#include "CollisionModel.hpp"
#include "Node.hpp"

BouncebackNodes::BouncebackNodes(LatticeModel &lm
  , CollisionModel *cm)
  : BoundaryNodes(true, false, lm),
    nodes {},
    cm_ {cm}
{}

BouncebackNodes::BouncebackNodes(LatticeModel &lm
  , StreamModel *sm)
  : BoundaryNodes(true, true, lm),
    nodes {},
    sm_ {sm}
{}

void BouncebackNodes::AddNode(std::size_t x
  , std::size_t y)
{
  auto nx = lm_.GetNumberOfColumns();
  auto n = y * nx + x;
  nodes.push_back(Node(x, y, nx));
  // in C++11 nullptr is implicitly cast to boolean false
  // http://stackoverflow.com/questions/11279715/nullptr-and-checking-if-a-
  // pointer-points-to-a-valid-object
  if (cm_) cm_->AddNodeToSkip(n);
}

void BouncebackNodes::UpdateNodes(std::vector<std::vector<double>> &df
  , bool is_modify_stream)
{
  if (is_modify_stream) {
    auto nx = lm_.GetNumberOfColumns();
    auto ny = lm_.GetNumberOfRows();
    for (auto &node : nodes) {
      auto n = node.n;
      if (node.neighbours.empty())
          node.neighbours = BouncebackNodes::FindNeighbourNode(node);
      auto left = n % nx == 0;
      auto right = n % nx == nx - 1;
      auto bottom = n / nx == 0;
      auto top = n / nx == ny - 1;
      if (node.neighbours[0]){
        if (bottom) df[n][N] = node.df_node[S];
        if (top) df[n][S] = node.df_node[N];
      }
      if (node.neighbours[1]) {
        if (left) df[n][E] = node.df_node[W];
        if (right) df[n][W] = node.df_node[E];
      }
      if (bottom || left) df[n][NE] = node.df_node[SW];
      if (bottom || right) df[n][NW] = node.df_node[SE];
      if (top || right) df[n][SW] = node.df_node[NE];
      if (top || left) df[n][SE] = node.df_node[NW];
    }  // node
  }
  else {
    if (cm_) {
      for (auto node : nodes) {
        auto n = node.n;
        auto temp_node = df[n];
        df[n][E] = temp_node[W];
        df[n][N] = temp_node[S];
        df[n][W] = temp_node[E];
        df[n][S] = temp_node[N];
        df[n][NE] = temp_node[SW];
        df[n][NW] = temp_node[SE];
        df[n][SW] = temp_node[NE];
        df[n][SE] = temp_node[NW];
      }  // node
    }
    if (sm_) {
      for (auto &node : nodes) node.df_node = df[node.n];
    }
  }
}

std::vector<bool> BouncebackNodes::FindNeighbourNode(Node &node)
{
  std::vector<bool> result = {false, false};
  auto x = node.x;
  auto y = node.y;
  for (auto other_node : nodes) {
    auto o_x = other_node.x;
    auto o_y = other_node.y;
    if (o_y == y && (o_x == x + 1 || o_x == x - 1)) result[0] = true;
    if (o_x == x && (o_y == y + 1 || o_y == y - 1)) result[1] = true;
    if (result[0] && result[1]) return result;
  }  // node
  return result;
}
