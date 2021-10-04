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

#include <casc/casc>

#include "gtest/gtest.h"

using SurfaceMeshType = casc::AbstractSimplicialComplex<int, // KEYTYPE
                                                        int, // Root data
                                                        int, // Vertex data
                                                        int, // Edge data
                                                        int  // Face data
                                                        >;

class CASCFunctionsTest : public testing::Test {
protected:
  CASCFunctionsTest() {}
  ~CASCFunctionsTest() {}
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

TEST_F(CASCFunctionsTest, Star) {
  SimplexSet dest, cmp;
  auto s = mesh.get_simplex_up({3, 4});
  getStar(mesh, s, dest);

  cmp.insert(mesh.get_simplex_up({3, 4}));
  cmp.insert(mesh.get_simplex_up({1, 3, 4}));
  cmp.insert(mesh.get_simplex_up({3, 4, 5}));
  EXPECT_TRUE(dest == cmp);

  cmp.clear();
  dest.clear();

  SimplexSet S;
  S.insert(mesh.get_simplex_up({0}));
  S.insert(mesh.get_simplex_up({2}));
  getStar(mesh, S, dest);

  cmp.insert(mesh.get_simplex_up({0}));
  cmp.insert(mesh.get_simplex_up({0, 1}));
  cmp.insert(mesh.get_simplex_up({0, 3}));
  cmp.insert(mesh.get_simplex_up({0, 5}));
  cmp.insert(mesh.get_simplex_up({0, 1, 3}));
  cmp.insert(mesh.get_simplex_up({0, 3, 5}));
  cmp.insert(mesh.get_simplex_up({2}));
  cmp.insert(mesh.get_simplex_up({1, 2}));
  cmp.insert(mesh.get_simplex_up({2, 4}));
  cmp.insert(mesh.get_simplex_up({2, 5}));
  cmp.insert(mesh.get_simplex_up({1, 2, 4}));
  cmp.insert(mesh.get_simplex_up({2, 4, 5}));
  EXPECT_TRUE(dest == cmp);
}

TEST_F(CASCFunctionsTest, Closure) {
  SimplexSet dest, cmp;
  auto s = mesh.get_simplex_up({3, 4});
  getClosure(mesh, s, dest);

  cmp.insert(mesh.get_simplex_up({3, 4}));
  cmp.insert(mesh.get_simplex_up({3}));
  cmp.insert(mesh.get_simplex_up({4}));
  EXPECT_TRUE(dest == cmp);

  cmp.clear();
  dest.clear();

  SimplexSet S;
  S.insert(mesh.get_simplex_up({1, 3}));
  S.insert(mesh.get_simplex_up({4, 5}));
  getClosure(mesh, S, dest);

  cmp.insert(mesh.get_simplex_up({1}));
  cmp.insert(mesh.get_simplex_up({3}));
  cmp.insert(mesh.get_simplex_up({4}));
  cmp.insert(mesh.get_simplex_up({5}));
  cmp.insert(mesh.get_simplex_up({1, 3}));
  cmp.insert(mesh.get_simplex_up({4, 5}));
  EXPECT_TRUE(dest == cmp);
}

TEST_F(CASCFunctionsTest, Link) {
  SimplexSet dest, cmp;
  auto s = mesh.get_simplex_up({0});
  getLink(mesh, s, dest);

  cmp.insert(mesh.get_simplex_up({1}));
  cmp.insert(mesh.get_simplex_up({3}));
  cmp.insert(mesh.get_simplex_up({5}));
  cmp.insert(mesh.get_simplex_up({1, 3}));
  cmp.insert(mesh.get_simplex_up({3, 5}));
  EXPECT_TRUE(dest == cmp);

  cmp.clear();
  dest.clear();

  SimplexSet S;
  S.insert(mesh.get_simplex_up({0}));
  S.insert(mesh.get_simplex_up({2}));
  getLink(mesh, S, dest);

  cmp.insert(mesh.get_simplex_up({1}));
  cmp.insert(mesh.get_simplex_up({3}));
  cmp.insert(mesh.get_simplex_up({1, 3}));
  cmp.insert(mesh.get_simplex_up({3, 5}));

  cmp.insert(mesh.get_simplex_up({1}));
  cmp.insert(mesh.get_simplex_up({4}));
  cmp.insert(mesh.get_simplex_up({5}));
  cmp.insert(mesh.get_simplex_up({1, 4}));
  cmp.insert(mesh.get_simplex_up({4, 5}));
  EXPECT_TRUE(dest == cmp);
}
