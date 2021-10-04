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

#pragma once

#include <iostream>

#include "SimplicialComplex.h"

#include "Orientable.h"
#include "Vertex.h"
#include "util.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
/**
 * @brief      Properties that Faces should have
 */
struct FaceProperties {
  int marker;    /**< @brief Marker */
  bool selected; /**< @brief selection flag */
};

/**
 * @brief      Face object
 */
struct Face : casc::Orientable, FaceProperties {
  Face() {}
  Face(Orientable orient, FaceProperties prop)
      : Orientable(orient), FaceProperties(prop) {}
};

/**
 * @brief      Type for containing root metadata
 */
struct Global {
  bool closed; /**< @brief is the surface mesh closed or not */
  int _marker; /**< @brief doman marker, to be used when tetrahedralizing */
  float volume_constraint; /**< @brief volume constraint of the tetrahedralized
                              domain */
  bool use_volume_constraint; /**< @brief flag that determines if the volume
                                 constraint is used */
  float min[3];               /**< @brief minimal coordinate of nodes */
  float max[3];               /**< @brief maximal coordinate of nodes */
  float avglen;               /**< @brief average edge length */
  bool hole; /**< @brief flag that determines if the mesh is a hole or not */
};

/**
 * @brief      A helper struct containing the traits/types in the simplicial
 *             complex
 */
struct complex_traits {
  using KeyType = int; /**< @brief the index type */
  using NodeTypes =
      util::type_holder<Global, Vertex, void,
                        Face>; /**< @brief the types of each Node */
  using EdgeTypes =
      util::type_holder<casc::Orientable, casc::Orientable,
                        casc::Orientable>; /**< @brief the types of each Edge */
};

using SurfaceMesh = casc::simplicial_complex<complex_traits>;

/**
 * READERS AND WRITERS
 */
// Geomview OFF
std::unique_ptr<SurfaceMesh> readOFF(const std::string &filename);
void writeOFF(const std::string &filename, const SurfaceMesh &mesh);

// Wavefront OBJ
std::unique_ptr<SurfaceMesh> readOBJ(const std::string &filename);
void writeOBJ(const std::string &filename, const SurfaceMesh &mesh);

void print(const SurfaceMesh &mesh);
