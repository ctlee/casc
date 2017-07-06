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
        // NOTE TO CTL: this is the same as levels but on SimplexID not Node*
        // // Add functions for set addition and difference, intersection

        template <std::size_t j>
        using Simplex = typename Complex::template SimplexID<j>;
        using LevelIndex = typename Complex::LevelIndex;
        using SimplexIDLevel = typename util::int_type_map<std::size_t, std::tuple, LevelIndex, Simplex>::type;
        using type = typename util::type_map<SimplexIDLevel, NodeSet>::type;

        // Tuple of NodeSets by level
        type tupleSet;

        /**
         * @brief      Default constructor does nothing. Compiling 
         */
        SimplexSet() {};

        template <size_t k>
        inline void insert(Simplex<k> s){
            std::get<k>(tupleSet).insert(s);
        }

        template <size_t k>
        inline void erase(Simplex<k> s){
            std::get<k>(tupleSet).erase(s);
        }



        // friend std::ostream& operator<<(std::ostream& output, const SimplexSet<Complex>& S){
        //     output << "SimplexSet(";
        //     util::int_for_each<std::size_t, LevelIndex>(PrintHelper(), 
        //             std::forward<std::ostream&>(output), 
        //             std::forward<const SimplexSet<Complex>&>(S));
        //     output << ")" << std::endl;
        //     return output;
        // }
    
    private:

    };


    template <typename Complex>
    struct PrintHelper
    {
        template <std::size_t k>
        static void apply(SimplexSet<Complex>& S)
        {
            std::cout << "{l=" << k;
            auto s = std::get<k>(S.tupleSet);
            for(auto simplex : s){
                std::cout << ", " << simplex;
            }
            std::cout << "} ";
        }
    };

    template <typename Complex>
    void printSS(SimplexSet<Complex>& S)
    {
        std::cout << "SimplexSet(";
        util::int_for_each<std::size_t, 
            typename Complex::LevelIndex>(PrintHelper<Complex>(), S);
        std::cout << ")" << std::endl;
    }


    struct UnionHelper
    {
        template <typename Complex, std::size_t k>
        static void apply(const SimplexSet<Complex>& A, 
                const SimplexSet<Complex>& B, 
                SimplexSet<Complex>& dest){
                auto sA = std::get<k>(A);
                auto sB = std::get<k>(B);
                auto sDest = std::get<k>(dest);

                std::sort(sA.begin(), sA.end());
                std::sort(sB.begin(), sB.end());

                std::set_union(sA.begin(), sA.end(),
                       sB.begin(), sB.end(),                  
                       std::back_inserter(sDest));
        }
    };

    template <typename Complex>
    void set_union(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B, 
            SimplexSet<Complex>& dest){
        util::int_for_each<std::size_t, Complex::LevelIndex>(UnionHelper(A, B, dest));
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

