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
 * @file CASCFunctions.h
 * @brief Contains various functions that operate on simplicial complexes
 */

#pragma once

#include "CASCTraversals.h"
#include "SimplexSet.h"
#include "SimplicialComplex.h"
#include "stringutil.h"
#include <fstream>
#include <iostream>

namespace casc {
/// @cond detail
/// Namespace for templated helpers of convenience functions
namespace func_detail {

/**
 * @brief      Visitor for grabbing simplices in a BFS traversal. See also
 *             visit_BFS_up and visit_BFS_down.
 *
 * @tparam     Complex  Typename of the complex.
 */
template <typename Complex> struct SimplexAggregator {
  /// Alias for a typed SimplexSet
  using SimplexSet = typename casc::SimplexSet<Complex>;

  /**
   * @brief      Constructor of the aggregator.
   *
   * @param      p     Pointer to SimplexSet to fill.
   */
  SimplexAggregator(SimplexSet *p) : pLevels(p) {}

  /**
   * @brief      Overloaded visit function to append different dimensioned
   *             SimplexIDs into the SimplexSet.
   *
   * @param      F          The complex of interest
   * @param[in]  s          SimplexID to visit.
   *
   * @tparam     k          Dimension of the simplex.
   *
   * @return     True if the traversal should continue.
   */
  template <std::size_t k>
  bool visit(Complex &, typename Complex::template SimplexID<k> s) {
    // If the simplex isn't there, insert it.
    if (pLevels->find(s) == pLevels->template end<k>()) {
      pLevels->insert(s);
      return true;
    } else {
      // Everything after has been found already
      return false;
    }
  }

private:
  SimplexSet *pLevels;
};

/**
 * @brief      Helper for computing the star of a set of simplices.
 *
 * @tparam     Complex  Typename of the simplicial complex.
 */
template <typename Complex> struct StarHelper {
  /**
   * @brief      Iterate over the SimplexSet and compute the star.
   *
   * @param      F     Complex of interest.
   * @param      S     SimplexSet of simplices to compute the star of.
   * @param      dest  SimplexSet where the star should go.
   *
   * @tparam     k     Dimension of the current simplex dimension to traverse.
   */
  template <std::size_t k>
  static void apply(Complex &F, casc::SimplexSet<Complex> &S,
                    casc::SimplexSet<Complex> &dest) {
    auto s = casc::get<k>(S);
    for (auto simplex : s) {
      visit_BFS_up(SimplexAggregator<Complex>(&dest), F, simplex);
    }
  }
};

/**
 * @brief      Helper for computing the closure of a set of simplices.
 *
 * @tparam     Complex  Typename of the simplicial complex.
 */
template <typename Complex> struct ClosureHelper {
  /**
   * @brief      Iterate over the SimplexSet and compute the closure
   *
   * @param      F     Complex of interest.
   * @param      S     SimplexSet of simplices to compute the closure of.
   * @param      dest  SimplexSet where the closure should go.
   *
   * @tparam     k     Dimension of the current simplex dimension to traverse.
   */
  template <std::size_t k>
  static void apply(Complex &F, casc::SimplexSet<Complex> &S,
                    casc::SimplexSet<Complex> &dest) {
    auto s = casc::get<k>(S);
    for (auto simplex : s) {
      visit_BFS_down(SimplexAggregator<Complex>(&dest), F, simplex);
    }
  }
};

/**
 * @brief      Visitor for printing connectivity of the simplicial complex.
 *
 * @tparam     Complex  Typename of the simplicial complex.
 */
template <typename Complex> struct GraphVisitor {
  /// ostream to write out to.
  std::ostream &fout;

  /**
   * @brief      Constructor
   *
   * @param      os    Ostream to print to.
   */
  GraphVisitor(std::ostream &os) : fout(os) {}

  /**
   * @brief      Generic visitor prints the simplices and edge connectivity.
   *
   * @param[in]  F          Complex of interest.
   * @param[in]  s          Simplex to visit.
   *
   * @tparam     level      Dimension of the simplex.
   *
   * @return     True
   */
  template <std::size_t level>
  bool visit(const Complex &F, typename Complex::template SimplexID<level> s) {
    auto name = to_string(F.get_name(s));

    auto covers = F.get_cover(s);
    for (auto cover : covers) {
      auto edge = F.get_edge_up(s, cover);
      auto nextName = to_string(F.get_name(edge.up()));
      if ((*edge).orientation == 1) {
        fout << "   \"" << name << "\" -> \"" << nextName << "\"" << std::endl;
      } else {
        fout << "   \"" << nextName << "\" -> \"" << name << "\"" << std::endl;
      }
    }
    return true;
  }

  /**
   * @brief      Explicit specialization for visiting the second to top level
   *             simplices.
   *
   * @param[in]  F          Complex of interest
   * @param[in]  s          Simplex to visit.
   *
   * @return     True;
   */
  bool visit(const Complex &F,
             typename Complex::template SimplexID<Complex::topLevel - 1> s) {

    auto name = to_string(F.get_name(s));
    auto covers = F.get_cover(s);
    for (auto cover : covers) {
      auto edge = F.get_edge_up(s, cover);
      auto nextName = to_string(F.get_name(edge.up()));
      auto orient = (*edge.up()).orientation;
      if (orient == 1) {
        nextName = "+ " + nextName;
      } else {
        nextName = "- " + nextName;
      }
      if ((*edge).orientation == 1) {
        fout << "   \"" << name << "\" -> \"" << nextName << "\"" << std::endl;
      } else {
        fout << "   \"" << nextName << "\" -> \"" << name << "\"" << std::endl;
      }
    }
    return true;
  }

  /**
   * @brief      Explicit specialization for visiting the facets of the
   *complex.
   *
   * @param[in]  F          Complex of interest.
   * @param[in]  s          Simplex to visit.
   */
  void visit(const Complex &,
             typename Complex::template SimplexID<Complex::topLevel>) {}
};

/**
 * @brief      Generic template for printing out DOT meta info.
 *
 * @tparam     Complex  Typename of the complex.
 * @tparam     K        Dimension to go through.
 */
template <typename Complex, typename K> struct DotHelper {};

/**
 * @brief      Partial specialization for listing names of simplices.
 *
 * @tparam     Complex  Typename of the complex.
 * @tparam     k        Simplex dimension to traverse.
 */
template <typename Complex, std::size_t k>
struct DotHelper<Complex, std::integral_constant<std::size_t, k>> {
  /**
   * @brief      Print out a list of simplices in a simplex dimension.
   *
   * @param      fout  Stream to print to.
   * @param[in]  F     Complex of interest.
   */
  static void printlevel(std::ofstream &fout, const Complex &F) {
    auto nodes = F.template get_level_id<k>();
    fout << "subgraph cluster_" << k << " {\n"
         << "label=\"Level " << k << "\"\n";
    for (auto node : nodes) {
      fout << "\"" << to_string(F.get_name(node)) << "\";";
    }
    fout << "\n}\n";
    DotHelper<Complex, std::integral_constant<std::size_t, k + 1>>::printlevel(
        fout, F);
  }
};

/**
 * @brief      List the names of simplices at the top level
 *
 * @tparam     Complex  Typename of the complex.
 */
template <typename Complex>
struct DotHelper<Complex,
                 std::integral_constant<std::size_t, Complex::topLevel>> {
  /**
   * @brief      Print out a list of facets of the complex.
   *
   * @param      fout  Stream to print to.
   * @param[in]  F     Complex of interest.
   */
  static void printlevel(std::ofstream &fout, const Complex &F) {
    auto nodes = F.template get_level_id<Complex::topLevel>();
    fout << "subgraph cluster_" << Complex::topLevel << " {\n"
         << "label=\"Level " << Complex::topLevel << "\"\n";
    for (auto node : nodes) {
      auto orient = (*node).orientation;
      if (orient == 1) {
        fout << "\"+ ";
      } else {
        fout << "\"- ";
      }
      fout << to_string(F.get_name(node)) << "\";";
    }
    fout << "\n}\n";
  }
};
} // end namespace func_detail
/// @endcond

/**
 * @brief      Gets the star of a SimplexSet.
 *
 * @param[in]  F        Complex of interest.
 * @param[in]  S        SimplexSet to compute the star of.
 * @param[out] dest     Destination SimplexSet.
 *
 * @tparam     Complex  Typename of the complex.
 */
template <typename Complex>
void getStar(Complex &F, casc::SimplexSet<Complex> &S,
             casc::SimplexSet<Complex> &dest) {
  using SimplexSet = typename casc::SimplexSet<Complex>;
  using RevIndex = typename SimplexSet::cRevIndex;

  // Start at the top and work up. We can assume that if we've seen it then
  // everything after has been added.
  util::int_for_each<std::size_t, RevIndex>(func_detail::StarHelper<Complex>(),
                                            F, S, dest);
}

/**
 * @brief      Gets the star of a simplex.
 *
 * @param[in]  F        Complex of interest.
 * @param      s        Simplex to get the star of.
 * @param[out] dest     Destination SimplexSet.
 *
 * @tparam     Complex  Typename of the complex.
 * @tparam     Simplex  Typename of the simplex.
 */
template <typename Complex, typename Simplex>
void getStar(Complex &F, Simplex &s, casc::SimplexSet<Complex> &dest) {
  visit_BFS_up(func_detail::SimplexAggregator<Complex>(&dest), F, s);
}

/**
 * @brief      Gets the closure of a simplex set.
 *
 * @param[in]  F        Complex of interest.
 * @param[in]  S        SimplexSet to compute the closure of.
 * @param[out] dest     Destination SimplexSet
 *
 * @tparam     Complex  Typename of the complex.
 */
template <typename Complex>
void getClosure(Complex &F, casc::SimplexSet<Complex> &S,
                casc::SimplexSet<Complex> &dest) {
  using SimplexSet = typename casc::SimplexSet<Complex>;
  using LevelIndex = typename SimplexSet::cLevelIndex;
  // Start at the bottom and work down.
  // We can assume that everything below has been looked at.
  util::int_for_each<std::size_t, LevelIndex>(
      func_detail::ClosureHelper<Complex>(), F, S, dest);
}

/**
 * @brief      Compute the closure of a simplex.
 *
 * @param[in]  F        Complex of interest.
 * @param[in]  s        Simplex of interest.
 * @param[out] dest     Destination SimplexSet.
 *
 * @tparam     Complex  Typename of the complex.
 * @tparam     Simplex  Typename of the simplex.
 */
template <typename Complex, typename Simplex>
void getClosure(Complex &F, Simplex &s, casc::SimplexSet<Complex> &dest) {
  visit_BFS_down(func_detail::SimplexAggregator<Complex>(&dest), F, s);
}

/**
 * @brief      Gets the link of a SimplexSet.
 *
 * @param[in]  F        Complex of interest.
 * @param[in]  S        SimplexSet to get the link of.
 * @param[out] dest     Destination SimplexSet.
 *
 * @tparam     Complex  Typename of the complex.
 */
template <typename Complex>
void getLink(Complex &F, casc::SimplexSet<Complex> &S,
             casc::SimplexSet<Complex> &dest) {
  using SimplexSet = typename casc::SimplexSet<Complex>;

  SimplexSet star;
  SimplexSet closure;
  SimplexSet closeStar;
  SimplexSet starClose;
  getStar(F, S, star);
  getClosure(F, star, closeStar);

  getClosure(F, S, closure);
  getStar(F, closure, starClose);
  casc::set_difference(closeStar, starClose, dest);
}

/**
 * @brief      Gets the link of a simplex
 *
 * @param      F        Complex of interest.
 * @param      s        Simplex of interest.
 * @param      dest     Destination SimplexSet.
 *
 * @tparam     Complex  Typename of the complex.
 * @tparam     Simplex  Typename of the simplex.
 */
template <typename Complex, typename Simplex>
void getLink(Complex &F, Simplex &s, casc::SimplexSet<Complex> &dest) {
  using SimplexSet = typename casc::SimplexSet<Complex>;
  SimplexSet star;
  SimplexSet closure;
  SimplexSet closeStar;
  SimplexSet starClose;
  getStar(F, s, star);
  getClosure(F, star, closeStar);

  getClosure(F, s, closure);
  getStar(F, closure, starClose);
  casc::set_difference(closeStar, starClose, dest);
}

/**
 * @brief      Writes out the topology of an ASC into the dot format.
 *
 * The resulting dot file can be rendered into an image using tools such as
 * GraphViz.
 * ~~~~~~~~~~~~~~~~~~~~~~~~{.sh}
 * dot -Tpng input.dot > output.png
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @param[in]  filename  Filename to write out to.
 * @param[in]  F         Simplicial complex to generate the DOT of.
 *
 * @tparam     Complex   Typename of the simplicial complex.
 */
template <typename Complex>
void writeDOT(const std::string &filename, Complex &F) {
  // TODO: Put back the const F (0)
  std::ofstream fout(filename);
  if (!fout.is_open()) {
    std::cerr << "File '" << filename << "' could not be writen to."
              << std::endl;
    fout.close();
    exit(1);
  }

  fout << "digraph {\n"
       << "node [shape = record,height = .1]"
       << "splines=line;\n"
       << "dpi=300;\n";
  auto v = func_detail::GraphVisitor<Complex>(fout);
  visit_BFS_up(v, F, F.get_simplex_up());

  // List the simplices
  func_detail::DotHelper<
      Complex, std::integral_constant<std::size_t, 0>>::printlevel(fout, F);
  fout << "}\n";
  fout.close();
}
} // end namespace casc
