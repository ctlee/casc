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
#include "gtest/gtest.h"
#include "SimplicialComplex.h"
#include "SimplexSet.h"
#include "decimate.h"

using SurfaceMeshType = casc::AbstractSimplicialComplex<
        int, // KEYTYPE
        int, // Root data
        int, // Vertex data
        int, // Edge data
        int // Face data
    >;

class DecimationTest : public testing::Test {
protected:
    DecimationTest() {}
    ~DecimationTest() {}
    virtual void SetUp() {
    	mesh.insert({0,1,3});
    	mesh.insert({0,3,5});
    	mesh.insert({1,3,4});
    	mesh.insert({3,4,5});
    	mesh.insert({1,2,4});
    	mesh.insert({2,4,5});
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

template <typename Complex>
struct Callback
{
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using KeyType = typename Complex::KeyType;

    template <std::size_t k>
    int operator()(Complex& F,
            const std::array<KeyType, k>& new_name,
            const SimplexSet& merged){
        // std::cout << merged << " -> " << new_name << std::endl;
        return 0;
    }
};

TEST_F(DecimationTest, Decimation){
    auto s = mesh.get_simplex_up({3,4});
    casc::decimate(mesh, s, Callback<SurfaceMeshType>());
}