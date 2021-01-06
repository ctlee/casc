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

#include "gtest/gtest.h"
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <set>

#include <casc/casc>

using TetMeshType = casc::AbstractSimplicialComplex<int, // KEYTYPE
                                                    int, // Root data
                                                    int, // Vertex data
                                                    int, // Edge data
                                                    int, // Face data
                                                    int>;

class SimplexSetTest : public testing::Test {
protected:
  SimplexSetTest() {}
  ~SimplexSetTest() {}
  virtual void SetUp() { mesh.insert({1, 2, 3, 4}); }
  virtual void TearDown() {
    S.clear();
    EXPECT_TRUE(S.empty<1>());
    EXPECT_TRUE(S.empty<2>());
    EXPECT_TRUE(S.empty<3>());
    EXPECT_TRUE(S.empty<4>());
  }
  TetMeshType mesh;
  using SimplexSet = typename casc::SimplexSet<TetMeshType>;
  SimplexSet S;
};

TEST_F(SimplexSetTest, SimplexSetInsertRemove) {
  EXPECT_TRUE(S.empty<1>());
  EXPECT_TRUE(S.empty<2>());
  EXPECT_TRUE(S.empty<3>());
  EXPECT_TRUE(S.empty<4>());

  for (auto s : mesh.get_level_id<1>())
    S.insert(s);

  EXPECT_EQ(4, S.size<1>());
  EXPECT_EQ(0, S.size<2>());
  EXPECT_EQ(0, S.size<3>());
  EXPECT_EQ(0, S.size<4>());

  auto t1 = S.get<1>();
  EXPECT_EQ(4, t1.size());

  auto s = mesh.get_simplex_up({4});
  S.erase(s);
  EXPECT_EQ(3, S.size<1>());
}

TEST_F(SimplexSetTest, SimplexSetUnion) {
  SimplexSet S2, dest;

  S.insert(mesh.get_simplex_up({1}));
  S.insert(mesh.get_simplex_up({2}));
  S2.insert(mesh.get_simplex_up({2}));
  S2.insert(mesh.get_simplex_up({1, 2, 3, 4}));

  casc::set_union(S, S2, dest);
  EXPECT_EQ(2, dest.size<1>());
  EXPECT_EQ(0, dest.size<2>());
  EXPECT_EQ(0, dest.size<3>());
  EXPECT_EQ(1, dest.size<4>());
}

TEST_F(SimplexSetTest, SimplexSetIntersection) {
  SimplexSet S2, dest;

  S.insert(mesh.get_simplex_up({1}));
  S.insert(mesh.get_simplex_up({2}));
  S2.insert(mesh.get_simplex_up({2}));
  S2.insert(mesh.get_simplex_up({1, 2, 3, 4}));

  casc::set_intersection(S, S2, dest);
  EXPECT_EQ(1, dest.size<1>());
  EXPECT_EQ(0, dest.size<2>());
  EXPECT_EQ(0, dest.size<3>());
  EXPECT_EQ(0, dest.size<4>());

  auto it = dest.find(mesh.get_simplex_up({2}));
  EXPECT_NE(it, dest.end<1>());
}

TEST_F(SimplexSetTest, SimplexSetDifference) {
  SimplexSet S2, dest;

  S.insert(mesh.get_simplex_up({1}));
  S.insert(mesh.get_simplex_up({2}));
  S2.insert(mesh.get_simplex_up({2}));
  S2.insert(mesh.get_simplex_up({1, 2, 3, 4}));

  casc::set_difference(S, S2, dest);
  EXPECT_EQ(1, dest.size<1>());
  EXPECT_EQ(0, dest.size<2>());
  EXPECT_EQ(0, dest.size<3>());
  EXPECT_EQ(0, dest.size<4>());

  EXPECT_EQ(dest.find(mesh.get_simplex_up({2})), dest.end<1>());
  EXPECT_NE(dest.find(mesh.get_simplex_up({1})), dest.end<1>());
}

TEST_F(SimplexSetTest, SimplexSetComparisons) {
  SimplexSet S2;
  S.insert(mesh.get_simplex_up({1}));
  S.insert(mesh.get_simplex_up({2}));
  S.insert(mesh.get_simplex_up({1, 2, 3, 4}));

  S2.insert(mesh.get_simplex_up({1}));
  S2.insert(mesh.get_simplex_up({2}));
  S2.insert(mesh.get_simplex_up({1, 2, 3, 4}));
  EXPECT_TRUE(S == S2);
  EXPECT_TRUE(S2 == S);

  S2.erase(mesh.get_simplex_up({1, 2, 3, 4}));
  EXPECT_TRUE(S != S2);
  EXPECT_TRUE(S2 != S);
}
