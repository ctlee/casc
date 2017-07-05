#pragma once

#include <algorithm>

namespace casc
{
    template <class T> using set = std::set<T>;
    template <class T> using vector = std::vector<T>;

    template <typename Complex>
    struct SimplexSet
    {
        // NOTE TO CTL: this is the same as levels but on SimplexID not Node*
        // // Add functions for set addition and difference, intersection
        // inline insert/remove
        // Also iterations over the simplex set?

        template <std::size_t j>
        using Simplex = typename Complex::template SimplexID<j>;
        using LevelIndex = typename std::make_index_sequence<Complex::numLevels>;
        using SimplexIDLevel = typename util::int_type_map<std::size_t, std::tuple, LevelIndex, Simplex>::type;
        using type = typename util::type_map<SimplexIDLevel, NodeSet>::type;
    };

    template <class Complex, typename K>
    struct Union {};

    template <class Complex, std::size_t k>
    struct Union<Complex, std::integral_constant<std::size_t, k>>
    {
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

    template <class Complex>
    struct Union<Complex, std::integral_constant<std::size_t, Complex::topLevel>>
    {
        static void apply(const SimplexSet<Complex>& A, 
                    const SimplexSet<Complex>& B, 
                    SimplexSet<Complex>& dest){
            static constexpr auto k = Complex::topLevel;

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

    template <class Complex>
    SimplexSet<Complex>& set_union(const SimplexSet<Complex>& A, 
            const SimplexSet<Complex>& B){

        SimplexSet<Complex> dest;
        Union<Complex, std::integral_constant<std::size_t, 0>>::apply(A, B, dest);
        return dest;
    }

    // Apply some function to sets at each level
    template <typename Func, typename Complex, typename K>
    struct SimplexSet_Up_Node {};

    


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

