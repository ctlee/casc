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

#include <algorithm>
#include <unordered_set>
#include "util.h"
#include "stringutil.h"

namespace casc
{   
    template <class T> using set = std::set<T>;
    template <class T> using vector = std::vector<T>;

    template <typename Complex>
    struct SimplexSet
    {
        template <std::size_t j>
        using SimplexID           = typename Complex::template SimplexID<j>;
        using LevelIndex        = typename Complex::LevelIndex;
        using cLevelIndex       = typename util::remove_first_val<std::size_t,
                LevelIndex>::type;
        using RevIndex          = typename util::reverse_sequence<std::size_t,
                LevelIndex>::type;
        using cRevIndex         = typename util::reverse_sequence<std::size_t,
                cLevelIndex>::type;
        using type_this         = SimplexSet<Complex>;

        /**
         * @brief      Default constructor does nothing. 
         */
        SimplexSet() {};

        template <size_t k>
        inline void insert(SimplexID<k> s){
            std::get<k>(tupleSet).insert(s);
        }

        void insert(const SimplexSet<Complex>& s){
             util::int_for_each<std::size_t,LevelIndex>(
                    InsertHelper(), this, s);
        }

        template <size_t k>
        inline void erase(SimplexID<k> s){
            std::get<k>(tupleSet).erase(s);
        }

        void erase(const SimplexSet<Complex>& s){
                util::int_for_each<std::size_t,LevelIndex>(
                    EraseHelper(), this, s);
        }

        template <size_t k>
        inline auto find(SimplexID<k> s){
            return std::get<k>(tupleSet).find(s);
        }

        template <size_t k>
        inline auto end(){
            return std::get<k>(tupleSet).end();
        }

        template <size_t k>
        inline auto begin(){
            return std::get<k>(tupleSet).begin();
        }

        template <size_t k>
        inline auto& get(){
            return std::get<k>(tupleSet);
        }
        
        template <size_t k>
        inline auto& get() const {
            return std::get<k>(tupleSet);
        }

        void clear(){
            util::int_for_each<std::size_t, LevelIndex>(ClearHelper(), this);
        }

        friend std::ostream& operator<<(std::ostream& output, const SimplexSet<Complex>& S){
            output << "SimplexSet(";
            util::int_for_each<std::size_t, LevelIndex>(PrintHelper(), 
                    output,S);
            output << ")";
            return output;
        }

        using SimplexIDLevel    = typename util::int_type_map<std::size_t, 
                std::tuple, LevelIndex, SimplexID>::type;
        typename util::type_map<SimplexIDLevel, NodeSet>::type tupleSet;
    
    private:
        struct InsertHelper
        {
            template <std::size_t k>
            static void apply(type_this* that, const SimplexSet<Complex>& S)
            {
                auto s = std::get<k>(S.tupleSet);
                for(auto simplex : s){
                    that->insert(simplex);
                }
            }
        };
        
        struct EraseHelper
        {
            template <std::size_t k>
            static void apply(type_this* that, const SimplexSet<Complex>& S)
            {
                auto s = std::get<k>(S.tupleSet);
                for(auto simplex : s){
                    that->erase(simplex);
                }
            }
        };
        
        struct PrintHelper
        {
            template <std::size_t k>
            static void apply(std::ostream& output, const SimplexSet<Complex>& S)
            {
                output << "[l=" << k;
                auto s = std::get<k>(S.tupleSet);
                for(auto simplex : s){
                    output << ", " << simplex;
                }
                output << "]";
            }
        };

        struct ClearHelper
        {
            template <std::size_t k>
            void apply(type_this* that)
            {
                auto& s = std::get<k>(that->tupleSet);
                s.clear();
            }
        };

    };

    template <std::size_t k, typename Complex>
    static inline auto& get(SimplexSet<Complex>& S){
        return S.template get<k>();
    }

    template <std::size_t k, typename Complex>
    static inline auto& get(const SimplexSet<Complex>& S){
        return S.template get<k>();
    }

    namespace simplex_set_detail{
        template <typename Complex>
        struct UnionH
        {
            template <std::size_t k>
            static void apply(const SimplexSet<Complex>& A, 
                    const SimplexSet<Complex>& B, 
                    SimplexSet<Complex>& dest){
                auto a = std::get<k>(A.tupleSet);
                auto b = std::get<k>(B.tupleSet);
                auto& d = std::get<k>(dest.tupleSet);
                d.insert(a.begin(), a.end());
                d.insert(b.begin(), b.end());
            }
        };

        template <typename Complex>
        struct IntersectH
        {
            template <std::size_t k>
            static void apply(const SimplexSet<Complex>& A, 
                    const SimplexSet<Complex>& B, 
                    SimplexSet<Complex>& dest){
                auto a = casc::get<k>(A);
                auto b = casc::get<k>(B);
                auto& d = casc::get<k>(dest);

                if(a.size() < b.size()){
                    for(auto item : a){
                        if(b.find(item) != b.end())
                            d.insert(item);
                    }
                }else{
                    for(auto item : b){
                        if(a.find(item) != a.end())
                            d.insert(item);
                    }
                }
            }
        };

        template <typename Complex>
        struct DifferenceH
        {
            template<std::size_t k>
            static void apply(const SimplexSet<Complex>& A,
                    const SimplexSet<Complex>& B,
                    SimplexSet<Complex>& dest){
                auto a = casc::get<k>(A);
                auto b = casc::get<k>(B);
                auto& d = casc::get<k>(dest);

                for(auto item : a){
                    if(b.find(item) == b.end())
                        d.insert(item);
                }
            }
        };
    } // end namespace simplex_set_detail
    
    template <typename Complex>
    static SimplexSet<Complex>&& set_union(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B){
        SimplexSet<Complex> dest;
        dest.insert(A);
        dest.insert(B);
        return std::move(dest);
    }

    template <typename Complex>
    static void set_union(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B, 
            SimplexSet<Complex>& dest){
        util::int_for_each<std::size_t, 
                typename Complex::LevelIndex>(
                simplex_set_detail::UnionH<Complex>(), A, B, dest);
    }

    template <typename Complex>
    static SimplexSet<Complex>&& set_intersection(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B){
        SimplexSet<Complex> dest;
        util::int_for_each<std::size_t, 
                typename Complex::LevelIndex>(
                simplex_set_detail::IntersectH<Complex>(), A, B, dest);
        return std::move(dest);
    }

    template <typename Complex>
    static void set_intersection(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B, 
            SimplexSet<Complex>& dest){
        util::int_for_each<std::size_t, 
                typename Complex::LevelIndex>(
                simplex_set_detail::IntersectH<Complex>(), A, B, dest);
    }

    template <typename Complex>
    static SimplexSet<Complex>&& set_difference(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B){
        SimplexSet<Complex> dest;
        util::int_for_each<std::size_t,
                typename Complex::LevelIndex>(
                simplex_set_detail::DifferenceH<Complex>(), A, B, dest);
        return std::move(dest);
    }

    template <typename Complex>
    static void set_difference(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B, 
            SimplexSet<Complex>& dest){
        util::int_for_each<std::size_t,
                typename Complex::LevelIndex>(
                simplex_set_detail::DifferenceH<Complex>(), A, B, dest);
    }


    template <typename Complex>
    struct SimplexDataSet
    {
        using KeyType = typename Complex::KeyType;

        template <std::size_t k, typename T>
        struct DataType
        {
            using type = std::pair<std::array<KeyType,k>, T>;
        };

        template <std::size_t k>
        struct DataType<k, void>
        {
            using type = std::array<KeyType,k>;
        };

        template <std::size_t j>
        using DataSet = typename DataType<j, typename Complex::template NodeData<j>>::type;
        using LevelIndex = typename std::make_index_sequence<Complex::numLevels>;
        using SimplexIDLevel = typename util::int_type_map<std::size_t, 
                std::tuple, LevelIndex, DataSet>::type;
        using type = typename util::type_map<SimplexIDLevel, casc::vector>::type;
    };

    
} // end namespace casc

