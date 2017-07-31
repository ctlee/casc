#pragma once

#include <iostream>

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include "util.h"
#include "SimplicialComplex.h"
#include "Orientable.h"
#include "Vertex.h"
/**
 * @brief      Properties that Faces should have
 */
struct FaceProperties
{
    int  marker;   /**< @brief Marker */
    bool selected; /**< @brief selection flag */
};

/**
 * @brief      Face object
 */
struct Face : Orientable, FaceProperties
{
    Face() {}
    Face(Orientable orient, FaceProperties prop)
        : Orientable(orient), FaceProperties(prop)
    {}
};

/**
 * @brief      Type for containing root metadata
 */
struct Global
{
    bool  closed;                /**< @brief is the surface mesh closed or not */
    int   _marker;               /**< @brief doman marker, to be used when tetrahedralizing */
    float volume_constraint;     /**< @brief volume constraint of the tetrahedralized domain */
    bool  use_volume_constraint; /**< @brief flag that determines if the volume constraint is used */
    float min[3];                /**< @brief minimal coordinate of nodes */
    float max[3];                /**< @brief maximal coordinate of nodes */
    float avglen;                /**< @brief average edge length */
    bool hole;                   /**< @brief flag that determines if the mesh is a hole or not */
};

/**
 * @brief      A helper struct containing the traits/types in the simplicial
 *             complex
 */
struct complex_traits
{
    using KeyType = int;                                                    /**< @brief the index type */
    using NodeTypes = util::type_holder<Global,Vertex,void,Face>;           /**< @brief the types of each Node */
    using EdgeTypes = util::type_holder<Orientable,Orientable,Orientable>;  /**< @brief the types of each Edge */
};

using SurfaceMesh = simplicial_complex<complex_traits>;


/**
 * READERS AND WRITERS
 */
// Geomview OFF
std::unique_ptr<SurfaceMesh> readOFF(const std::string& filename);
void writeOFF(const std::string& filename, const SurfaceMesh& mesh);

// Wavefront OBJ
std::unique_ptr<SurfaceMesh> readOBJ(const std::string& filename);
void writeOBJ(const std::string& filename, const SurfaceMesh& mesh);

void print(const SurfaceMesh& mesh);