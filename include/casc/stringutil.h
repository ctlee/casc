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
// or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301 USA

/**
 * @file  stringutil.h
 * @brief String utilities for CASC.
 */

#pragma once

#include <string>

namespace casc {
/**
 * @brief      Returns a string representation of the vertex subsimplicies
 *             of a given simplex
 *
 * @param[in]  A     Array containing name of a simplex.
 *
 * @tparam     T     Typename KeyType.
 * @tparam     k     Dimension of the simplex.
 *
 * @return     String representation of the object.
 */
template <typename T, std::size_t k>
std::string to_string(const std::array<T, k> &A) {
  if (k == 0) {
    return "{root}";
  }
  std::string out;
  out += "{";
  for (int i = 0; i + 1 < k; ++i) {
    out += std::to_string(A[i]) + ",";
  }
  if (k > 0) {
    out += std::to_string(A[k - 1]);
  }
  out += "}";
  return out;
}
} // end namespace casc
