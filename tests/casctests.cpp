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



#include <iostream>
#include <map>
#include <cmath>
#include "gtest/gtest.h"
#include "SimplicialComplex.h"



using MeshType = casc::AbstractSimplicialComplex<
        int, // KEYTYPE
        int, // Root data
        int, // Vertex data
        int, // Edge data
        int // Face data
    >;

using MeshTypeTet = casc::AbstractSimplicialComplex<
        int, // KEYTYPE
        int, // Root data
        int, // Vertex data
        int, // Edge data
        int, // Face data
        int
    >;


TEST(CASCTest, TestStructure){

	MeshType mesh = MeshType();
	EXPECT_EQ(1, mesh.size<0>());
	EXPECT_EQ(0, mesh.size<1>());
	EXPECT_EQ(0, mesh.size<2>());
	EXPECT_EQ(0, mesh.size<3>());

	// Vertex v1 = Vertex();
	// mesh.insert<1>({1}, v1);
	// Vertex v2 = Vertex(1,0,0);
	// mesh.insert<1>({2}, v2);
	// Vertex v3 = Vertex(0,0,1);
 //    mesh.insert<1>({3}, v3);
 //   	Vertex v4 = Vertex(0,1,0);
 //    mesh.insert<1>({4}, v4);
	// EXPECT_EQ(1, mesh.size<0>());
	// EXPECT_EQ(4, mesh.size<1>());
	// EXPECT_EQ(0, mesh.size<2>());
	// EXPECT_EQ(0, mesh.size<3>());

	// mesh.insert<3>({1,2,3});
 //    mesh.insert<3>({2,3,4});
 //    mesh.insert<3>({1,3,4});
	// EXPECT_EQ(1, mesh.size<0>());
	// EXPECT_EQ(4, mesh.size<1>());
	// EXPECT_EQ(6, mesh.size<2>());
	// EXPECT_EQ(3, mesh.size<3>());

	// auto node1Data = *mesh.get_simplex_up<1>({1});
	// EXPECT_EQ(node1Data, v1);

	// auto node2Data = *mesh.get_simplex_up<1>({2});
	// EXPECT_EQ(node2Data, v2);

	// auto node3Data = *mesh.get_simplex_up<1>({3});
	// EXPECT_EQ(node3Data, v3);

	// auto node4Data = *mesh.get_simplex_up<1>({4});
	// EXPECT_EQ(node4Data, v4);

	// auto result = compute_orientation(mesh);

	// EXPECT_EQ(true, std::get<1>(result));
	// EXPECT_EQ(true, std::get<2>(result));
}