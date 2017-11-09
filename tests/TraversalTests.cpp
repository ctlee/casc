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
#include <queue>
#include "gtest/gtest.h"
#include "SimplicialComplex.h"
#include "SimplexSet.h"
#include "CASCFunctions.h"

using SurfaceMeshType = casc::AbstractSimplicialComplex<
        int, // KEYTYPE
        int, // Root data
        int, // Vertex data
        int, // Edge data
        int // Face data
    >;

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

class CASCTraversalTest : public testing::Test {
protected:
    CASCTraversalTest() {}
    ~CASCTraversalTest() {}
    virtual void SetUp() {
        mesh.insert({0,1,2});
        mesh.insert({0,2,3});
        mesh.insert({0,3,4});
        mesh.insert({0,4,5});
        mesh.insert({0,5,6});
        mesh.insert({0,6,1});
        int prev = 6;
        int idx = 6;
        int skip = 1;
        int tmp;

        std::array<int,3> name;
        std::deque<int> deq {1,2,3,4,5,6};
        std::queue<int> p(deq); 
        std::queue<int> n;

        for(int ring = 0; ring < 5; ++ring){
            for (int i = 0; i < 3; ++i){
                if(i == 0){
                    tmp = p.front();
                    p.pop();
                }

                // std::cout << "----------" << std::endl;
               
                ++idx;
                n.push(idx);
                // std::cout << idx << std::endl;

                name = {prev, tmp, idx};
                // std::cout << name << std::endl;
                mesh.insert(name);
                
                ++idx;
                n.push(idx);
                // std::cout << idx << std::endl;

                name = {tmp, idx, idx-1};
                // std::cout << name << std::endl;
                mesh.insert(name);
                prev = idx;

                // std::cout << "----------" << std::endl;

                for(int j = 0; j < skip; ++j){
                    ++idx;
                    n.push(idx);
                    // std::cout << idx << std::endl;

                    if(idx%2 == 1){
                        name = {prev, idx, tmp};
                        // std::cout << name << std::endl;
                        mesh.insert(name);

                        tmp = p.front();
                        p.pop();

                        name = {idx, tmp, tmp-1};
                    }
                    else{
                        if (!p.empty()){
                            name = {prev, tmp, tmp+1};
                            // std::cout << name << std::endl;
                            mesh.insert(name);
                        
                            tmp = p.front();
                            p.pop();
                            name = {idx, prev, tmp};
                        }
                        else{
                            name = {idx, tmp, n.front()};
                            // std::cout << name << std::endl;
                            mesh.insert(name);
                            name = {idx, prev, tmp};
                        }
                    }
                    // std::cout << name << std::endl;
                    mesh.insert(name);
                    prev = idx;
                }
                ++skip;
            }
            --skip;
            ASSERT_TRUE(p.empty());
            p.swap(n);
            ASSERT_TRUE(n.empty());
        }
	}
    virtual void TearDown() {}

	SurfaceMeshType mesh;
	using SimplexSet = typename casc::SimplexSet<SurfaceMeshType>;
};

TEST_F(CASCTraversalTest, kNeighbors){
    auto nid = mesh.get_simplex_up({0});

    std::set<SurfaceMeshType::SimplexID<1> > nbors;

    int total = 0;
    casc::kneighbors_up(mesh, nid, 0, nbors);
    EXPECT_EQ(nbors.size(), total);
    nbors.clear();

    total += 6;
    casc::kneighbors_up(mesh, nid, 1, nbors);
    EXPECT_EQ(nbors.size(), total);
    nbors.clear();

    total += 2*6;
    casc::kneighbors_up(mesh, nid, 2, nbors);
    EXPECT_EQ(nbors.size(), total);
    nbors.clear();

    total += 3*6;
    casc::kneighbors_up(mesh, nid, 3, nbors);
    EXPECT_EQ(nbors.size(), total);
    nbors.clear();

    total += 4*6;
    casc::kneighbors_up(mesh, nid, 4, nbors);
    EXPECT_EQ(nbors.size(), total);
    nbors.clear();
}

