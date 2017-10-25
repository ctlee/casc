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
 * @file  decimate.h
 * @brief Meta-data aware decimation functions.
 */

#pragma once

#include <typeinfo>

#include "SimplexSet.h"
#include "SimplexMap.h"
#include "CASCTraversals.h"
#include "CASCFunctions.h"

namespace casc
{
/// @cond detail
/// Namespace for decimation related helpers
namespace decimation_detail
{
    /**
     * @brief      A map of
     *
     * @tparam     Complex  Typename of the simplicial_complex.
     */
    template <typename Complex>
    struct SimplexDataSet
    {
        /// Typename of vertices
        using KeyType = typename Complex::KeyType;

        /**
         * @brief      Data type makes a pair of array of keys to data type.
         *
         * @tparam     k     Dimension of simplex.
         * @tparam     T     Typename of the data.
         */
        template <std::size_t k, typename T>
        struct DataType
        {
            /// Pair of array to type
            using type = std::pair<std::array<KeyType,k>, T>;
        };

        /**
         * @brief      DataType for simplices with no data. 
         *
         * @tparam     k     Dimension of the simplex.
         */
        template <std::size_t k>
        struct DataType<k, void>
        {
            /// Array
            using type = std::array<KeyType,k>;
        };

        /// Template to resolve NodeData types for DataType.
        template <std::size_t j>
        using DataSet = typename DataType<j, typename Complex::template NodeData<j>>::type;
        /// Sequence of compile time integers.
        using LevelIndex = typename std::make_index_sequence<Complex::numLevels>;
        /// Tuple of DataSets corresponding to an integral level.
        using SimplexIDLevel = typename util::int_type_map<std::size_t, 
                std::tuple, LevelIndex, DataSet>::type;
        /// Vector of DataTypes for each integral level.
        using type = typename util::type_map<SimplexIDLevel, casc::vector>::type;
    };

    /**
     * @brief      Struct functional to get the complete neighborhood around a simplex
     *
     * @tparam     Complex  Type of simplicial complex
     */
    template <typename Complex>
    struct GetCompleteNeighborhood
    {
        using SimplexSet = typename casc::SimplexSet<Complex>;

        /**
         * @brief      Constructor
         *
         * @param      p     SimplexSet to use to pass results back
         */
        GetCompleteNeighborhood(SimplexSet* p) : pLevels(p) {}

        /**
         * @brief      Continue traversing, to the next level
         *
         * @return     True, continue the BFS
         */
        template <std::size_t level>
        bool visit(Complex& F, typename Complex::template SimplexID<level> s)
        {
            return true;
        }

        /**
         * @brief      Terminal case, go back up (visit_node_up).
         *
         * @param      F          Simplicial Complex
         * @param[in]  s          Simplex of interest
         *
         * @return     False, stop the BFS traversal
         */
        bool visit(Complex& F, typename Complex::template SimplexID<1> s)
        {
            visit_node_up(
                casc_detail::SimplexAggregator<Complex>(pLevels), F, s);
            return false;
        }

    private:
        SimplexSet* pLevels;
    };

    /**
     * @brief      Move found simplices from pLevels to pGrab.
     *
     * @tparam     Complex  Typename of Simplicial Complex
     */
    template <typename Complex>
    struct GrabVisitor
    {
        using SimplexSet = typename casc::SimplexSet<Complex>;

        GrabVisitor(SimplexSet* p, SimplexSet* grab) : pLevels(p), pGrab(grab) {}

        template <std::size_t level>
        bool visit(Complex& F, typename Complex::template SimplexID<level> s)
        {
            if(pLevels->find(s) != pLevels->template end<level>())
            {
                //std::cout << "GrabVisitor (found): " << s << std::endl;
                pLevels->erase(s);
                pGrab->insert(s);
                return true;
            }
            else
            {
                return false;
            }
        }

    private:
        SimplexSet* pLevels;
        SimplexSet* pGrab;
    };

    template <typename Complex, std::size_t BaseLevel>
    struct InnerVisitor
    {
        using SimplexSet = typename casc::SimplexSet<Complex>;
        using SimplexMap = typename casc::SimplexMap<Complex>;
        using Simplex = typename Complex::template SimplexID<BaseLevel>;
        using KeyType = typename Complex::KeyType;

        InnerVisitor(SimplexSet* p, Simplex s, KeyType np, SimplexMap* rv)
            : pLevels(p), simplex(s), new_point(np), data(rv) {}

        /**
         * @brief      Overloaded visit function
         *
         * @param      F          { parameter_description }
         * @param[in]  <unnamed>  { parameter_description }
         *
         * @tparam     OldLevel   { description }
         *
         * @return     { description_of_the_return_value }
         */
        template <std::size_t OldLevel>
        bool visit(Complex& F, typename Complex::template SimplexID<OldLevel> s)
        {
            constexpr std::size_t NewLevel = OldLevel - BaseLevel + 1;

            if(pLevels->find(s) != pLevels->template end<OldLevel>())
            {
                //std::cout << "InnerVisitor (found): " << s << std::endl;
                auto old_name = F.get_name(s);
                auto base_name = F.get_name(simplex);
                using NewArrayType = std::array<KeyType,NewLevel>;
                NewArrayType new_name;

                std::size_t i = 0; // new_name
                std::size_t j = 0; // old_name
                std::size_t k = 0; // base_name

                new_name[i++] = new_point;

                // Remove base_name from old_name and append to new_name
                while(i < NewLevel)
                {
                    if(base_name[k] == old_name[j])
                    {
                        // if equivalent than skip the value
                        ++j; ++k;
                    }
                    else
                    {
                        // append to new_name and increment
                        new_name[i++] = old_name[j++];
                    }
                }

                SimplexSet grab;
                visit_node_down(GrabVisitor<Complex>(pLevels,&grab), F, s);

                auto& levelMap = casc::get<NewLevel>(*data);
                auto it = levelMap.find(new_name);
                if(it != levelMap.end()){
                    it->second.insert(grab);
                }
                else{
                    auto ret = levelMap.insert(
                            std::pair<NewArrayType, SimplexSet>(new_name, grab));
                    assert(ret.second);
                }
            }
            return true;
        }

    private:
        SimplexSet* pLevels;
        Simplex     simplex;
        KeyType     new_point;
        SimplexMap* data;
    };


    template <typename Complex>
    struct MainVisitor
    {
        using SimplexSet = typename casc::SimplexSet<Complex>;
        using SimplexMap = typename casc::SimplexMap<Complex>;
        using KeyType = typename Complex::KeyType;

        MainVisitor(SimplexSet* p, KeyType np, SimplexMap* rv)
            : pLevels(p), new_point(np), data(rv) {}

        template <std::size_t level>
        bool visit(Complex& F, typename Complex::template SimplexID<level> s)
        {
            //std::cout << "MainVisitor: " << s << std::endl;
            visit_node_up(
                    InnerVisitor<Complex,level>(
                            pLevels,s,new_point,data),
                    F, s);
            return true;
        }

    private:
        SimplexSet* pLevels;
        KeyType     new_point;
        SimplexMap* data;
    };

    template <typename Complex, template<typename> class Callback>
    struct RunCallback
    {
        using SimplexMap = typename casc::SimplexMap<Complex>;
        using SimplexSet = typename casc::SimplexSet<Complex>;
        using SimplexDataSet = typename SimplexDataSet<Complex>::type;
        using KeyType = typename Complex::KeyType;
        template <std::size_t level> 
        using DataType = typename Complex::template NodeData<level>;


        template <std::size_t k, typename ReturnType>
        struct PerformCallback
        {
            static void apply(Complex& F, Callback<Complex>&& clbk, 
                    SimplexDataSet& rv,
                    const std::array<KeyType, k>& new_name,
                    const SimplexSet& merged){
                ReturnType rval = clbk(F, new_name, merged);
                std::get<k>(rv).push_back(std::make_pair(new_name,rval));
            }
        };

        template <std::size_t k>
        struct PerformCallback<k, void>
        {
            static void apply(Complex& F, Callback<Complex>&& clbk, 
                    SimplexDataSet& rv,
                    const std::array<KeyType, k>& new_name,
                    const SimplexSet& merged){
                clbk(F,new_name, merged);
                std::get<k>(rv).push_back(new_name);
            }
        };


        template <std::size_t k>
        static void apply(Complex& F, SimplexMap& S, 
                Callback<Complex>&& clbk, SimplexDataSet& rv){
            auto& levelMap = casc::get<k>(S);
            for (auto s : levelMap){
                PerformCallback<k, DataType<k>>::apply(F, std::forward<Callback<Complex>>(clbk),
                        rv, s.first, s.second);
            }
        }
    };

    template <typename Complex>
    struct PerformRemoval
    {
        template <std::size_t k>
        static void apply(Complex& F, casc::SimplexSet<Complex>& S){
            for(auto curr : casc::get<k>(S))
                F.remove(curr);
        }
    };

    
    template <typename Complex>
    struct PerformInsertion {
        using KeyType = typename Complex::KeyType;

        template <std::size_t k, class T>
        static void insert(Complex& F, std::pair<std::array<KeyType, k>, T> P){
            F.insert(P.first, P.second);
        }

        template <std::size_t k>
        static void insert(Complex& F, std::array<KeyType, k> A){
            F.insert(A);
        }

        template <std::size_t k>
        static void apply(Complex& F, 
                typename SimplexDataSet<Complex>::type& data){
            for(auto curr : std::get<k>(data)){
                insert(F, curr);
            }
        }
    };
} // end namespace decimation_detail

/**
 * @brief      Remove simplex in SimplexSet S from complex F
 *
 * @param      F        Simplicial Complex
 * @param      S        SimplexSet
 *
 * @tparam     Complex  Typename of complex
 */
template <typename Complex>
void perform_removal(Complex& F, casc::SimplexSet<Complex>& S){
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using LevelIndex = typename SimplexSet::cRevIndex;
    util::int_for_each<std::size_t, LevelIndex>(
            decimation_detail::PerformRemoval<Complex>(), F, S);
}

/**
 * @brief      Insert all simplices in SimplexSet $S$ into complex $F$
 *
 * @param      F        Simplicial Complex
 * @param      S        SimplexSet 
 *
 * @tparam     Complex  Typename of complex
 */
template <typename Complex>
void perform_insertion(Complex& F, 
        typename decimation_detail::SimplexDataSet<Complex>::type& S){
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using LevelIndex = typename SimplexSet::cLevelIndex;
    util::int_for_each<std::size_t, LevelIndex>(
            decimation_detail::PerformInsertion<Complex>(), F, S);
}  

template <typename Complex, template<typename> class Callback>
void run_user_callback(Complex& F,
        casc::SimplexMap<Complex>& S,
        Callback<Complex>&& clbk,
        typename decimation_detail::SimplexDataSet<Complex>::type& rv){
    using SimplexMap = typename casc::SimplexMap<Complex>;
    using LevelIndex = typename SimplexMap::cLevelIndex;

    util::int_for_each<std::size_t, LevelIndex>(
            decimation_detail::RunCallback<Complex, Callback>(), 
                F, S, std::forward<Callback<Complex>>(clbk), rv);  
}


/**
 * @brief      Decimation of a simplex
 *
 * @param      F         Simplicial complex to operate on
 * @param[in]  s         A simplex in F
 * @param      clbk      Callback function describing the mapping from simplices before to after
 *
 * @tparam     Complex   Type of simplicial complex
 * @tparam     Simplex   Type of simplex
 * @tparam     Callback  Functional type for metatemplate magic
 */
template <typename Complex, typename Simplex, template<typename> class Callback>
void decimate(Complex& F, Simplex s, Callback<Complex>&& clbk)
{
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using SimplexMap = typename casc::SimplexMap<Complex>;

    int np = F.add_vertex();
    SimplexSet nbhd;
    SimplexMap simplexMap;

    visit_node_down(
            decimation_detail::GetCompleteNeighborhood<Complex>(&nbhd),
            F, s);

    SimplexSet doomed = nbhd; // Save the list of simplices
    visit_node_down(
            decimation_detail::MainVisitor<Complex>(
                    &nbhd, np, &simplexMap), 
            F, s);
    typename decimation_detail::SimplexDataSet<Complex>::type rv;
    run_user_callback(F, simplexMap, std::forward<Callback<Complex>>(clbk), rv);
    perform_removal(F, doomed); 
    perform_insertion(F, rv);
}
} // end namespace casc
