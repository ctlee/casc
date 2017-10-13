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

#include <array>
#include <map>
#include "util.h"
#include "stringutil.h"

namespace casc
{   
    template <class T> using set = std::set<T>;
    template <class T> using vector = std::vector<T>;

    template <typename Complex>
    struct SimplexMap
    {
        template <std::size_t j>
        using SimplexID          = typename Complex::template SimplexID<j>;
        using LevelIndex        = typename Complex::LevelIndex;
        using cLevelIndex       = typename util::remove_first_val<std::size_t,
                LevelIndex>::type;
        using RevIndex          = typename util::reverse_sequence<std::size_t,
                LevelIndex>::type;
        using cRevIndex         = typename util::reverse_sequence<std::size_t,
                cLevelIndex>::type;
        using type_this         = SimplexMap<Complex>;

        /**
         * @brief      Default constructor does nothing.
         */
        SimplexMap() {};

        // TODO: Put in convenience functions for easy accession etc...

        template <size_t k>
        inline auto& get(){
            return std::get<k>(tupleMap);
        }
        
        template <size_t k>
        inline auto& get() const {
            return std::get<k>(tupleMap);
        }

        // template <size_t k>
        // inline auto&& get(){
        //     return std::move(std::get<k>(tupleMap));
        // }
        
        // template <size_t k>
        // inline auto&& get() const {
        //     return std::move(std::get<k>(tupleMap));
        // }

        friend std::ostream& operator<<(std::ostream& output, const SimplexMap<Complex>& S){
            output << "SimplexMap(";
            util::int_for_each<std::size_t, LevelIndex>(PrintHelper(), 
                    output,S);
            output << ")";
            return output;
        }
     
    private:
        struct PrintHelper
        {
            template <std::size_t k>
            static void apply(std::ostream& output, const SimplexMap<Complex>& S)
            {
                output << "[l=" << k;
                auto s = std::get<k>(S.tupleMap);
                for(auto simplex : s){
                    output << ", " << cascstringutil::to_string(simplex.first) << ":" << simplex.second;
                }
                output << "]";
            }
        };

        template <std::size_t k> using array = std::array<typename Complex::KeyType, k>;
        using ArrayLevel = typename util::int_type_map<std::size_t, std::tuple, LevelIndex, array>::type;
        template <class T> using map = std::map<T,SimplexSet<Complex>>;
        typename util::type_map<ArrayLevel, map>::type tupleMap;
    };

    template <std::size_t k, typename Complex>
    static inline auto& get(SimplexMap<Complex>& S){
        return S.template get<k>();
    }

    template <std::size_t k, typename Complex>
    static inline auto& get(const SimplexMap<Complex>& S){
        return S.template get<k>();
    }
} // end namespace casc

