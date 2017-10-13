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

#pragma once

#include <iostream>
#include <queue>
#include <set>


namespace casc{

struct Orientable {
	int orientation;
};


/// Namespace for orientation helpers
namespace casc_orientable{

template <class Complex, class SizeT >
struct init_orientation_helper {};

template <class Complex, std::size_t k >
struct init_orientation_helper<Complex, std::integral_constant<std::size_t, k>>
{
	static void f(Complex& F)
	{
		for(auto curr : F.template get_level_id<k>())
		{
			for(auto a : F.get_cover(curr))
			{
				int orient = 1;
				for(auto b : F.get_name(curr))
				{
					// Count the number of indices > name
					if(a > b)
					{
						orient *= -1;
					}
					else
					{
						break;
					}
				}
				(*F.get_edge_up(curr,a)).orientation = orient;
			}
		}

		init_orientation_helper<Complex,std::integral_constant<std::size_t,k+1>>::f(F);
	}
};

template <typename Complex>
struct init_orientation_helper<Complex, std::integral_constant<std::size_t, Complex::topLevel>>
{
	static void f(Complex& F) {}
};
} // end namespace casc_orientable

// TODO: Implement this as a disjoint set operation during insertion (0)
template <typename Complex>
std::tuple<int, bool, bool> compute_orientation(Complex& F)
{
	// TODO: separate out init and clear orientation for speed (5)
	// init orientation
	casc_orientable::init_orientation_helper<Complex,std::integral_constant<std::size_t,0>>::f(F);

	// clear orientation
	for(auto& curr : F.template get_level<Complex::topLevel>())
	{
		curr.orientation = 0;
	}

	// compute orientation
	constexpr std::size_t k = Complex::topLevel - 1;

	std::queue<typename Complex::template SimplexID<k>> frontier;
	std::set<typename Complex::template SimplexID<k>> visited;
	int connected_components = 0;
	bool orientable = true;
	bool psuedo_manifold = true;
	for(auto outer : F.template get_level_id<k>())
	{
//		if(!F.get(outer))
		if(visited.find(outer) == visited.end())
		{
			++connected_components;
			frontier.push(outer);

			while(!frontier.empty())
			{
				typename Complex::template SimplexID<k> curr = frontier.front();
//				if(!F.get(curr))
				if(visited.find(curr) == visited.end())
				{
//					F.get(curr) = 1;
					visited.insert(curr);

					auto w = F.get_cover(curr);

					if(w.size() == 1)
					{
						//std::cout << curr << ":" << w[0] << " ~ Boundary" << std::endl;
					}
					else if(w.size() == 2)
					{
						// std::cout << curr << " " << w[0] << std::endl;
						// std::cout << curr << " " << w[1] << std::endl;
						auto& edge0 = *F.get_edge_up(curr, w[0]);
						auto& edge1 = *F.get_edge_up(curr, w[1]);

						auto& node0 = *F.get_simplex_up(curr, w[0]);
						auto& node1 = *F.get_simplex_up(curr, w[1]);

						if(node0.orientation == 0)
						{
							if(node1.orientation == 0)
							{
								node0.orientation = 1;
								node1.orientation = -edge1.orientation * edge0.orientation * node0.orientation;
							}
							else
							{
								node0.orientation = -edge0.orientation * edge1.orientation * node1.orientation;
							}
						}
						else
						{
							if(node1.orientation == 0)
							{
								node1.orientation = -edge1.orientation * edge0.orientation * node0.orientation;
							}
							else
							{
								if(edge0.orientation*node0.orientation + edge1.orientation*node1.orientation != 0)
								{
									orientable = false;
									std::cout << "+++++" << std::endl;
									std::cout << edge0.orientation << " : " << node0.orientation << std::endl;
									std::cout << edge1.orientation << " : " << node1.orientation << std::endl;

									std::cout << " : "
									          << edge0.orientation*node0.orientation + edge1.orientation*node1.orientation
									          << std::endl;
									std::cout << "-----"
									          << std::endl;
									std::cout << "Non-Orientable: "
									          << edge0.orientation*node0.orientation + edge1.orientation*node1.orientation
									          << std::endl;
								}
							}
						}

						std::vector<typename Complex::template SimplexID<k>> tmp;
						neighbors_up<Complex, k, decltype(std::back_inserter(tmp))>(F, curr, std::back_inserter(tmp));
						for(auto nid : tmp)
						{
							frontier.push(nid);
						}
					}
					else
					{
						psuedo_manifold = false;
					}
				}
				frontier.pop();
			}
		}
	}

	return std::make_tuple(connected_components, orientable, psuedo_manifold);
}
} // end namespace casc