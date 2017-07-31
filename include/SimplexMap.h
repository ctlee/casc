#pragma once

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

