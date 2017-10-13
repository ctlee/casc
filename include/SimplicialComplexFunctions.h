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

#pragma once

#include <iostream>
#include <fstream>
#include <ostream>
#include "SimplexSet.h"
#include "SimplicialComplex.h"
#include "SimplicialComplexVisitors.h"
#include "stringutil.h"

namespace casc
{

/// Namespace for templated helpers of convenience functions
namespace casc_func_detail
{
    template <typename Complex>
    struct SimplexAggregator
    {
        using SimplexSet = typename casc::SimplexSet<Complex>;

        SimplexAggregator(SimplexSet* p) : pLevels(p) {}

        template <std::size_t k>
        bool visit(Complex& F, typename Complex::template SimplexID<k> s)
        {
            // If the simplex isn't there, insert it.
            if(pLevels->find(s) == pLevels->template end<k>())
            {
                pLevels->insert(s);
                return true;
            }
            else{
                // Everything after has been found already
                return false;
            }
        }
    private:
        SimplexSet* pLevels;
    };

    template <typename Complex>
    struct StarHelper
    {
        template <std::size_t k>
        static void apply(Complex& F, 
                casc::SimplexSet<Complex>& S,
                casc::SimplexSet<Complex>& dest){
            auto s = casc::get<k>(S);
            for(auto simplex : s){
                visit_node_up(SimplexAggregator<Complex>(&dest), F, simplex);
            }
        }
    };

    template <typename Complex>
    struct ClosureHelper
    {
        template <std::size_t k>
        static void apply(Complex& F, 
                casc::SimplexSet<Complex>& S,
                casc::SimplexSet<Complex>& dest){
            auto s = casc::get<k>(S);
            for(auto simplex : s){
                visit_node_down(SimplexAggregator<Complex>(&dest), F, simplex);
            }
        }
    };

    /**
     * @brief      Visitor for printing connectivity of the ASC
     *
     * @tparam     Complex  Type details of the ASC
     */
    template <typename Complex>
    struct GraphVisitor
    {
        std::ostream& fout;
        GraphVisitor(std::ostream& os) : fout(os) {}

        template <std::size_t level> 
        bool visit(const Complex& F, typename Complex::template SimplexID<level> s)
        {
            auto name = cascstringutil::to_string(F.get_name(s));

            auto covers = F.get_cover(s);
            for(auto cover : covers){
                auto edge = F.get_edge_up(s, cover);
                auto nextName = cascstringutil::to_string(F.get_name(edge.up()));
                if((*edge).orientation == 1){
                    fout    << "   \"" << name << "\" -> \"" 
                            << nextName << "\"" << std::endl;
                }
                else{
                    fout    << "   \"" << nextName << "\" -> \"" 
                            << name << "\"" <<  std::endl;
                }
            }
            return true;
        }

        bool visit(const Complex& F, typename Complex::template SimplexID<Complex::topLevel-1> s){

            auto name = cascstringutil::to_string(F.get_name(s));
            auto covers = F.get_cover(s);
            for(auto cover : covers){
                auto edge = F.get_edge_up(s, cover);
                auto nextName = cascstringutil::to_string(F.get_name(edge.up()));
                auto orient = (*edge.up()).orientation;
                if (orient == 1){
                    nextName = "+ " + nextName;
                }
                else{
                    nextName = "- " + nextName;
                }
                if((*edge).orientation == 1){
                    fout    << "   \"" << name << "\" -> \"" 
                            << nextName << "\"" << std::endl;
                }
                else{
                    fout    << "   \"" << nextName << "\" -> \"" 
                            << name << "\"" << std::endl;
                }
            }
            return true;
        }

        void visit(const Complex& F, typename Complex::template SimplexID<Complex::topLevel> s){}
    };

    template <typename Complex, typename K>
    struct DotHelper {};

    /**
     * @brief      List the names of simplices at each level
     *
     * @tparam     Complex  Type of the ASC
     * @tparam     k        Level to traverse
     */
    template <typename Complex, std::size_t k>
    struct DotHelper<Complex, std::integral_constant<std::size_t, k>>
    {
        static void printlevel(std::ofstream& fout, const Complex& F){
            auto nodes = F.template get_level_id<k>();
            fout    << "subgraph cluster_" << k << " {\n"
                    << "label=\"Level " << k << "\"\n";
            for (auto node : nodes){
                fout << "\"" << cascstringutil::to_string(F.get_name(node)) << "\";";
            }
            fout    << "\n}\n";
            DotHelper<Complex, std::integral_constant<std::size_t,k+1>>::printlevel(fout, F);
        }
    };

    /**
     * @brief      List the names of simplices at the top level
     *
     * @tparam     Complex  Type of the ASC
     */
    template <typename Complex>
    struct DotHelper<Complex, std::integral_constant<std::size_t, Complex::topLevel>>
    {
        static void printlevel(std::ofstream& fout, const Complex& F){
            auto nodes = F.template get_level_id<Complex::topLevel>();
            fout    << "subgraph cluster_" << Complex::topLevel << " {\n"
                    << "label=\"Level " << Complex::topLevel << "\"\n";
            for (auto node : nodes){
                auto orient = (*node).orientation;
                if (orient == 1){
                    fout << "\"+ ";
                }
                else{
                    fout << "\"- ";
                }
                fout << cascstringutil::to_string(F.get_name(node)) << "\";";
            }
            fout    << "\n}\n";
        }
    };
} // end namespace casc_func_detail

/**
 * @brief      Gets the star of a SimplexSet
 *
 * @param      F        { parameter_description }
 * @param      S        { parameter_description }
 * @param      dest     The destination
 *
 * @tparam     Complex  { description }
 */
template <typename Complex>
void getStar(Complex& F, casc::SimplexSet<Complex>& S,
        casc::SimplexSet<Complex>& dest){
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using RevIndex = typename SimplexSet::cRevIndex;

    // Start at the top and work up. We can assume that if we've seen it then
    // everything after has been added.
    util::int_for_each<std::size_t, RevIndex>(
            casc_func_detail::StarHelper<Complex>(), F, S, dest);
}

/**
 * @brief      Gets the Star of simplex s
 *
 * @param      F        { parameter_description }
 * @param      s        { parameter_description }
 * @param      dest     The destination
 *
 * @tparam     Complex  { description }
 * @tparam     Simplex  { description }
 */
template <typename Complex, typename Simplex>
void getStar(Complex& F, Simplex& s, casc::SimplexSet<Complex>& dest){
    visit_node_up(casc_func_detail::SimplexAggregator<Complex>(&dest), F, s);
}

template <typename Complex>
void getClosure(Complex& F, casc::SimplexSet<Complex>& S,
        casc::SimplexSet<Complex>& dest){
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using LevelIndex = typename SimplexSet::cLevelIndex;
    // Start at the bottom and work down.
    // We can assume that everything below has been looked at.
    util::int_for_each<std::size_t, LevelIndex>(
            casc_func_detail::ClosureHelper<Complex>(), F, S, dest);
}

/**
 * @brief      Gets the Closure of simplex s
 *
 * @param      F        { parameter_description }
 * @param      s        { parameter_description }
 * @param      dest     The destination
 *
 * @tparam     Complex  { description }
 * @tparam     Simplex  { description }
 */
template <typename Complex, typename Simplex>
void getClosure(Complex& F, Simplex& s, casc::SimplexSet<Complex>& dest){
    visit_node_down(casc_func_detail::SimplexAggregator<Complex>(&dest), F, s);
}

template <typename Complex>
void getLink(Complex& F, casc::SimplexSet<Complex>& S,
        casc::SimplexSet<Complex>& dest){
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using LevelIndex = typename SimplexSet::cLevelIndex;
    using RevIndex = typename SimplexSet::cRevIndex;

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

template <typename Complex, typename Simplex>
void getLink(Complex& F, Simplex& s, casc::SimplexSet<Complex>& dest){
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

// TODO: Put back the const F (0)
/**
 * @brief      Writes out the topology of an ASC into the dot format. The resulting dot file can be rendered into an image using tools such as GraphViz. dot -Tpng file.dot > image.png.
 *             dot -Tpng input.dot > output.png
 *
 * @param[in]  filename  The filename
 * @param[in]  F         ASC of interest
 *
 * @tparam     Complex   Type of the ASC
 */
template <typename Complex>
void writeDOT(const std::string& filename, Complex& F){
	std::ofstream fout(filename);
	if(!fout.is_open())
	{
	    std::cerr   << "File '" << filename 
	                << "' could not be writen to." << std::endl;
        fout.close();
	    exit(1); 
	}

	fout 	<< "digraph {\n"
            << "node [shape = record,height = .1]"
            << "splines=line;\n"
			<< "dpi=300;\n";
    auto v = casc_func_detail::GraphVisitor<Complex>(fout);
	visit_node_up(v, F, F.get_simplex_up());

    // List the simplices
    casc_detail::DotHelper<Complex, 
            std::integral_constant<std::size_t, 0>>::printlevel(fout, F);
	fout << "}\n";
    fout.close();
}
} // end namespace casc