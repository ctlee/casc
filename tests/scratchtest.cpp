/**
 * This is a scratch space for testing new functions and such on the fly.
 */

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "ASCFunctions.h"
#include "stringutil.h"
#include "SurfaceMesh.h"
#include "SimplicialComplexVisitors.h"
#include "Vertex.h"

int  main(int argc, char *argv[])
{

    auto mesh = AbstractSimplicialComplex<int,int,int,int,int,int>();

    mesh.insert<3>({0,1,2});

    auto v = mesh.get_simplex_up({0,1,2});
    if(v != nullptr){
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
