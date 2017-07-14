#pragma once

#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include "util.h"

namespace casc
{
    template <class T> using set = std::set<T>;
    template <class T> using vector = std::vector<T>;

    template <typename Complex>
    struct SimplexSet
    {
        template <std::size_t j>
        using Simplex           = typename Complex::template SimplexID<j>;
        using LevelIndex        = typename Complex::LevelIndex;
        using cLevelIndex       = typename util::remove_first_val<std::size_t,LevelIndex>::type;
        using RevIndex          = typename util::reverse_sequence<std::size_t,LevelIndex>::type;
        using cRevIndex         = typename util::reverse_sequence<std::size_t,cLevelIndex>::type;
        using SimplexIDLevel    = typename util::int_type_map<std::size_t, std::tuple, LevelIndex, Simplex>::type;
        using type              = typename util::type_map<SimplexIDLevel, NodeSet>::type;
        using type_this         = SimplexSet<Complex>;

        // Tuple of NodeSets by level
        type tupleSet;

        /**
         * @brief      Default constructor does nothing. Compiling 
         */
        SimplexSet() {};

        template <size_t k>
        inline auto get(){
            return std::get<k>(tupleSet);
        }
        
        template <size_t k>
        inline auto get() const {
            return std::get<k>(tupleSet);
        }


        template <size_t k>
        inline void insert(Simplex<k> s){
            std::get<k>(tupleSet).insert(s);
        }

        template <size_t k>
        inline void erase(Simplex<k> s){
            std::get<k>(tupleSet).erase(s);
        }

        template <size_t k>
        inline auto find(Simplex<k> s){
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
    
    private:
        struct PrintHelper
        {
            template <std::size_t k>
            static void apply(std::ostream& output, const SimplexSet<Complex>& S)
            {
                output << "{l=" << k;
                auto s = std::get<k>(S.tupleSet);
                for(auto simplex : s){
                    output << ", " << simplex;
                }
                output << "} ";
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
                auto a = std::get<k>(A.tupleSet);
                auto b = std::get<k>(B.tupleSet);
                auto& d = std::get<k>(dest.tupleSet);

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
                auto a = std::get<k>(A.tupleSet);
                auto b = std::get<k>(B.tupleSet);
                auto& d = std::get<k>(dest.tupleSet);

                for(auto item : a){
                    if(b.find(item) == b.end())
                        d.insert(item);
                }
            }
        };
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
    static void set_intersect(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B, 
            SimplexSet<Complex>& dest){
        util::int_for_each<std::size_t, 
                typename Complex::LevelIndex>(
                simplex_set_detail::IntersectH<Complex>(), A, B, dest);
    }


    template <typename Complex>
    static void set_difference(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B, 
            SimplexSet<Complex>& dest){
        util::int_for_each<std::size_t,
                typename Complex::LevelIndex>(
                simplex_set_detail::DifferenceH<Complex>(), A, B, dest);
    }


    // This probably goes with decimation?
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
        using SimplexIDLevel = typename util::int_type_map<std::size_t, std::tuple, LevelIndex, DataSet>::type;
        using type = typename util::type_map<SimplexIDLevel, casc::vector>::type;
    };
}

