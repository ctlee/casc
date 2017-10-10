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
 * ****************************************************************************
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
#include "SimplicialComplex.h"
#include "SimplicialComplexVisitors.h"

int  main(int argc, char *argv[])
{

    auto mesh = AbstractSimplicialComplex<
        int, // KEYTYPE
        int, // Root data
        int, // Vertex data
        int, // Edge data
        int, // Face data
        int  // Volume data
    >();

    mesh.insert<3>({0,1,2});

    auto v = mesh.get_simplex_up({0,1,2});
    if(v != nullptr){
        std::cout << v << std::endl;
        for(auto c : mesh.get_name(v)){
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }
    else{
        std::cout << "Simplex v doesn't exist" << std::endl;
    }
    
    std::cout << "EOF" << std::endl;
}
