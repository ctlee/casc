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
 * @file  SimplexSet.h
 * @brief SimplexSet data structure and associated convenience functions.
 */

#pragma once

#include "util.h"
#include <algorithm>
#include <unordered_set>

namespace casc {

/**
 * @brief      A multiset to store simplices in a simplicial_complex.
 *
 * This is really a tuple of sets where each set corresponds to a simplex
 * dimension. Many convenience functions are wrapped so this behaves much like
 * a std::set.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex> struct SimplexSet {
  /// Alias for SimplexID
  template <std::size_t j>
  using SimplexID = typename Complex::template SimplexID<j>;
  /// Index sequence of types from the simplicial_complex
  using LevelIndex = typename Complex::LevelIndex;
  /// Index sequence starting at 1
  using cLevelIndex =
      typename util::remove_first_val<std::size_t, LevelIndex>::type;
  /// Reversed index sequence
  using RevIndex =
      typename util::reverse_sequence<std::size_t, LevelIndex>::type;
  /// Reversed index sequence stops at 1
  using cRevIndex =
      typename util::reverse_sequence<std::size_t, cLevelIndex>::type;
  /// Typename of this
  using type_this = SimplexSet<Complex>;

  /// Tuple of SimplexIDs wrt an integral level.
  using SimplexIDLevel =
      typename util::int_type_map<std::size_t, std::tuple, LevelIndex,
                                  SimplexID>::type;
  // No real sense to hide this tuple of sets from the end users.
  // Making it private, we'd have to introduce lots of friend structs.
  /// Tuple of NodeSets per level.
  typename util::type_map<SimplexIDLevel, NodeSet>::type tupleSet;

  /// Default constructor
  SimplexSet(){};
  /// Default destructor
  ~SimplexSet(){};

  // type_this& operator=(const type_this& other){
  //     util::int_for_each<std::size_t, LevelIndex>(CopyHelper(), this, other);
  // }

  // type_this& operator=(type_this&& other){
  //     util::int_for_each<std::size_t, LevelIndex>(CopyHelper(), this, other);
  // }

  /**
   * @brief      Checks if a level has no elements.
   *
   * @tparam     k     Level to check.
   *
   * @return     True if the container is empty, false otherwise.
   */
  template <std::size_t k> inline auto empty() const noexcept {
    return std::get<k>(tupleSet).empty();
  }

  /**
   * @brief      Return the number of elements in a level.
   *
   * @tparam     k     Simplex dimension to query
   *
   * @return     Returns the number of simplices of dimension `k` are in the
   *             set.
   */
  template <std::size_t k> inline auto size() const noexcept {
    return std::get<k>(tupleSet).size();
  }

  /**
   * @brief      Clear the contents.
   */
  void clear() {
    util::int_for_each<std::size_t, LevelIndex>(ClearHelper(), this);
  }

  /**
   * @brief      Insert a simplex into the set.
   *
   * @param[in]  s     Simplex to insert.
   *
   * @tparam     k     Simplex dimension of 's'.
   */
  template <std::size_t k> inline void insert(SimplexID<k> s) {
    std::get<k>(tupleSet).insert(s);
  }

  /**
   * @brief      Insert a SimplexSet into this.
   *
   * @param[in]  s     The SimplexSet to insert.
   */
  void insert(const SimplexSet<Complex> &s) {
    util::int_for_each<std::size_t, LevelIndex>(InsertHelper(), this, s);
  }

  /**
   * @brief      Remove a simplex from the set.
   *
   * @param[in]  s     Simplex to remove.
   *
   * @tparam     k     Simplex dimension of 's'.
   */
  template <std::size_t k> inline void erase(SimplexID<k> s) {
    std::get<k>(tupleSet).erase(s);
  }

  /**
   * @brief      Remove a set of simplices.
   *
   * @param[in]  s     SimplexSet to remove.
   */
  void erase(const SimplexSet<Complex> &s) {
    util::int_for_each<std::size_t, LevelIndex>(EraseHelper(), this, s);
  }

  /**
   * @brief      Get the simplex of interest.
   *
   * @param[in]  s     The simplex to search for.
   *
   * @tparam     k     Simplex dimension of 's'.
   *
   * @return     Iterator to an element with key equivalent to s. If no such
   *             element is found, past-the-end iterator (see end()) is
   *             returned.
   */
  template <std::size_t k> inline auto find(const SimplexID<k> s) {
    return std::get<k>(tupleSet).find(s);
  }

  /**
   * @brief      Get the simplex of interest.
   *
   * @param[in]  s     The simplex to search for.
   *
   * @tparam     k     Simplex dimension of 's'.
   *
   * @return     Iterator to an element with key equivalent to s. If no such
   *             element is found, past-the-end iterator (see end()) is
   *             returned.
   */
  template <std::size_t k> inline auto find(const SimplexID<k> s) const {
    return std::get<k>(tupleSet).find(s);
  }

  /**
   * @brief      Get the past-the-end iterator.
   *
   * @tparam     k     The simplex dimension to get iterator of.
   *
   * @return     Returns an iterator to the element following the last element
   *             of the set for the specified simplex dimension.
   */
  template <std::size_t k> inline auto end() {
    return std::get<k>(tupleSet).end();
  }

  /**
   * @brief      Get the past-the-end iterator.
   *
   * @tparam     k     The simplex dimension to get iterator of.
   *
   * @return     Returns an iterator to the element following the last element
   *             of the set for the specified simplex dimension.
   */
  template <std::size_t k> inline auto cend() const {
    return std::get<k>(tupleSet).cend();
  }

  /**
   * @brief      Get an interator to the first element of the container.
   *
   * @tparam     k    The simplex dimension to get iterator of.
   *
   * @return     Returns an iterator to the first element.
   */
  template <std::size_t k> inline auto begin() {
    return std::get<k>(tupleSet).begin();
  }

  /**
   * @brief      Get an interator to the first element of the container.
   *
   * @tparam     k    The simplex dimension to get iterator of.
   *
   * @return     Returns an iterator to the first element.
   */
  template <std::size_t k> inline auto cbegin() const {
    return std::get<k>(tupleSet).cbegin();
  }

  // /**
  //  * @brief      Get the NodeSet for a particular simplex dimension.
  //  *
  //  * @tparam     k     Simplex dimension to get.
  //  *
  //  * @return     Returns the NodeSet corresponding to the requested
  //  dimension.
  //  */
  template <std::size_t k> inline auto &get() { return std::get<k>(tupleSet); }

  // /**
  //  * @brief      Get the NodeSet for a particular simplex dimension.
  //  *
  //  * @tparam     k     Simplex dimension to get.
  //  *
  //  * @return     Returns the NodeSet corresponding to the requested
  //  dimension.
  //  */
  template <std::size_t k> inline auto &get() const {
    return std::get<k>(tupleSet);
  }

  /**
   * @brief      Print the SimplexSet.
   *
   * See also casc::simplicial_complex::SimplexID::operator<<.
   *
   * @param      output  Handle to the stream to print to.
   * @param[in]  S       SimplexSet to print.
   *
   * @return     Handle to the stream.
   */
  friend std ::ostream &operator<<(std::ostream &output,
                                   const SimplexSet<Complex> &S) {
    output << "SimplexSet(";
    util::int_for_each<std::size_t, LevelIndex>(PrintHelper(), output, S);
    output << ")";
    return output;
  }

private:
  /**
   * @brief      Helper struct to insert a SimplexSet.
   */
  struct InsertHelper {
    /**
     * @brief      Perform the insertion for a dimension.
     *
     * @param      that  Typename of this SimplexSet.
     * @param[in]  S     SimplexSet to insert
     *
     * @tparam     k     Simplex dimension to insert.
     */
    template <std::size_t k>
    static void apply(type_this *that, const SimplexSet<Complex> &S) {
      auto s = std::get<k>(S.tupleSet);
      for (auto simplex : s) {
        that->insert(simplex);
      }
    }
  };

  /**
   * @brief      Helper struct to compute a set difference.
   */
  struct EraseHelper {
    /**
     * @brief      Perform the set difference for a dimension.
     *
     * @param      that  Typename of this SimplexSet.
     * @param[in]  S     SimplexSet to remove from this.
     *
     * @tparam     k     Simplex dimension to erase.
     */
    template <std::size_t k>
    static void apply(type_this *that, const SimplexSet<Complex> &S) {
      auto s = std::get<k>(S.tupleSet);
      for (auto simplex : s) {
        that->erase(simplex);
      }
    }
  };

  /**
   * @brief      Helper struct to print the SimplexSet
   */
  struct PrintHelper {
    /**
     * @brief      Print the simplices in the level.
     *
     * @param      output  Handle to the stream to output to.
     * @param[in]  S       SimplexSet to print.
     *
     * @tparam     k       Simplex dimension to print.
     */
    template <std::size_t k>
    static void apply(std::ostream &output, const SimplexSet<Complex> &S) {
      output << "[l=" << k;
      auto s = std::get<k>(S.tupleSet);
      for (auto simplex : s) {
        output << ", " << simplex;
      }
      output << "]";
    }
  };

  /**
   * @brief      Helper struct to clear the SimplexSet.
   */
  struct ClearHelper {
    /**
     * @brief      Clear a dimension.
     *
     * @param      that  Typename of this SimplexSet.
     *
     * @tparam     k     Simplex dimension to clear.
     */
    template <std::size_t k> void apply(type_this *that) {
      auto &s = std::get<k>(that->tupleSet);
      s.clear();
    }
  };

  // struct CopyHelper
  // {
  //     template <std::size_t k>
  //     void apply(type_this& that, type_this& other){
  //         auto &s = that.get<k>();
  //         s = other.get<k>();
  //     }

  //     template <std::size_t k>
  //     void apply(type_this& that, type_this&& other){
  //         auto &s = that.get<k>();
  //         s = other.get<k>();
  //     }
  // };
};

/**
 * @brief      Get the NodeSet for a simplex dimension from a SimplexSet.
 *
 * @param      S        SimplexSet of interest.
 *
 * @tparam     k        Simplex dimension desired.
 * @tparam     Complex  Typename of the simplicial_complex.
 *
 * @return     A NodeSet which holds simplices of dimension 'k' and a member of
 *             SimplexSet 'S'.
 */
template <std::size_t k, typename Complex>
static inline auto &get(SimplexSet<Complex> &S) {
  return S.template get<k>();
}

/**
 * @overload
 */
template <std::size_t k, typename Complex>
static inline auto &get(const SimplexSet<Complex> &S) {
  return S.template get<k>();
}

/// @cond detail
/// Namespace for simplex container related helpers
namespace simplex_set_detail {

/**
 * @brief      Helper struct to compute the union of two SimplexSets.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex> struct UnionH {
  /**
   * @brief      Compute the union of two SimplexSets.
   *
   * \f$A\cup B\f$
   *
   * @param[in]  A     A SimplexSet
   * @param[in]  B     Another SimplexSet
   * @param[out] dest  The destination SimplexSet
   *
   * @tparam     k     The current simplex dimension to merge.
   */
  template <std::size_t k>
  static void apply(const SimplexSet<Complex> &A, const SimplexSet<Complex> &B,
                    SimplexSet<Complex> &dest) {
    auto a = std::get<k>(A.tupleSet);
    auto b = std::get<k>(B.tupleSet);
    auto &d = std::get<k>(dest.tupleSet);
    d.insert(a.begin(), a.end());
    d.insert(b.begin(), b.end());
  }
};

/**
 * @brief      Helper struct to compute the intersection of two SimplexSets.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex> struct IntersectH {
  /**
   * @brief      Compute the intersection of two SimplexSets.
   *
   * \f$A\cap B\f$
   *
   * @param[in]  A     A SimplexSet
   * @param[in]  B     Another SimplexSet
   * @param      dest  The destination SimplexSet.
   *
   * @tparam     k     The current simplex dimension to merge.
   */
  template <std::size_t k>
  static void apply(const SimplexSet<Complex> &A, const SimplexSet<Complex> &B,
                    SimplexSet<Complex> &dest) {
    auto a = casc::get<k>(A);
    auto b = casc::get<k>(B);
    auto &d = casc::get<k>(dest);

    if (a.size() < b.size()) {
      for (auto item : a) {
        if (b.find(item) != b.end())
          d.insert(item);
      }
    } else {
      for (auto item : b) {
        if (a.find(item) != a.end())
          d.insert(item);
      }
    }
  }
};

/**
 * @brief      Helper struct to compute the set intersection.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex> struct DifferenceH {
  /**
   * @brief      Compute the set difference for a simplex dimension.
   *
   * \f$ dest = A \setminus B \f$
   *
   * @param[in]  A     A SimplexSet.
   * @param[in]  B     Remove this SimplexSet from A.
   * @param      dest  The destination SimplexSet.
   *
   * @tparam     k     The simplex dimension to compute the difference of.
   */
  template <std::size_t k>
  static void apply(const SimplexSet<Complex> &A, const SimplexSet<Complex> &B,
                    SimplexSet<Complex> &dest) {
    auto a = casc::get<k>(A);
    auto b = casc::get<k>(B);
    auto &d = casc::get<k>(dest);

    for (auto item : a) {
      if (b.find(item) == b.end())
        d.insert(item);
    }
  }
};

/**
 * @brief      Helper struct to compute set equivalence.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex> struct OperatorEQH {
  /// Result of the comparison
  bool result;

  /// Default constructor
  OperatorEQH() : result(true) {}

  /**
   * @brief      Compare the two sets by level.
   *
   * @param[in]  lhs   The left hand side
   * @param[in]  rhs   The right hand side
   *
   * @tparam     k     Level to compare.
   */
  template <std::size_t k>
  void apply(const SimplexSet<Complex> &lhs, const SimplexSet<Complex> &rhs) {
    auto a = casc::get<k>(lhs);
    auto b = casc::get<k>(rhs);
    result &= a == b;
  }
};
} // end namespace simplex_set_detail
/// @endcond

/**
 * @brief      Compare if the sets are equivalent
 *
 * @param[in]  lhs      The left hand side
 * @param[in]  rhs      The right hand side
 *
 * @tparam     Complex  Typename of the simplicial_complex
 *
 * @return     True if the sets are equal, false otherwise.
 */
template <typename Complex>
bool operator==(const SimplexSet<Complex> &lhs,
                const SimplexSet<Complex> &rhs) {
  auto func = simplex_set_detail::OperatorEQH<Complex>();
  util::int_for_each<std::size_t, typename Complex::LevelIndex>(func, lhs, rhs);
  return func.result;
}

/**
 * @brief      Compare if the sets are not equivalent.
 *
 * @param[in]  lhs      The left hand side
 * @param[in]  rhs      The right hand side
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 *
 * @return     True if the sets are inequal, false otherwise.
 */
template <typename Complex>
bool operator!=(const SimplexSet<Complex> &lhs,
                const SimplexSet<Complex> &rhs) {
  return !(lhs == rhs);
}

/**
 * @brief      Compute the set union.
 *
 * @param[in]  A        A SimplexSet
 * @param[in]  B        Another SimplexSet
 * @param[out] dest     The destination SimplexSet.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex>
static void set_union(const SimplexSet<Complex> &A,
                      const SimplexSet<Complex> &B, SimplexSet<Complex> &dest) {
  util::int_for_each<std::size_t, typename Complex::LevelIndex>(
      simplex_set_detail::UnionH<Complex>(), A, B, dest);
}

/**
 * @brief      Compute the set intersection.
 *
 * @param[in]  A        A SimplexSet
 * @param[in]  B        Another SimplexSet
 * @param[out] dest     The destination SimplexSet.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex>
static void set_intersection(const SimplexSet<Complex> &A,
                             const SimplexSet<Complex> &B,
                             SimplexSet<Complex> &dest) {
  util::int_for_each<std::size_t, typename Complex::LevelIndex>(
      simplex_set_detail::IntersectH<Complex>(), A, B, dest);
}

/**
 * @brief      Compute the set difference.
 *
 * @param[in]  A        A SimplexSet
 * @param[in]  B        Another SimplexSet
 * @param[out] dest     The destination SimplexSet.
 *
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Complex>
static void set_difference(const SimplexSet<Complex> &A,
                           const SimplexSet<Complex> &B,
                           SimplexSet<Complex> &dest) {
  util::int_for_each<std::size_t, typename Complex::LevelIndex>(
      simplex_set_detail::DifferenceH<Complex>(), A, B, dest);
}
} // end namespace casc
