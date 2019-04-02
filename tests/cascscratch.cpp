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

/**
 * This is a scratch space for testing new functions and such on the fly.
 */

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "../casc"
#include "typetraits.h"

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

/**
 * @brief      A helper struct containing the traits/types in the simplicial
 *             complex
 */
struct complex_traits
{
    using KeyType = int;                                                    /**< @brief the index type */
    using NodeTypes = util::type_holder<int, int, int, int>;           /**< @brief the types of each Node */
    using EdgeTypes = util::type_holder<casc::Orientable,casc::Orientable,casc::Orientable>;  /**< @brief the types of each Edge */
};

using SurfaceMesh = casc::simplicial_complex<complex_traits>;


int  main(int argc, char *argv[])
{
    SurfaceMesh mesh;

    std::cout << type_name<decltype(mesh)>() << std::endl;

    mesh.insert({0,1,3});
    mesh.insert({0,3,5});
    mesh.insert({1,3,4});
    mesh.insert({3,4,5});
    mesh.insert({1,2,4});
    mesh.insert({2,4,5});

    auto s = mesh.get_simplex_up({3,4});
    // casc::decimate(mesh, s, Callback<SurfaceMesh>());
    casc::SimplexMap<SurfaceMesh> sM;
    casc::decimateFirstHalf(mesh, s, sM);

    std::cout << sM << std::endl;
    typename casc::decimation_detail::SimplexDataSet<SurfaceMesh>::type rv;
    casc::run_user_callback(mesh, sM, Callback<SurfaceMesh>(), rv);

    casc::decimateBackHalf(mesh, sM, rv);

    std::cout << "EOF" << std::endl;
}
