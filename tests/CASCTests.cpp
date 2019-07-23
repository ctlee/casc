/*
 * ***************************************************************************
 * This file is part of the Colored Abstract Simplicial Complex library.
 * Copyright (C) 2016-2017
 * by Christopher Lee, John Moody, Rommie Amaro, J. Andrew McCammon,
 *    and Michael Holst
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ***************************************************************************
 */

#include <array>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <map>
#include <set>
#include <casc/casc>

#include "gtest/gtest.h"

using SurfaceMeshType = casc::AbstractSimplicialComplex<
        int, // KEYTYPE
        int, // Root data
        int, // Vertex data
        int, // Edge data
        int // Face data
    >;

using TetMeshType = casc::AbstractSimplicialComplex<
        int, // KEYTYPE
        int, // Root data
        int, // Vertex data
        int, // Edge data
        int, // Face data
        int
    >;

class CASCTestFix : public testing::Test {
protected:
    CASCTestFix() {}
    ~CASCTestFix() {}
    virtual void SetUp() {
    	mesh.insert<1>({1}, 1);
			mesh.insert<1>({2}, 2);
    	mesh.insert<1>({3}, 3);
    	mesh.insert<1>({4}, 4);

    	mesh.insert<3>({1,2,3});
   		mesh.insert<3>({2,3,4});
   		mesh.insert<3>({1,2,4});
    	mesh.insert<3>({1,3,4});
	}
    virtual void TearDown() {}

	SurfaceMeshType mesh;
};

TEST_F(CASCTestFix, SimplexIDTraversal){
	auto vid = mesh.get_simplex_up({1});
	auto eid = mesh.get_simplex_up({1,3});
	auto eid2 = vid.get_simplex_up(3);

	EXPECT_EQ(eid, eid2);

	auto fid = mesh.get_simplex_up({1,2,3});
	auto fid2 = vid.get_simplex_up({2,3});
	EXPECT_EQ(fid, fid2);

	eid2 = fid2.get_simplex_down(2);
  EXPECT_EQ(eid, eid2);

  auto vid2 = fid2.get_simplex_down({2,3});
  EXPECT_EQ(vid, vid2);
}

TEST(CASCTest, DefaultConstructor){
 	SurfaceMeshType mesh = SurfaceMeshType();
  EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(0, mesh.size<1>());
	EXPECT_EQ(0, mesh.size<2>());
	EXPECT_EQ(0, mesh.size<3>());
}

TEST(CASCTest, DefaultConstructorTet){
 	TetMeshType mesh = TetMeshType();
  EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(0, mesh.size<1>());
	EXPECT_EQ(0, mesh.size<2>());
	EXPECT_EQ(0, mesh.size<3>());
	EXPECT_EQ(0, mesh.size<4>());
}

TEST(CASCTest, Insert){
	SurfaceMeshType mesh = SurfaceMeshType();

	mesh.insert<1>({1}, 1);
	mesh.insert<1>({2}, 2);
  mesh.insert<1>({3}, 3);
  mesh.insert<1>({4}, 4);
	EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(4, mesh.size<1>());
	EXPECT_EQ(0, mesh.size<2>());
	EXPECT_EQ(0, mesh.size<3>());

	mesh.insert<3>({1,2,3}, 5);
  mesh.insert<3>({2,3,4}, 6);
  mesh.insert<3>({1,3,4}, 7);
	EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(4, mesh.size<1>());
	EXPECT_EQ(6, mesh.size<2>());
	EXPECT_EQ(3, mesh.size<3>());

	int data;
	data = *mesh.get_simplex_up<1>({1});
	EXPECT_EQ(data, 1);
	data = *mesh.get_simplex_up<1>({2});
	EXPECT_EQ(data, 2);
	data = *mesh.get_simplex_up<1>({3});
	EXPECT_EQ(data, 3);
	data = *mesh.get_simplex_up<1>({4});
	EXPECT_EQ(data, 4);
	data = *mesh.get_simplex_up({1,2,3});
	EXPECT_EQ(data, 5);
	data = *mesh.get_simplex_up({2,3,4});
	EXPECT_EQ(data, 6);
	data = *mesh.get_simplex_up({1,3,4});
	EXPECT_EQ(data, 7);
}

// Check the SimplexID boolean operators.
TEST(CASCTest, SimplexBooleanOps){
	TetMeshType mesh = TetMeshType();
	mesh.insert<4>({1,2,3,4});

	auto s1 = mesh.get_simplex_up({1});
	auto s2 = mesh.get_simplex_up({1,2});
	auto s3 = mesh.get_simplex_up({1,2,3});
	auto s4 = mesh.get_simplex_up({1,2,3,4});

	EXPECT_TRUE(
		mesh.eq(s1,s1) && mesh.eq(s2,s2) && mesh.eq(s3,s3) && mesh.eq(s4,s4)
	);

	EXPECT_FALSE(mesh.lt(s2,s1));
	EXPECT_TRUE(mesh.lt(s1,s4));
	EXPECT_TRUE(mesh.leq(s1,s1));
	EXPECT_TRUE(mesh.leq(s1,s2));
}

// Verify that the removal functions are removing the correct things.
TEST(CASCTest, Remove){
	TetMeshType mesh = TetMeshType();
	mesh.insert<4>({1,2,3,4});
	std::size_t removed = mesh.remove({3,4});
	EXPECT_EQ(4, removed);
	EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(4, mesh.size<1>());
	EXPECT_EQ(5, mesh.size<2>());
	EXPECT_EQ(2, mesh.size<3>());
	EXPECT_EQ(0, mesh.size<4>());

	auto s = mesh.get_simplex_up({3,4});
	EXPECT_EQ(s, nullptr) << "Simplex {3,4} should no longer exist.";
	EXPECT_FALSE(mesh.exists({1,3,4}));
	auto s2 = mesh.get_simplex_up({1,2,3,4});
	EXPECT_EQ(s2, nullptr) << "Simplex {1,2,3,4} should no longer exist.";


	mesh.insert<4>({1,2,3,4});
	std::array<int, 2> name = {3,4};
	removed = mesh.remove(name);
	EXPECT_EQ(4, removed);
	EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(4, mesh.size<1>());
	EXPECT_EQ(5, mesh.size<2>());
	EXPECT_EQ(2, mesh.size<3>());
	EXPECT_EQ(0, mesh.size<4>());

	s = mesh.get_simplex_up({3,4});
	EXPECT_EQ(s, nullptr) << "Simplex {3,4} should no longer exist.";
	EXPECT_FALSE(mesh.exists({1,3,4}));
	s2 = mesh.get_simplex_up({1,2,3,4});
	EXPECT_EQ(s2, nullptr) << "Simplex {1,2,3,4} should no longer exist.";

	mesh.insert<4>({1,2,3,4});
	s = mesh.get_simplex_up({3,4});
	removed = mesh.remove(s);
	EXPECT_EQ(4, removed);
	EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(4, mesh.size<1>());
	EXPECT_EQ(5, mesh.size<2>());
	EXPECT_EQ(2, mesh.size<3>());
	EXPECT_EQ(0, mesh.size<4>());

	s = mesh.get_simplex_up({3,4});
	EXPECT_EQ(s, nullptr) << "Simplex {3,4} should no longer exist.";
	EXPECT_FALSE(mesh.exists({1,3,4}));
	s2 = mesh.get_simplex_up({1,2,3,4});
	EXPECT_EQ(s2, nullptr) << "Simplex {1,2,3,4} should no longer exist.";
}

// Insert `trials` number of vertices with random integer data. Then check if
// the inserted data is indeed correct.
TEST(CASCTest, InsertRandomVals){
	int trials = 100;

	std::srand(static_cast<unsigned int>(std::time(0)));
	int randomInt, key, name;
	SurfaceMeshType mesh = SurfaceMeshType();
	std::map<int, int> pairs; // Map to cache the data

	for(int i = 0; i < trials; i++){
		randomInt = std::rand();

		key = mesh.add_vertex();
		auto &data = *mesh.get_simplex_up({key});
		data = randomInt;

		pairs.emplace(key, randomInt);
	}
	EXPECT_EQ(mesh.size<1>(), trials)
		<< "Number of vertices does not match number inserted.";

	for(auto simplex : mesh.get_level_id<1>()){
		name = mesh.get_name(simplex)[0];
		auto search = pairs.find(name);
		if (search != pairs.end()){
			// check that the data is equal to the cached
			EXPECT_EQ(search->second, *simplex)
					<< "Data does not match inserted value.";
			pairs.erase(search);
		}
		else {
			FAIL() << "We shouldn't get here...";
		}
	}
	// Ensure that all vertices were removed...
	EXPECT_EQ(0, pairs.size())
		<< "All vertices should have been checked.";
}