// This file is part of the Colored Abstract Simplicial Complex library.
// Copyright (C) 2016-2021
// by Christopher T. Lee, John Moody, Rommie Amaro, J. Andrew McCammon,
//    and Michael Holst
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, see <http://www.gnu.org/licenses/>
// or write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
// Boston, MA 02111-1307 USA

#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <set>

#include "gtest/gtest.h"

#include <casc/casc>

using SurfaceMeshType = casc::AbstractSimplicialComplex<int, // KEYTYPE
                                                        int, // Root data
                                                        int, // Vertex data
                                                        int, // Edge data
                                                        int  // Face data
                                                        >;

class DecimationTest : public testing::Test {
protected:
  DecimationTest() {}
  ~DecimationTest() {}
  virtual void SetUp() {
    mesh.insert({0, 1, 3});
    mesh.insert({0, 3, 5});
    mesh.insert({1, 3, 4});
    mesh.insert({3, 4, 5});
    mesh.insert({1, 2, 4});
    mesh.insert({2, 4, 5});
  }
  virtual void TearDown() {}

  SurfaceMeshType mesh;
  using SimplexSet = typename casc::SimplexSet<SurfaceMeshType>;
};

// template <typename T, std::size_t k>
// std::ostream& operator<<(std::ostream& out, const std::array<T,k>& A)
// {
//     out << "[";
//     for(int i = 0; i + 1 < k; ++i)
//     {
//         out << A[i] << " ";
//     }
//     if(k > 0)
//     {
//         out << A[k-1];
//     }
//     out << "]";
//     return out;
// }

// template <typename T>
// std::ostream& operator<<(std::ostream& out, const std::set<T>& A)
// {
//     out << "[";
//     for(auto a : A)
//     {
//         std::cout << a << " ";
//     }
//     out << "]";
//     return out;
// }

template <typename Complex> struct Callback {
  using SimplexSet = typename casc::SimplexSet<Complex>;
  using KeyType = typename Complex::KeyType;

  template <std::size_t k>
  int operator()(Complex &, const std::array<KeyType, k> &,
                 const SimplexSet &) {
    // std::cout << merged << " -> " << new_name << std::endl;
    return 0;
  }
};

TEST_F(DecimationTest, Decimation) {
  auto s = mesh.get_simplex_up({3, 4});
  casc::decimate(mesh, s, Callback<SurfaceMeshType>());
}

TEST_F(DecimationTest, BoundaryCheck) {
  mesh.insert({1, 2, 6});
  mesh.insert({0, 1, 6});
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1})), false);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3})), false);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({4})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({4})), false);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 1})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 1})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 3})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 3})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 3})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 3})), false);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3, 5})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 4})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 4})), false);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3, 4})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3, 4})), false);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({4, 5})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({4, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 2})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 2})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 4})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 4})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 6})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 1, 3})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 1, 3})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 3, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 3, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 3, 4})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 3, 4})), false);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3, 4, 5})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3, 4, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 2, 4})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 2, 4})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 4, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 4, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 2, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 2, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 1, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 1, 6})), true);

  mesh.remove({1, 3, 4});
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({4})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({4})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 1})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 1})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 3})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 3})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 3})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 3})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3, 5})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 4})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 4})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3, 4})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3, 4})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({4, 5})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({4, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 2})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 2})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 4})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 4})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 6})), false);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 1, 3})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 1, 3})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 3, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 3, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({3, 4, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({3, 4, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 2, 4})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 2, 4})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({2, 4, 5})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({2, 4, 5})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({1, 2, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({1, 2, 6})), true);
  EXPECT_EQ(mesh.onBoundary(mesh.get_simplex_up({0, 1, 6})), true);
  EXPECT_EQ(mesh.nearBoundary(mesh.get_simplex_up({0, 1, 6})), true);
}
