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
 * @file SimplicialComplexVisitors.h
 * @brief Implementations of breadth-first visitor design pattern for easy
 *        traversal of CASC.
 */

#pragma once

#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include "SimplicialComplex.h"

namespace casc
{

/// Visitor design pattern helper templates
namespace casc_visitor_detail
{
template <typename Visitor, typename Traits, typename Complex, typename K>
struct BFS_Up_Node {};

template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct BFS_Up_Node<Visitor, Traits, Complex, std::integral_constant<std::size_t,k>>
{
    static constexpr auto level = k;
    using CurrSimplexID = typename Complex::template SimplexID<level>;
    using NextSimplexID = typename Complex::template SimplexID<level+1>;
    template <typename T> using Container = typename Traits::template Container<T>;

    using BFS_Up_Node_Next = BFS_Up_Node<Visitor,Traits,Complex,std::integral_constant<std::size_t,level+1>>;

    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
    {
        Container<NextSimplexID> next;

        for(auto curr = begin; curr != end; ++curr)
        {
            if(v.visit(F, *curr))
            {
                F.get_cover(*curr, [&](typename Complex::KeyType a)
                {
                    auto id = F.get_simplex_up(*curr,a);
                    next.insert(id);
                });
            }
        }

        BFS_Up_Node_Next::apply(std::forward<Visitor>(v), F, next.begin(), next.end());
    }
};

template <typename Visitor, typename Traits, typename Complex>
struct BFS_Up_Node<Visitor, Traits, Complex, std::integral_constant<std::size_t, Complex::topLevel>>
{
    static constexpr auto level = Complex::topLevel;
    using CurrSimplexID = typename Complex::template SimplexID<level>;

    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
    {
        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);
        }
    }
};



template <typename Visitor, typename Traits, typename Complex, typename K>
struct BFS_Down_Node {};

template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct BFS_Down_Node<Visitor, Traits, Complex, std::integral_constant<std::size_t,k>>
{
    static constexpr auto level = k;
    using CurrSimplexID = typename Complex::template SimplexID<level>;
    using NextSimplexID = typename Complex::template SimplexID<level-1>;
    template <typename T> using Container = typename Traits::template Container<T>;

    using BFS_Down_Node_Next = BFS_Down_Node<Visitor,Traits,Complex,std::integral_constant<std::size_t,level-1>>;

    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
    {
        Container<NextSimplexID> next;

        for(auto curr = begin; curr != end; ++curr)
        {
            if(v.visit(F, *curr)){
                F.get_name(*curr, [&](typename Complex::KeyType a)
                {
                    auto id = F.get_simplex_down(*curr,a);
                    next.insert(id);
                });
            }
        }

        BFS_Down_Node_Next::apply(std::forward<Visitor>(v), F, next.begin(), next.end());
    }
};

template <typename Visitor, typename Traits, typename Complex>
struct BFS_Down_Node<Visitor, Traits, Complex, std::integral_constant<std::size_t,1>>
{
    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
    {
        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);
        }
    }
};

/**
 * @brief      Case to catch accidents... calling down on root is bad.
 *
 * @tparam     Visitor  { description }
 * @tparam     Traits   { description }
 * @tparam     Complex  { description }
 */
// template <typename Visitor, typename Traits, typename Complex>
// struct BFS_Down_Node<Visitor, Traits, Complex, std::integral_constant<std::size_t,0>>
// {
//     template <typename Iterator>
//     static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
//     {}
// };






template <typename Visitor, typename Traits, typename Complex, typename K>
struct BFS_Edge {};

template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct BFS_Edge<Visitor, Traits, Complex, std::integral_constant<std::size_t,k>>
{
    static constexpr auto level = k;
    using CurrEdgeID = typename Complex::template EdgeID<level>;
    using NextEdgeID = typename Complex::template EdgeID<level+1>;
    using CurrSimplexID = typename Complex::template SimplexID<level>;
    template <typename T> using Container = typename Traits::template Container<T>;
    using BFS_Edge_Next = BFS_Edge<Visitor,Traits,Complex,std::integral_constant<std::size_t,level+1>>;


    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
    {
        Container<NextEdgeID> next;
        std::vector<typename Complex::KeyType> cover;

        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);

            CurrSimplexID n = curr->up();
            F.get_cover(n, std::back_inserter(cover));
            for(auto a : cover)
            {
                NextEdgeID id = F.get_edge_up(n,a);
                next.insert(next.end(), id);
            }
            cover.clear();
        }

        BFS_Edge_Next::apply(std::forward<Visitor>(v), F, next.begin(), next.end());
    }
};

template <typename Visitor, typename Traits, typename Complex>
struct BFS_Edge<Visitor, Traits, Complex, std::integral_constant<std::size_t, Complex::topLevel>>
{
    static constexpr auto level = Complex::topLevel;
    using CurrEdgeID = typename Complex::template EdgeID<level>;

    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, Iterator begin, Iterator end)
    {
        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);
        }
    }
};

template <typename Visitor, typename Complex, std::size_t k, std::size_t ring>
struct Neighbors_Up_Node
{
    static constexpr auto level = k;
    using SimplexID = typename Complex::template SimplexID<level>;

    using Neighbors_Up_Node_Next = Neighbors_Up_Node<Visitor,Complex,level,ring-1>;

    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes, Iterator begin, Iterator end)
    {
        NodeSet<SimplexID> next;

        for(auto curr = begin; curr != end; ++curr)
        {
            if(v.visit(F, *curr))
            {
                for(auto a : F.get_cover(*curr))
                {
                    auto id = F.get_simplex_up(*curr,a);
                    for(auto b : F.get_name(id)) 
                    {
                        auto nbor = F.get_simplex_down(id,b);
                        if(nodes.insert(nbor).second)
                        {
                            next.insert(nbor);
                        }
                    }
                }
            }
        }

        Neighbors_Up_Node_Next::apply(std::forward<Visitor>(v), F, nodes, next.begin(), next.end());
    }
};

template <typename Visitor, typename Complex, std::size_t k>
struct Neighbors_Up_Node<Visitor, Complex, k, 0>
{
    static constexpr auto level = k;
    using SimplexID = typename Complex::template SimplexID<level>;
    
    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes, Iterator begin, Iterator end)
    {
        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);
        }
    }
};



template <typename Visitor, typename Complex, std::size_t k, std::size_t ring>
struct Neighbors_Down_Node
{
    static constexpr auto level = k;
    using SimplexID = typename Complex::template SimplexID<level>;

    using Neighbors_Down_Node_Next = Neighbors_Down_Node<Visitor,Complex,
            level,ring-1>;

    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes, Iterator begin, Iterator end)
    {
        NodeSet<SimplexID> next;

        for(auto curr = begin; curr != end; ++curr)
        {
            if(v.visit(F, *curr))
            {
                for(auto a : F.get_name(*curr))
                {
                    auto id = F.get_simplex_down(*curr,a);
                    for(auto b : F.get_cover(id)) 
                    {
                        auto nbor = F.get_simplex_up(id,b);
                        if(nodes.insert(nbor).second)
                        {
                            next.insert(nbor);
                        }
                    }
                }
            }
        }

        Neighbors_Down_Node_Next::apply(std::forward<Visitor>(v), F, nodes, next.begin(), next.end());
    }
};

template <typename Visitor, typename Complex, std::size_t k>
struct Neighbors_Down_Node<Visitor, Complex, k, 0>
{
    static constexpr auto level = k;
    using SimplexID = typename Complex::template SimplexID<level>;
    
    template <typename Iterator>
    static void apply(Visitor&& v, Complex& F, NodeSet<SimplexID>& nodes, Iterator begin, Iterator end)
    {
        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);
        }
    }
};

template <typename T> using AllowRepeat = std::vector<T>;

struct BFS_NoRepeat_Node_Traits
{
    template <typename T> using Container = NodeSet<T>;
};

struct BFS_NoRepeat_Edge_Traits
{
    template <typename T> using Container = NodeSet<T>;
    // template <typename Complex, typename SimplexID> auto node_next(Complex F, SimplexID s);
};
} // End namespace casc_visitor_detail

template <typename Visitor, typename SimplexID>
void visit_BFS_up(Visitor&& v, typename SimplexID::complex& F, SimplexID s)
{
    namespace cvd = casc_visitor_detail;
    cvd::BFS_Up_Node<Visitor, cvd::BFS_NoRepeat_Node_Traits, typename SimplexID::complex, 
            std::integral_constant<std::size_t,SimplexID::level>>::apply(
                    std::forward<Visitor>(v),F,&s,&s+1);
}

template <typename Visitor, typename SimplexID>
void visit_BFS_down(Visitor&& v, typename SimplexID::complex& F, SimplexID s)
{
    namespace cvd = casc_visitor_detail;
    cvd::BFS_Down_Node<Visitor, cvd::BFS_NoRepeat_Node_Traits, typename SimplexID::complex, 
            std::integral_constant<std::size_t,SimplexID::level>>::apply(
                    std::forward<Visitor>(v),F,&s,&s+1);
}

template <typename Visitor, typename EdgeID>
void edge_up(Visitor&& v, typename EdgeID::complex& F, EdgeID s)
{
    namespace cvd = casc_visitor_detail;
    cvd::BFS_Edge<Visitor, cvd::BFS_NoRepeat_Edge_Traits, typename EdgeID::complex, 
            std::integral_constant<std::size_t,EdgeID::level>>::apply(
                    std::forward<Visitor>(v),F,&s,&s+1);
}

template <std::size_t rings, typename Visitor, typename SimplexID>
void visit_neighbors_up(Visitor&& v, typename SimplexID::complex& F, SimplexID s)
{
    NodeSet<SimplexID> nodes{s};
    namespace cvd = casc_visitor_detail;
    cvd::Neighbors_Up_Node<Visitor,typename SimplexID::complex,SimplexID::level,rings>::apply(
            std::forward<Visitor>(v),F,nodes,&s,&s+1);
}

template <std::size_t rings, typename Visitor, typename SimplexID>
void visit_neighbors_down(Visitor&& v, typename SimplexID::complex& F, SimplexID s)
{
    NodeSet<SimplexID> nodes{s};
    namespace cvd = casc_visitor_detail;
    cvd::Neighbors_Down_Node<Visitor,typename SimplexID::complex,SimplexID::level,rings>::apply(
            std::forward<Visitor>(v),F,nodes,&s,&s+1);
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
void neighbors(Complex &F, typename Complex::template SimplexID<level> nid, InsertIter iter)
{
    for (auto a : F.get_name(nid))
    {
        auto id = F.get_simplex_down(nid, a);
        for (auto b : F.get_cover(id))
        {
            auto nbor = F.get_simplex_up(id, b);
            if (nbor != nid)
            {
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
void neighbors(Complex &F, SimplexID nid, InsertIter iter)
{
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
void neighbors_up(Complex &F, typename Complex::template SimplexID<level> nid, InsertIter iter)
{
    for (auto a : F.get_cover(nid))
    {
        auto id = F.get_simplex_up(nid, a);
        for (auto b : F.get_name(id))
        {
            auto nbor = F.get_simplex_down(id, b);
            if (nbor != nid)
            {
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
void neighbors_up(Complex &F, SimplexID nid, InsertIter iter)
{
    neighbors_up<Complex, SimplexID::level, InsertIter>(F, nid, iter);
}


// TODO: convert this to use an iterator inserter (1)
/**
 * @brief      Code for returning a set of k-ring neighbors.
 *
 * @param      F        { parameter_description }
 * @param      nodes    The nodes
 * @param[in]  next     The next
 * @param[in]  ring     The ring
 *
 * @tparam     Complex  { description }
 * @tparam     level    { description }
 *
 * @return     { description_of_the_return_value }
 */
template <class Complex, std::size_t level>
std::set<typename Complex::template SimplexID<level> > kneighbors_up(
    Complex                                                &F,
    std::set<typename Complex::template SimplexID<level> > &nodes,
    std::set<typename Complex::template SimplexID<level> >  next,
    int                                                     ring)
{
    if (ring == 0)
    {
        return nodes;
    }
    std::set<typename Complex::template SimplexID<level> > tmp;
    for (auto nid : next)
    {
        for (auto a : F.get_cover(nid))
        {
            auto id = F.get_simplex_up(nid, a);
            for (auto b : F.get_name(id))
            {
                auto nbor = F.get_simplex_down(id, b);
                if (nodes.insert(nbor).second)
                {
                    tmp.insert(nbor);
                }
            }
        }
    }
    return neighbors_up<Complex, level>(F, nodes, tmp, ring-1);
}

/**
 * @brief      Helper function to help kneighbors_up to deduce the integral
 *             level of SimplexID.
 *
 * @param[in]  F          The simplicial complex
 * @param[in]  nid        Simplex of interest to get the nieghbors of.
 * @param[in]  ring       The number of rings to include as a neighbor
 *
 * @tparam     Complex    Typename of the complex.
 * @tparam     SimplexID  Typename of the SimplexID.
 *
 * @return     The set of k-ring neighbors.
 */
template <class Complex, class SimplexID>
std::set<SimplexID> kneighbors_up(Complex &F, SimplexID nid, int ring)
{
    std::set<SimplexID> nodes{
        nid
    };
    return neighbors_up<Complex, SimplexID::level>(F, nodes, nodes, ring);
}
} // End namespace casc

