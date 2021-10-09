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
 * @file  SimplexMap.h
 * @brief SimplexMap data structure and associated convenience functions.
 */

#pragma once

#include "stringutil.h"
#include "util.h"
#include <array>
#include <map>

namespace casc {

/**
 * @brief      A multimap to represent a map of simplex indices to a set of
 *             simplices.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex> struct SimplexMap {
  /// Alias for SimplexID
  template <std::size_t j>
  using SimplexID = typename Complex::template SimplexID<j>;
  /// Index sequence of types from the simplicial_complex
  using LevelIndex = typename Complex::LevelIndex;
  /// Index sequence starting at 1
  using cLevelIndex =
      typename util::remove_first_val<std::size_t, LevelIndex>::type;
  /// Reversed Index sequence
  using RevIndex =
      typename util::reverse_sequence<std::size_t, LevelIndex>::type;
  /// Reversed index sequence stops at 1
  using cRevIndex =
      typename util::reverse_sequence<std::size_t, cLevelIndex>::type;
  /// Typename of this object
  using type_this = SimplexMap<Complex>;

  /**
   * @brief      Default constructor.
   */
  SimplexMap(){};

  // TODO: Put in convenience functions for easy accession etc... (0)
  /**
   * @brief      Get the map for a particular simplex dimension.
   *
   * @tparam     k     Simplex dimension to retrieve.
   *
   * @return     A map of SimplexID<k> to SimplexSet.
   */
  template <std::size_t k> inline auto &get() { return std::get<k>(tupleMap); }

  /**
   * @overload
   */
  template <std::size_t k> inline auto &get() const {
    return std::get<k>(tupleMap);
  }

  /**
   * @brief      Print the SimplexMap.
   *
   * @param      output  Handle to the stream to print to.
   * @param[in]  S       SimplexMap to print.
   *
   * @return     Handle to the stream.
   */
  friend std ::ostream &operator<<(std::ostream &output,
                                   const SimplexMap<Complex> &S) {
    output << "SimplexMap(";
    util::int_for_each<std::size_t, LevelIndex>(PrintHelper(), output, S);
    output << ")";
    return output;
  }

private:
  /**
   * @brief      Helper struct to print the SimplexMap.
   */
  struct PrintHelper {
    /**
     * @brief      Print the SimplexMap.
     *
     * @param      output  Handle to the stream to print to.
     * @param[in]  S       SimplexMap to print.
     *
     * @tparam     k       The simplex dimension to print.
     */
    template <std::size_t k>
    static void apply(std::ostream &output, const SimplexMap<Complex> &S) {
      output << "[l=" << k;
      auto s = std::get<k>(S.tupleMap);
      for (auto simplex : s) {
        output << ", " << to_string(simplex.first) << ":" << simplex.second;
      }
      output << "]";
    }
  };

  /// Alias to create an Array of size k to store keys.
  template <std::size_t k>
  using array = std::array<typename Complex::KeyType, k>;
  /// A tuple of arrays of increasing size.
  using ArrayLevel = typename util::int_type_map<std::size_t, std::tuple,
                                                 LevelIndex, array>::type;
  /// Alias for a Map of type T to a SimplexSet.
  template <class T> using map = std::map<T, SimplexSet<Complex>>;
  /// The full tuple of maps of an Array of keys to SimplexSet.
  typename util::type_map<ArrayLevel, map>::type tupleMap;
};

/**
 * @brief      Get the map for a simplex dimension.
 *
 * @param      S        SimplexMap to retrieve from.
 *
 * @tparam     k        Simplex dimension.
 * @tparam     Complex  Typename of the complex.
 *
 * @return     Returns a map of std::Array<KeyType, k> to SimplexSet.
 */
template <std::size_t k, typename Complex>
static inline auto &get(SimplexMap<Complex> &S) {
  return S.template get<k>();
}

/// @overload
template <std::size_t k, typename Complex>
static inline auto &get(const SimplexMap<Complex> &S) {
  return S.template get<k>();
}
} // end namespace casc
