#pragma once

#include <string>

namespace cascstringutil
{
    /**
     * @brief      Returns a string representation of the vertex subsimplicies of a given simplex
     *
     * @param[in]  A     Array containing name of a simplex
     *
     * @tparam     T     Type of the array elements
     * @tparam     k     Number of elements
     *
     * @return     String representation of the object.
     */
    template <typename T, std::size_t k>
    std::string to_string(const std::array<T,k>& A)
    {
        if (k==0){
            return "{root}";
        }
        std::string out;
        out += "{";
        for(int i = 0; i + 1 < k; ++i)
        {
            out += std::to_string(A[i]) + ",";
        }
        if(k > 0)
        {
            out += std::to_string(A[k-1]);
        }
        out += "}";
        return out;
    }
}