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
 * ***************************************************************************
 */

#pragma once

#include <utility>
#include <array>

/// Metatemplate programming utilities namespace
namespace util
{
/**
 * @brief      A range object to support range based for loops.
 *
 * This is a basic data structure which implements a `begin()` and `end()` 
 * functions for range based for lopping added in C++11.
 * See also 
 * <a href="http://en.cppreference.com/w/cpp/language/range-for">range-for</a>.
 *
 * @tparam     T     Typename of the iterator
 */
template<typename T> struct range
{

    /**
     * @brief      Construct a range for a container class.
     *
     * @param[in]  c  Container class which implements begin() and end().
     *
     * @tparam     C  Typename of the container.
     */
    template <class C>
    range(C &&c) : _begin(c.begin()), _end(c.end()) {}

    /**
     * @brief      Construct a range from an iterator.
     *
     * @param[in]  b     Beginning iterator
     * @param[in]  e     End iterator.
     */
    range(T b, T e) : _begin(b), _end(e) {}

    /**
     * @brief      Get the beginning iterator.
     *
     * @return     Returns an iterator to the beginning.
     */
    T begin() { return _begin; }

    /**
     * @brief      Get the end iterator.
     *
     * @return     Returns an iterator to the end.
     */
    T end() { return _end; }

    private:
        /// Iterator to the beginning.
        T _begin;
        /// Iterator to the end.
        T _end;
};

/**
 * @brief      Make a range object.
 *
 * @param[in]  b     Iterator to the beginning.
 * @param[in]  e     Iterator to the end.
 *
 * @tparam     T     Typename of the iterator.
 *
 * @return     Returns a range of the iterators.
 */
template<typename T> range<T> make_range(T b, T e)
{
    return range<T>(std::move(b), std::move(e));
}

/**
 * @brief      Makes a range object.
 *
 * @param[in]  p     A pair containing begin and end iterators.
 *
 * @tparam     T     Typename of the iterator.
 *
 * @return     Returns a range of the iterators.
 */
template<typename T> range<T> make_range(std::pair<T, T> p)
{
    return range<T>(std::move(p.first), std::move(p.second));
}

/**
 * @brief      Queue based data structure to hold list of types.
 *
 * Types in the type_holder can be accessed by accessing the `head` type.
 * Subsequent types are in the `tail`. See also type_get.
 *
 * @tparam     Ts    List of typenames
 */
template <typename ... Ts>
struct type_holder
{
    /// Length of the list of types
    static const size_t size = sizeof ... (Ts);
};

/**
 * @brief      Partial specialization to allow FIFO access of typenames.
 *
 * @tparam     T     The first typename
 * @tparam     Ts    The following typenames
 */
template <typename T, typename ... Ts>
struct type_holder<T, Ts...>
{
    /// The first type
    using head = T;
    /// The following types
    using tail = type_holder<Ts...>;
    /// Length of the list of types
    static const size_t size = 1 + type_holder<Ts...>::size;
};

/**
 * @brief      Helper to get the kth element from a type_holder.
 *
 * This is the empty general template which will be later specialized.
 *
 * @tparam     k     Integer index of the type to retrieve
 * @tparam     T     A type_holder queue of typenames
 */
template <size_t k, typename T>
struct type_get {};

/**
 * @brief      Specialization for terminal case.
 * 
 * @tparam     Ts    Following typenames
 */
template <typename ... Ts>
struct type_get<0, type_holder<Ts...> >
{
    /// The first type of the type_holder
    using type = typename type_holder<Ts...>::head;
};

/**
 * @brief      Specialization to recursively pop types to get the kth type.
 *
 * @tparam     k     Integral constant of the type to get
 * @tparam     Ts    List of typenames
 */
template <size_t k, typename ... Ts>
struct type_get<k, type_holder<Ts...> >
{
    /// Recurse after popping the first type off
    using type = typename type_get<k-1, typename type_holder<Ts...>::tail>::type;
};

/// Namespace for utility helper functions
namespace detail
{
/**
 * @brief      Helper to broadcast a list of types into a class.
 *
 * @tparam     C	 Class to old a list of types
 * @tparam     V     Class to broadcast the types into
 * @tparam     Rs    List of resulting types
 */
template <class C, template <typename> class V, typename ... Rs>
struct type_map_helper {};

/**
 * @brief      Terminal condition: place the mapped types into a tuple
 *
 * @tparam     G     Empty tuple
 * @tparam     V     Type template <class T> class to map into
 * @tparam     Rs	 List of already mapped types
 */
template <template <class ...> class G, template <typename> class V, typename ... Rs>
struct type_map_helper<G<>, V, Rs...>
{
    using type = G<Rs...>;
};

/**
 * @brief      Iteratively map the types in G into V<T>.
 *
 * @tparam     G     Tuple of types
 * @tparam     T     Current type
 * @tparam     Ts    List of remaining types
 * @tparam     V     Type template <class T> class to map into
 * @tparam     Rs    List of already mapped types
 */
template <template < class ...> class G, typename T, typename ... Ts, template <typename> class V, typename ... Rs>
struct type_map_helper<G<T, Ts...>, V, Rs...>
{
    using type = typename type_map_helper<G<Ts...>, V, Rs..., V<T> >::type;
};
}     // end of namespace detail

/**
 * @brief      Map the types in C into V<T>
 *
 * Given a container of types `C<T1,T2,T3,...>` and template template type 
 * `V<T>`, this function will apply the types in C to V<T>. This produces
 * C<V<T1>, V<T2>, V<T3>, ...>. 
 * 
 * @tparam     C     Container of types to map into V<T>
 * @tparam     V     Type template <class T> class to map into
 */
template <class C, template <typename> class V>
struct type_map
{
    using type = typename detail::type_map_helper<C, V>::type;
};

namespace detail
{
/**
 * @brief      Template for future specialization
 */
template <class IntegerType, template <class ...> class OutHolder, class IntegerSequence, template <IntegerType> class F, typename ... Accumulators>
struct int_type_map_helper {};

/**
 * @brief      Apply the typenames to the OutHolder
 *
 * @tparam     Integer      Integral type
 * @tparam     OutHolder    Type to ultimately hold the accumulated
 * @tparam     InHolder     Class of index sequence
 * @tparam     F            Type to apply index to
 * @tparam     Accumulator  List of mapped typenames F<I>
 */
template <class Integer, template <class ...> class OutHolder, template <class, Integer...> class InHolder, template <Integer> class F, class ... Accumulator>
struct int_type_map_helper<Integer, OutHolder, InHolder<Integer>, F, Accumulator...>
{
    using type = OutHolder<Accumulator...>;
};

/**
 * @brief      Iterates across integers and fills accumulator with F<I>
 *
 * @tparam     Integer      Integral type
 * @tparam     OutHolder    Type to ultimately hold the accumulated
 * @tparam     InHolder     Class of index sequence
 * @tparam     I            Current integer
 * @tparam     Is           Next integer(s)
 * @tparam     F            Type to apply index to
 * @tparam     Accumulator  List of previously mapped typenames F<I>
 */
template <class Integer, template <class ...> class OutHolder, template <class, Integer...> class InHolder, Integer I, Integer... Is, template <Integer> class F, class ... Accumulator>
struct int_type_map_helper<Integer, OutHolder, InHolder<Integer, I, Is...>, F, Accumulator...>
{
    using type = typename int_type_map_helper<Integer, OutHolder, InHolder<Integer, Is...>, F, Accumulator..., F<I> >::type;
};
} // end namespace detail

/**
 * @brief      Maps an integer sequence and typename, F<I>, into outholder
 * 
 * Given an Integer Sequence I<0,1,2,3,...> and template template type F<I>,
 * this function produces Out<F<0>, F<1>, F<2>, ...>. 
 *
 * @tparam     IntegerType      Typename of an integer type
 * @tparam     OutHolder        Typename of a holder for types 
 * @tparam     IntegerSequence  Integral sequence of types
 * @tparam     F                Typename of class to be broadcast with integer
 */
template <class IntegerType, template <class ...> class OutHolder, class IntegerSequence, template <IntegerType> class F>
struct int_type_map
{
    using type = typename detail::int_type_map_helper<IntegerType, OutHolder, IntegerSequence, F>::type;
};

/**
 * @brief      Move a list of types from one container to another.
 *
 * @tparam     TUPLE        Empty container
 * @tparam     HOLDER_FULL  Full container
 */
template <template <class ...> class TUPLE, typename HOLDER_FULL>
struct type_swap
{};

/**
 * @brief      Move a list of types from one container to another.
 *
 * @tparam     TUPLE   Empty container
 * @tparam     HOLDER  Full container 
 * @tparam     Ts      Typenames in full container
 */
template <template <class ...> class TUPLE, template <class ...> class HOLDER, typename ... Ts>
struct type_swap<TUPLE, HOLDER<Ts...> >
{
	/// Empty container filled with typenames from full container
    using type = TUPLE<Ts...>;
};


namespace detail
{
/**
 * @brief      { struct_description }
 *
 * @tparam     Integer          { description }
 * @tparam     IntegerSequence  { description }
 * @tparam     Accumulator      { description }
 */
template <class Integer, class IntegerSequence, Integer... Accumulator>
struct reverse_sequence_helper {};

template <class Integer,
          template<class, Integer...> class InHolder,
          Integer... Accumulator>
struct reverse_sequence_helper<Integer, InHolder<Integer>, Accumulator...>
{
    using type = InHolder<Integer, Accumulator...>;
};

template <class Integer,
          template<class, Integer...> class InHolder,
          Integer I, Integer... Is,
          Integer... Accumulator>
struct reverse_sequence_helper<Integer, InHolder<Integer, I, Is...>, Accumulator...>
{
    using type = typename reverse_sequence_helper<Integer,
                                                  InHolder<Integer, Is...>, I, Accumulator...>::type;
};
} // end namespace detail

/**
 * @brief      Reverse an Integer Sequence
 *
 * @tparam     Integer          { description }
 * @tparam     IntegerSequence  { description }
 */
template <class Integer, class IntegerSequence>
struct reverse_sequence
{
    using type = typename detail::reverse_sequence_helper<Integer, IntegerSequence>::type;
};


template <class Integer, class IntegerSequence>
struct remove_first_val {};

template <class Integer,
          template<class, Integer...> class InHolder,
          Integer I, Integer... Is>
struct remove_first_val<Integer, InHolder<Integer, I, Is...> >
{
    using type = InHolder<Integer, Is...>;
};


namespace detail
{
/**
 * @brief      Template type for future specialization
 
 */
template <typename Integer, typename IntegerSequence, typename Fn, typename ... Args>
struct int_for_each_helper {};

/**
 * @brief      Terminal Case
 *
 * @tparam     Integer   { description }
 * @tparam     InHolder  { description }
 * @tparam     I         { description }
 * @tparam     Fn        { description }
 * @tparam     Args      { description }
 */
template <class Integer, template <class, Integer...> class InHolder,
          Integer I, typename Fn, typename ... Args>
struct int_for_each_helper<Integer, InHolder<Integer, I>, Fn, Args...>
{
    static void apply(Fn &&f, Args && ... args)
    {
        f.template apply<I>(std::forward<Args>(args) ...);
    }
};

/**
 * @brief      Recurse through the integer series
 *
 * @tparam     Integer   { description }
 * @tparam     InHolder  { description }
 * @tparam     I         { description }
 * @tparam     Is        { description }
 * @tparam     Fn        { description }
 * @tparam     Args      { description }
 */
template <class Integer, template <class, Integer...> class InHolder,
          Integer I, Integer... Is, typename Fn, typename ... Args>
struct int_for_each_helper<Integer, InHolder<Integer, I, Is...>, Fn, Args...>
{
    static void apply(Fn &&f, Args && ... args)
    {
        f.template apply<I>(std::forward<Args>(args) ...);
        int_for_each_helper<Integer, InHolder<Integer, Is...>, Fn, Args...>::apply(
            std::forward<Fn>(f),
            std::forward<Args>(args) ...);
    }
};
} // end namespace detail

/**
 * @brief      Calls a function f.apply<k>() for a sequence of integer k's
 *
 * @param[in]  args             Arguments to f
 * @param[in]  f                Functor with apply<k>() method
 *
 * @tparam     Integer          Integer type
 * @tparam     IntegerSequence  Sequence of integers to iterate
 * @tparam     Fn               Typename of functor f
 * @tparam     Args             Typenames of the arguments
 */
template <class Integer, typename IntegerSequence, typename Fn, typename ... Args>
void int_for_each(Fn &&f, Args && ... args)
{
    detail::int_for_each_helper<Integer, IntegerSequence, Fn, Args...>::apply(std::forward<Fn>(f),
                                                                      std::forward<Args>(args) ...);
}
} // End of namespace util
