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
 * @file  CASCTraversals.h
 * @brief Implementations of various advanced traversals such as by neighborhood
 *        and breadth first search.
 */

#pragma once

#include <casc/casc>
#include <iostream>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace casc {
/// @cond detail
/// Visitor design pattern helper templates
namespace visitor_detail {

/**
 * @brief      General template for BFS up helper.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 * @tparam     K        Current simplex dimension to traverse.
 */
template <typename Visitor, typename Traits, typename Complex, typename K>
struct BFS_Up_Node {};

/**
 * @brief      Partial specialization for BFS up helper for non facet
 *             dimensions.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 * @tparam     k        Current simplex dimension to traverse.
 */
template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct BFS_Up_Node<Visitor, Traits, Complex,
                   std::integral_constant<std::size_t, k>> {
  /// Simplex dimension currently traversed
  static constexpr auto level = k;
  /// Typename of the current simplex
  using CurrSimplexID = typename Complex::template SimplexID<level>;
  /// Typename of coboundary simplices
  using NextSimplexID = typename Complex::template SimplexID<level + 1>;
  /// Container to use to hold coboundary simplices for next recursion.
  template <typename T>
  using Container = typename Traits::template Container<T>;

  /// Alias for the recursive call
  using BFS_Up_Node_Next =
      BFS_Up_Node<Visitor, Traits, Complex,
                  std::integral_constant<std::size_t, level + 1>>;

  /**
   * @brief      Visit simplices in the current dimension and continue.
   *
   * @param[in]  v          Visitor functor.
   * @param[in]  F          The simplicial_complex to traverse.
   * @param[in]  begin      Iterator to simplices to traverse.
   * @param[in]  end        Iterator to end of simplices to traverse.
   *
   * @tparam     Iterator   Typename of the iterator.
   */
  template <typename Iterator>
  static void apply(Visitor &&v, Complex &F, Iterator begin, Iterator end) {
    Container<NextSimplexID> next;

    for (auto curr = begin; curr != end; ++curr) {
      if (v.visit(F, *curr)) {
        F.get_cover(*curr, [&](typename Complex::KeyType a) {
          auto id = F.get_simplex_up(*curr, a);
          next.insert(id);
        });
      }
    }

    BFS_Up_Node_Next::apply(std::forward<Visitor>(v), F, next.begin(),
                            next.end());
  }
};

/**
 * @brief      Partial specialization for BFS up helper for facets.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Visitor, typename Traits, typename Complex>
struct BFS_Up_Node<Visitor, Traits, Complex,
                   std::integral_constant<std::size_t, Complex::topLevel>> {
  /// Simplex dimension of facets
  static constexpr auto level = Complex::topLevel;
  /// Typename of the current simplices
  using CurrSimplexID = typename Complex::template SimplexID<level>;

  /**
   * @brief      Visit simplices in the current dimension and continue.
   *
   * @param[in]  v          Visitor functor.
   * @param[in]  F          The simplicial_complex to traverse.
   * @param[in]  begin      Iterator to simplices to traverse.
   * @param[in]  end        Iterator to end of simplices to traverse.
   *
   * @tparam     Iterator   Typename of the iterator.
   */
  template <typename Iterator>
  static void apply(Visitor &&v, Complex &F, Iterator begin, Iterator end) {
    for (auto curr = begin; curr != end; ++curr) {
      v.visit(F, *curr);
    }
  }
};

/**
 * @brief      General template for BFS down helper.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 * @tparam     K        Current simplex dimension to traverse.
 */
template <typename Visitor, typename Traits, typename Complex, typename K>
struct BFS_Down_Node {};

/**
 * @brief      Partial specialization for BFS down helper for non facet
 *             dimensions.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 * @tparam     k        Current simplex dimension to traverse.
 */
template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct BFS_Down_Node<Visitor, Traits, Complex,
                     std::integral_constant<std::size_t, k>> {
  /// Simplex dimension current traversed
  static constexpr auto level = k;
  /// Typename of the current simplices
  using CurrSimplexID = typename Complex::template SimplexID<level>;
  /// Typename of boundary simplices
  using NextSimplexID = typename Complex::template SimplexID<level - 1>;
  /// Container to use to hold boundary simplices for next recursion
  template <typename T>
  using Container = typename Traits::template Container<T>;

  /// Alias for the recursive call
  using BFS_Down_Node_Next =
      BFS_Down_Node<Visitor, Traits, Complex,
                    std::integral_constant<std::size_t, level - 1>>;

  /**
   * @brief      Visit simplices in the current dimension and continue.
   *
   * @param[in]  v          Visitor functor.
   * @param[in]  F          The simplicial_complex to traverse.
   * @param[in]  begin      Iterator to simplices to traverse.
   * @param[in]  end        Iterator to end of simplices to traverse.
   *
   * @tparam     Iterator   Typename of the iterator.
   */
  template <typename Iterator>
  static void apply(Visitor &&v, Complex &F, Iterator begin, Iterator end) {
    Container<NextSimplexID> next;

    for (auto curr = begin; curr != end; ++curr) {
      if (v.visit(F, *curr)) {
        F.get_name(*curr, [&](typename Complex::KeyType a) {
          auto id = F.get_simplex_down(*curr, a);
          next.insert(id);
        });
      }
    }

    BFS_Down_Node_Next::apply(std::forward<Visitor>(v), F, next.begin(),
                              next.end());
  }
};

/**
 * @brief      Partial specialization for BFS down helper for vertices
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Visitor, typename Traits, typename Complex>
struct BFS_Down_Node<Visitor, Traits, Complex,
                     std::integral_constant<std::size_t, 1>> {
  /**
   * @brief      Visit simplices in the current dimension and continue.
   *
   * @param[in]  v          Visitor functor.
   * @param[in]  F          The simplicial_complex to traverse.
   * @param[in]  begin      Iterator to simplices to traverse.
   * @param[in]  end        Iterator to end of simplices to traverse.
   *
   * @tparam     Iterator   Typename of the iterator.
   */
  template <typename Iterator>
  static void apply(Visitor &&v, Complex &F, Iterator begin, Iterator end) {
    for (auto curr = begin; curr != end; ++curr) {
      v.visit(F, *curr);
    }
  }
};

/**
 * @deprecated
 * @brief      Case to catch accidents... calling down on root is bad.
 *
 * @tparam     Visitor  { description }
 * @tparam     Traits   { description }
 * @tparam     Complex  { description }
 */
// template <typename Visitor, typename Traits, typename Complex>
// struct BFS_Down_Node<Visitor, Traits, Complex,
// std::integral_constant<std::size_t,0>>
// {
//     template <typename Iterator>
//     static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
//     {}
// };

/**
 * @brief      General tempalte for BFS traversal across edges.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 * @tparam     K        Current simplex dimension to traverse.
 */
template <typename Visitor, typename Traits, typename Complex, typename K>
struct BFS_Edge {};

/**
 * @brief      Partial specialization for BFS Edge for non facets.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 * @tparam     k        Current simplex dimension to traverse.
 */
template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct BFS_Edge<Visitor, Traits, Complex,
                std::integral_constant<std::size_t, k>> {
  /// Current simplex dimension to traverse
  static constexpr auto level = k;
  /// Typename of the current EdgeID
  using CurrEdgeID = typename Complex::template EdgeID<level>;
  /// Typename of the next EdgeID
  using NextEdgeID = typename Complex::template EdgeID<level + 1>;
  /// Typename of the current SimplexID
  using CurrSimplexID = typename Complex::template SimplexID<level>;
  /// Container to use to hold coboundary edges for next recursion.
  template <typename T>
  using Container = typename Traits::template Container<T>;
  /// Alias for the recursive call
  using BFS_Edge_Next =
      BFS_Edge<Visitor, Traits, Complex,
               std::integral_constant<std::size_t, level + 1>>;

  /**
   * @brief      Visit simplices in the current dimension and continue.
   *
   * @param[in]  v          Visitor functor.
   * @param[in]  F          The simplicial_complex to traverse.
   * @param[in]  begin      Iterator to edges to traverse.
   * @param[in]  end        Iterator to end of edges to traverse.
   *
   * @tparam     Iterator   Typename of the iterator.
   */
  template <typename Iterator>
  static void apply(Visitor &&v, Complex &F, Iterator begin, Iterator end) {
    Container<NextEdgeID> next;
    std::vector<typename Complex::KeyType> cover;

    for (auto curr = begin; curr != end; ++curr) {
      v.visit(F, *curr);

      CurrSimplexID n = curr->up();
      F.get_cover(n, std::back_inserter(cover));
      for (auto a : cover) {
        NextEdgeID id = F.get_edge_up(n, a);
        next.insert(next.end(), id);
      }
      cover.clear();
    }

    BFS_Edge_Next::apply(std::forward<Visitor>(v), F, next.begin(), next.end());
  }
};

/**
 * @brief      Partial specialization for BFS Edge for facets.
 *
 * @tparam     Visitor  Type of visitor functor.
 * @tparam     Traits   Traits of the BFS traversal.
 * @tparam     Complex  Typename of the simplicial_complex.
 */
template <typename Visitor, typename Traits, typename Complex>
struct BFS_Edge<Visitor, Traits, Complex,
                std::integral_constant<std::size_t, Complex::topLevel>> {
  /// Simplex dimension current traversed
  static constexpr auto level = Complex::topLevel;
  /// Typename of the current edge
  using CurrEdgeID = typename Complex::template EdgeID<level>;

  /**
   * @brief      Visit the edges to facets.
   *
   * @param[in]  v          Visitor functor.
   * @param[in]  F          The simplicial_complex to traverse.
   * @param[in]  begin      Iterator to edges to traverse.
   * @param[in]  end        Iterator to end of edges to traverse.
   *
   * @tparam     Iterator   Typename of the iterator.
   */
  template <typename Iterator>
  static void apply(Visitor &&v, Complex &F, Iterator begin, Iterator end) {
    for (auto curr = begin; curr != end; ++curr) {
      v.visit(F, *curr);
    }
  }
};

/// Allow repeat visits of simplices for BFS visits.
struct BFS_Repeat_Node_traits {
  /// Use a vector to allow duplicates
  template <typename T> using Container = std::vector<T>;
};

/// No repeat traits for BFS simplex visitor.
struct BFS_NoRepeat_Node_Traits {
  /// Use a NodeSet to avoid duplicates
  template <typename T> using Container = NodeSet<T>;
};

/// No repeat traits for BFS edge visitor.
struct BFS_NoRepeat_Edge_Traits {
  /// Use a NodeSet to avoid duplicates.
  template <typename T> using Container = NodeSet<T>;
  // template <typename Complex, typename SimplexID> auto node_next(Complex F,
  // SimplexID s);
};
} // End namespace visitor_detail
/// @endcond

/**
 * @brief      Traverse BFS up the complex and apply a visitor function to each
 *             simplex visited.
 *
 * @param[in]  v          Visitor functor to apply.
 * @param      F          The simplicial_complex to traverse.
 * @param[in]  s          The simplex to start at.
 *
 * @tparam     Visitor    Typename of the functor.
 * @tparam     SimplexID  Typename of the simplex.
 */
template <typename Visitor, typename SimplexID>
void visit_BFS_up(Visitor &&v, typename SimplexID::complex &F, SimplexID s) {
  namespace cvd = visitor_detail;
  cvd::BFS_Up_Node<Visitor, cvd::BFS_NoRepeat_Node_Traits,
                   typename SimplexID::complex,
                   std::integral_constant<std::size_t, SimplexID::level>>::
      apply(std::forward<Visitor>(v), F, &s, &s + 1);
}

/**
 * @brief      Traverse BFS down the complex and apply a visitor function to
 * each
 *             simplex visited.
 *
 * @param[in]  v          Visitor functor to apply.
 * @param      F          The simplicial_complex to traverse.
 * @param[in]  s          The simplex to start at.
 *
 * @tparam     Visitor    Typename of the functor.
 * @tparam     SimplexID  Typename of the simplex.
 */
template <typename Visitor, typename SimplexID>
void visit_BFS_down(Visitor &&v, typename SimplexID::complex &F, SimplexID s) {
  namespace cvd = visitor_detail;
  cvd::BFS_Down_Node<Visitor, cvd::BFS_NoRepeat_Node_Traits,
                     typename SimplexID::complex,
                     std::integral_constant<std::size_t, SimplexID::level>>::
      apply(std::forward<Visitor>(v), F, &s, &s + 1);
}

/**
 * @brief      Traverse across edges BFS.
 *
 * @param[in]  v          Visitor functor to apply.
 * @param      F          The simplicial_complex to traverse.
 * @param[in]  s          The edge to start at.
 *
 * @tparam     Visitor    Typename of the functor.
 * @tparam     EdgeID     Typename of the edge.
 */
template <typename Visitor, typename EdgeID>
void edge_up(Visitor &&v, typename EdgeID::complex &F, EdgeID s) {
  namespace cvd = visitor_detail;
  cvd::BFS_Edge<Visitor, cvd::BFS_NoRepeat_Edge_Traits,
                typename EdgeID::complex,
                std::integral_constant<std::size_t, EdgeID::level>>::
      apply(std::forward<Visitor>(v), F, &s, &s + 1);
}

/**
 * @brief      Push the immediate face neighbors into the provided iterator.
 *
 *  This function gets the set of neighbors which share a common face. We
 *  compute this by traversing to all faces of the simplex of interest. Then we
 *  get all cofaces of this set. Depending on the type of iterator passed,
 *  duplicate simplices will be included or excluded. Note that this is the
 *  traditional definition of neighbor. For example, faces which share an edge
 *  are neighbors.
 *
 * @param      F           The simplicial complex
 * @param[in]  nid         Simplex to get neighbors of.
 * @param[in]  iter        The iterator to push members into.
 *
 * @tparam     Complex     Type of the simplicial complex
 * @tparam     level       The integral level of the node
 * @tparam     InsertIter  Typename of the iterator.
 */
template <class Complex, std::size_t level, class InsertIter>
void neighbors(Complex &F, typename Complex::template SimplexID<level> nid,
               InsertIter iter) {
  for (auto a : F.get_name(nid)) {
    auto id = F.get_simplex_down(nid, a);
    for (auto b : F.get_cover(id)) {
      auto nbor = F.get_simplex_up(id, b);
      if (nbor != nid) {
        *iter++ = nbor;
      }
    }
  }
}

/**
 * @brief      This is a helper function to assist neighbors to automatically
 *             deduce the integral level.
 *
 * @param      F           The simplicial complex.
 * @param[in]  nid         Simplex to get neighbors of.
 * @param[in]  iter        The iterator to push members into.
 *
 * @tparam     Complex     Type of the simplicial complex
 * @tparam     level       The integral level of the node
 * @tparam     InsertIter  Typename of the iterator.
 */
template <class Complex, class SimplexID, class InsertIter>
void neighbors(Complex &F, SimplexID nid, InsertIter iter) {
  neighbors<Complex, SimplexID::level, InsertIter>(F, nid, iter);
}

/**
 * @brief      Push the immediate coface neighbors into the provided iterator.
 *
 * @param      F           The simplicial complex.
 * @param[in]  nid         Simplex to get neighbors of.
 * @param[in]  iter        The iterator to push members into.
 *
 * @tparam     Complex     Type of the simplicial complex
 * @tparam     level       The integral level of the node
 * @tparam     InsertIter  Typename of the iterator.
 */
template <class Complex, std::size_t level, class InsertIter>
void neighbors_up(Complex &F, typename Complex::template SimplexID<level> nid,
                  InsertIter iter) {
  for (auto a : F.get_cover(nid)) {
    auto id = F.get_simplex_up(nid, a);
    for (auto b : F.get_name(id)) {
      auto nbor = F.get_simplex_down(id, b);
      if (nbor != nid) {
        *iter++ = nbor;
      }
    }
  }
}

/**
 * @brief      This is a helper function to assist neighbors to automatically
 *             deduce the integral level.
 *
 * @param      F           The simplicial complex.
 * @param[in]  nid         Simplex to get neighbors of.
 * @param[in]  iter        The iterator to push members into.
 *
 * @tparam     Complex     Type of the simplicial complex
 * @tparam     level       The integral level of the node
 * @tparam     InsertIter  Typename of the iterator.
 */
template <class Complex, class SimplexID, class InsertIter>
void neighbors_up(Complex &F, SimplexID nid, InsertIter iter) {
  neighbors_up<Complex, SimplexID::level, InsertIter>(F, nid, iter);
}

/**
 * @brief      Code for returning a set of k-ring neighbors.
 *
 * @param[in]  F         The simplicial_complex to traverse.
 * @param[in]  ring      The number of rings of neighbors to collect.
 * @param[out] nbors     Set of previously seen simplices.
 * @param[in]  begin     The begin
 * @param[in]  end       The end
 *
 * @tparam     Complex   Typename of the simplicial_complex.
 * @tparam     level     Simplex dimension of the simplex and neighbors.
 * @tparam     Iterator  { description }
 */
template <class Complex, std::size_t level, typename Iterator>
void kneighbors_up(Complex &F, int ring,
                   std::set<typename Complex::template SimplexID<level>> &nbors,
                   Iterator begin, Iterator end) {
  if (ring == 0) {
    return;
  }
  std::set<typename Complex::template SimplexID<level>> next;
  for (auto nid = begin; nid != end; ++nid) {
    for (auto a : F.get_cover(*nid)) {
      auto id = F.get_simplex_up(*nid, a);
      for (auto b : F.get_name(id)) {
        auto nbor = F.get_simplex_down(id, b);
        if (nbors.insert(nbor).second) {
          next.insert(nbor);
        }
      }
    }
  }
  return kneighbors_up<Complex, level>(F, ring - 1, nbors, next.begin(),
                                       next.end());
}

/**
 * @brief      Helper function to help kneighbors_up to deduce the integral
 *             level of SimplexID.
 *
 * @param[in]  F           The simplicial complex
 * @param[in]  nid         Simplex of interest to get the nieghbors of.
 * @param[in]  ring        The number of rings to include as a neighbor.
 * @param[out] nbors       Set of neighbors to populate.
 *
 * @tparam     Complex     Typename of the complex.
 * @tparam     SimplexID   Typename of the SimplexID.
 */
template <class Complex, class SimplexID>
void kneighbors_up(Complex &F, SimplexID nid, int ring,
                   std::set<SimplexID> &nbors) {
  nbors.insert(nid);
  std::set<SimplexID> next{nid};
  kneighbors_up<Complex, SimplexID::level>(F, ring, nbors, next.begin(),
                                           next.end());
  nbors.erase(nid);
}

/**
 * @brief      Code for returning a set of k-ring neighbors.
 *
 * @param[in]  F         The simplicial_complex to traverse.
 * @param[in]  ring      The number of rings of neighbors to collect.
 * @param[out] nbors     Set of previously seen simplices.
 * @param[in]  begin     The begin
 * @param[in]  end       The end
 *
 * @tparam     Complex   Typename of the simplicial_complex.
 * @tparam     level     Simplex dimension of the simplex and neighbors.
 * @tparam     Iterator  { description }
 */
template <class Complex, std::size_t level, typename Iterator>
void kneighbors(Complex &F, int ring,
                std::set<typename Complex::template SimplexID<level>> &nbors,
                Iterator begin, Iterator end) {
  if (ring == 0) {
    return;
  }
  std::set<typename Complex::template SimplexID<level>> next;
  for (auto nid = begin; nid != end; ++nid) {
    for (auto a : F.get_name(*nid)) {
      auto id = F.get_simplex_down(*nid, a);
      for (auto b : F.get_cover(id)) {
        auto nbor = F.get_simplex_up(id, b);
        if (nbors.insert(nbor).second) {
          next.insert(nbor);
        }
      }
    }
  }
  return kneighbors_up<Complex, level>(F, ring - 1, nbors, next.begin(),
                                       next.end());
}

/**
 * @brief      Helper function to help kneighbors to deduce the integral
 *             level of SimplexID.
 *
 * @param[in]  F           The simplicial complex
 * @param[in]  nid         Simplex of interest to get the nieghbors of.
 * @param[in]  ring        The number of rings to include as a neighbor.
 * @param[out] nbors       Set of neighbors to populate.
 *
 * @tparam     Complex     Typename of the complex.
 * @tparam     SimplexID   Typename of the SimplexID.
 */
template <class Complex, class SimplexID>
void kneighbors(Complex &F, SimplexID nid, int ring,
                std::set<SimplexID> &nbors) {
  nbors.insert(nid);
  std::set<SimplexID> next{nid};
  kneighbors<Complex, SimplexID::level>(F, ring, nbors, next.begin(),
                                        next.end());
  nbors.erase(nid);
}

} // End namespace casc

// namespace visitor_detail
// {
// template <typename Visitor, typename Complex, std::size_t k, std::size_t
// ring>
// struct Neighbors_Up_Node
// {
//     static constexpr auto level = k;
//     using SimplexID = typename Complex::template SimplexID<level>;

//     using Neighbors_Up_Node_Next =
// Neighbors_Up_Node<Visitor,Complex,level,ring-1>;

//     template <typename Iterator>
//     static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes,
// Iterator begin, Iterator end)
//     {
//         NodeSet<SimplexID> next;

//         for(auto curr = begin; curr != end; ++curr)
//         {
//             if(v.visit(F, *curr))
//             {
//                 for(auto a : F.get_cover(*curr))
//                 {
//                     auto id = F.get_simplex_up(*curr,a);
//                     for(auto b : F.get_name(id))
//                     {
//                         auto nbor = F.get_simplex_down(id,b);
//                         if(nodes.insert(nbor).second)
//                         {
//                             next.insert(nbor);
//                         }
//                     }
//                 }
//             }
//         }

//         Neighbors_Up_Node_Next::apply(std::forward<Visitor>(v), F, nodes,
// next.begin(), next.end());
//     }
// };

// template <typename Visitor, typename Complex, std::size_t k>
// struct Neighbors_Up_Node<Visitor, Complex, k, 0>
// {
//     static constexpr auto level = k;
//     using SimplexID = typename Complex::template SimplexID<level>;

//     template <typename Iterator>
//     static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes,
// Iterator begin, Iterator end)
//     {
//         for(auto curr = begin; curr != end; ++curr)
//         {
//             v.visit(F, *curr);
//         }
//     }
// };

// template <typename Visitor, typename Complex, std::size_t k, std::size_t
// ring>
// struct Neighbors_Down_Node
// {
//     static constexpr auto level = k;
//     using SimplexID = typename Complex::template SimplexID<level>;

//     using Neighbors_Down_Node_Next = Neighbors_Down_Node<Visitor,Complex,
//             level,ring-1>;

//     template <typename Iterator>
//     static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes,
// Iterator begin, Iterator end)
//     {
//         NodeSet<SimplexID> next;

//         for(auto curr = begin; curr != end; ++curr)
//         {
//             if(v.visit(F, *curr))
//             {
//                 for(auto a : F.get_name(*curr))
//                 {
//                     auto id = F.get_simplex_down(*curr,a);
//                     for(auto b : F.get_cover(id))
//                     {
//                         auto nbor = F.get_simplex_up(id,b);
//                         if(nodes.insert(nbor).second)
//                         {
//                             next.insert(nbor);
//                         }
//                     }
//                 }
//             }
//         }

//         Neighbors_Down_Node_Next::apply(std::forward<Visitor>(v), F, nodes,
// next.begin(), next.end());
//     }
// };

// template <typename Visitor, typename Complex, std::size_t k>
// struct Neighbors_Down_Node<Visitor, Complex, k, 0>
// {
//     static constexpr auto level = k;
//     using SimplexID = typename Complex::template SimplexID<level>;

//     template <typename Iterator>
//     static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes,
// Iterator begin, Iterator end)
//     {
//         for(auto curr = begin; curr != end; ++curr)
//         {
//             v.visit(F, *curr);
//         }
//     }
// };
// }

// template <std::size_t rings, typename Visitor, typename SimplexID>
// void visit_neighbors_up(Visitor&& v, typename SimplexID::complex& F,
// SimplexID s)
// {
//     NodeSet<SimplexID> nodes{s};
//     namespace cvd = visitor_detail;
//     cvd::Neighbors_Up_Node<Visitor,typename
// SimplexID::complex,SimplexID::level,rings>::apply(
//             std::forward<Visitor>(v),F,nodes,&s,&s+1);
// }

// template <std::size_t rings, typename Visitor, typename SimplexID>
// void visit_neighbors_down(Visitor&& v, typename SimplexID::complex& F,
// SimplexID s)
// {
//     NodeSet<SimplexID> nodes{s};
//     namespace cvd = visitor_detail;
//     cvd::Neighbors_Down_Node<Visitor,typename
// SimplexID::complex,SimplexID::level,rings>::apply(
//             std::forward<Visitor>(v),F,nodes,&s,&s+1);
// }
