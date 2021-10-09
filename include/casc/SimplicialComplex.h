// This file is part of the Colored Abstract Simplicial Complex library.
// Copyright (C) 2016-2021
// by Christopher T. Lee, John Moody, Rommie Amaro, J. Andrew McCammon,
//    and Michael Holst
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, see <http://www.gnu.org/licenses/>
// or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301 USA

/**
 * @file  SimplicialComplex.h
 * @brief This header contains the main CASC data structure and associated
 *        components.
 */

#pragma once

#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "index_tracker.h"
#include "util.h"

#if __has_cpp_attribute(maybe_unused)
#define MAYBE_UNUSED [[maybe_unused]]
#else
#define MAYBE_UNUSED
#endif

/// Namespace for everything CASC
namespace casc {
/// @cond detail
/// Namespace for CASC internal data structures
namespace detail {
/// Data structure to store simplices by level.
template <class T> using map = std::map<std::size_t, T>;

/**
 * @brief      A generic pair type representing Key to Value associations.
 *
 * @tparam     T1    Typename of the Key
 * @tparam     T2    Typename of the Value
 */
template <typename T1, typename T2> struct asc_pair {
  using this_t = asc_pair<T1, T2>;
  asc_pair() {}
  asc_pair(const T1 &first, const T2 &second) : _pair(first, second) {}
  asc_pair(T1 &&first, T2 &&second)
      : _pair(std::forward<T1>(first), std::forward<T2>(second)) {}
  asc_pair(const this_t &other) : _pair(other._pair) {}
  asc_pair(this_t &&other)
      : _pair(std::forward<std::pair<T1, T2>>(other._pair)) {}

  operator T1() const { return _pair.first; }

  this_t &operator=(const this_t &other) {
    _pair = other._pair;
    return *this;
  }

  this_t &operator=(this_t &&other) {
    _pair = std::move(other._pair);
    return *this;
  }

  friend bool operator==(const this_t &lhs, const this_t &rhs) {
    return lhs.first == rhs.first;
  }
  friend bool operator!=(const this_t &lhs, const this_t &rhs) {
    return lhs.first != rhs.first;
  }
  friend bool operator<=(const this_t &lhs, const this_t &rhs) {
    return lhs.first <= rhs.first;
  }
  friend bool operator>=(const this_t &lhs, const this_t &rhs) {
    return lhs.first >= rhs.first;
  }
  friend bool operator<(const this_t &lhs, const this_t &rhs) {
    return lhs.first < rhs.first;
  }
  friend bool operator>(const this_t &lhs, const this_t &rhs) {
    return lhs.first > rhs.first;
  }

  T1 &first = _pair.first;
  T2 &second = _pair.second;

private:
  std::pair<T1, T2> _pair;
};

/**
 * @brief      Array of asc_pairs sorted by Key for boundary adjacency storage.
 *
 * @tparam     KEY_T  Typename of Key
 * @tparam     VAL_T  Typename of Value
 * @tparam     k      Size of the array
 */
template <typename KEY_T, typename VAL_T, std::size_t k> struct asc_arraymap {
  using pair_t = asc_pair<KEY_T, VAL_T>;
  using array_t = std::array<pair_t, k>;
  using iterator = typename array_t::iterator;
  using const_iterator = typename array_t::const_iterator;

  asc_arraymap() {
    _begin = _array.begin();
    _end = _array.begin();
  }

  void insert(pair_t &p) {
    if (_end == _array.end())
      throw std::out_of_range(
          "insert&: Adding element beyond the end of array.");
    *_end = p;
    ++_end;
    std::sort(_begin, _end);
  }

  void insert(pair_t &&p) {
    if (_end == _array.end())
      throw std::out_of_range(
          "insert&&: Adding element beyond the end of array.");
    *_end = std::forward<pair_t>(p);
    ++_end;
    std::sort(_begin, _end);
  }

  iterator find(const KEY_T &key) { return std::find(_begin, _end, key); }

  void erase(const KEY_T &key) {
    auto it = std::find(_begin, _end, key);
    if (it != _end) {
      std::copy(it + 1, _end, it);
      --_end;
    }
  }

  std::size_t size() const { return std::distance(_end, _begin); }

  VAL_T &operator[](const KEY_T &key) {
    auto it = std::find(_begin, _end, key);
    if (it != _end) {
      return it->second;
    } else {
      if (_end == _array.end())
        throw std::out_of_range(
            "operator[]: Adding element beyond the end of array.");
      _end->first = key;
      ++_end;
      std::sort(_begin, _end);
      return std::find(_begin, _end, key)->second;
    }
  }

  iterator begin() { return _begin; }
  iterator end() { return _end; }
  const_iterator cbegin() const { return _begin; }
  const_iterator cend() const { return _end; }

private:
  array_t _array;
  iterator _begin;
  iterator _end;
};

/**
 * @brief      Sorted vector of asc_pairs for coboundary relation storage.
 *
 * @tparam     KEY_T  Typename of Key
 * @tparam     VAL_T  Typename of Values
 */
template <typename KEY_T, typename VAL_T> struct asc_vectormap {
  using pair_t = asc_pair<KEY_T, VAL_T>;
  using vector_t = std::vector<pair_t>;
  using iterator = typename vector_t::iterator;
  using const_iterator = typename vector_t::const_iterator;

  asc_vectormap() {}

  void insert(pair_t &p) {
    iterator first = std::lower_bound(_vector.begin(), _vector.end(), p);
    if ((first == _vector.end()) || (*first != p)) {
      _vector.insert(first, p);
    } else {
      std::cout << "Item already exists...";
    }
  }

  void insert(pair_t &&p) {
    iterator first = std::lower_bound(_vector.begin(), _vector.end(), p);
    if ((first == _vector.end()) || (*first != p)) {
      _vector.insert(first, std::forward<pair_t>(p));
    } else {
      std::cout << "Item already exists...";
    }
  }

  iterator find(const KEY_T &key) {
    iterator first = std::lower_bound(_vector.begin(), _vector.end(), key);
    if (first != _vector.end()) {
      if (*first != key) {
        return _vector.end();
      } else {
        return first;
      }
    } else {
      return first;
    }
  }

  void erase(const KEY_T &key) {
    iterator it = this->find(key);
    if (it != _vector.end()) {
      _vector.erase(it);
    }
  }

  std::size_t size() const { return _vector.size(); }

  VAL_T &at(const KEY_T &key) {
    iterator first = std::lower_bound(_vector.begin(), _vector.end(), key);
    if ((first == _vector.end()) || (first->first != key)) {
      throw std::out_of_range("Could not find element in asc_vectormap.");
    } else {
      return first->second;
    }
  }

  VAL_T &operator[](const KEY_T &key) {
    iterator first = std::lower_bound(_vector.begin(), _vector.end(), key);
    if ((first == _vector.end()) || (first->first != key)) {
      first = _vector.emplace(first, pair_t());
      first->first = key;
      return first->second;
    } else {
      return first->second;
    }
  }

  iterator begin() { return _vector.begin(); }
  iterator end() { return _vector.end(); }
  const_iterator cbegin() const { return _vector.cbegin(); }
  const_iterator cend() const { return _vector.cend(); }

private:
  vector_t _vector;
};

/**
 * @brief Template prototype for Nodes in CASC.
 *
 * asc_Node must be defined outside of simplicial_complex because C++ does
 * not allow internal templates to be partially specialized. This template
 * prototype is later specialized to represent various Node roles.
 */
template <class KeyType, std::size_t k, std::size_t N, typename DataTypes,
          class>
struct asc_Node;

/// This is the base Node class.
struct asc_NodeBase {
  /**
   * @brief      Construct a Node
   *
   * @param[in]  id    An internal integer identifier of the Node.
   */
  asc_NodeBase(std::size_t id) : _node(id) {}
  virtual ~asc_NodeBase(){}; /**< Destructor */
  std::size_t _node;         /**< Internal Node ID*/
};

/**
 * @brief      Base class for Node with some data.
 *
 * @tparam     DataType  Typename of the data to be stored.
 */
template <class DataType> struct asc_NodeData {
  DataType _data; /**< stored data with type DataType */
};

/**
 * @brief      Explicit specialization for Nodes without data.
 *
 * This exists so that the compiler knows to not allocate any memory to
 * store data when void is specified.
 */
template <> struct asc_NodeData<void> {};

/**
 * @brief      Base class for Nodes with edge data.
 *
 * @tparam     KeyType   Typename of index for indexing Nodes.
 * @tparam     DataType  Typename of the data stored on the edge.
 */
template <class KeyType, class DataType> struct asc_EdgeData {
  /** The map of SimplexIDs to stored edge data. */
  std::unordered_map<KeyType, DataType> _edge_data;
};

/**
 * @brief      Explicit specialization for Nodes with no edge data.
 *
 * @tparam     KeyType   Typename of index for indexing Nodes.
 */
template <class KeyType> struct asc_EdgeData<KeyType, void> {};

/**
 * @brief      Base class for Node with parent nodes
 *
 * @tparam     KeyType        Typename of the Node index
 * @tparam     k              The Simplex dimension
 * @tparam     N              Dimension of the Complex
 * @tparam     NodeDataTypes  A util::type_holder array of Node types
 * @tparam     EdgeDataTypes  A util::type_holder array of Edge types
 */
template <class KeyType, std::size_t k, std::size_t N, class NodeDataTypes,
          class EdgeDataTypes>
struct asc_NodeDown
    : public asc_EdgeData<KeyType,
                          typename util::type_get<k - 1, EdgeDataTypes>::type> {
  /** Alias the typename of the parent Node */
  using DownNodeT = asc_Node<KeyType, k - 1, N, NodeDataTypes, EdgeDataTypes>;

  /** Map of indices to parent Node pointers*/
  asc_arraymap<KeyType, DownNodeT *, k> _down;
  // std::map<KeyType, DownNodeT*> _down;
};

/**
 * @brief      Base class for Node with children Nodes
 *
 * @tparam     KeyType        Typename of the Node index
 * @tparam     k              The Simplex dimension
 * @tparam     N              Dimension of the Complex
 * @tparam     NodeDataTypes  A util::type_holder array of Node types
 * @tparam     EdgeDataTypes  A util::type_holder array of Edge types
 */
template <class KeyType, std::size_t k, std::size_t N, class NodeDataTypes,
          class EdgeDataTypes>
struct asc_NodeUp {
  /// Typename of the nodes up.
  using UpNodeT = asc_Node<KeyType, k + 1, N, NodeDataTypes, EdgeDataTypes>;
  asc_vectormap<KeyType, UpNodeT *> _up;
  // std::unordered_map<KeyType, UpNodeT*> _up;      /**< @brief Map of pointers
  // to children */
};

/**
 * @brief      Node with both parents and children
 *
 * @tparam     KeyType        Typename of the Node index
 * @tparam     k              The Simplex dimension
 * @tparam     N              Dimension of the Complex
 * @tparam     NodeDataTypes  A util::type_holder of Node types
 * @tparam     EdgeDataTypes  A util::type_holder of Edge types
 */
template <class KeyType, std::size_t k, std::size_t N, class NodeDataTypes,
          class EdgeDataTypes>
struct asc_Node
    : public asc_NodeBase,
      public asc_NodeData<typename util::type_get<k, NodeDataTypes>::type>,
      public asc_NodeDown<KeyType, k, N, NodeDataTypes, EdgeDataTypes>,
      public asc_NodeUp<KeyType, k, N, NodeDataTypes, EdgeDataTypes> {
  /// Dimension of the simplex.
  static constexpr std::size_t level = k;

  /**
   * @brief      Default constructor
   *
   * @param[in]  id    The internal integer identifier.
   */
  asc_Node(std::size_t id) : asc_NodeBase(id) {}

  /**
   * @brief      Print the Node out for debugging only
   *
   * @param      output  The output stream.
   * @param[in]  node    The Node of interest to print.
   *
   * @return     A handle to the output stream.
   */
  friend std ::ostream &operator<<(std::ostream &output, const asc_Node &node) {
    output << "Node(level=" << k << ", "
           << "id=" << node._node;
    if (node._down.size() > 0) {
      for (auto it = node._down.cbegin(); it != node._down.cend(); ++it) {
        output << ", NodeDownID={'" << it->first << "', " << it->second->_node
               << "}";
      }
    }
    if (node._up.size() > 0) {
      for (auto it = node._up.cbegin(); it != node._up.cend(); ++it) {
        output << ", NodeUpID={'" << it->first << "', " << it->second->_node
               << "}";
      }
    }
    output << ")";
    return output;
  }
};

/**
 * @brief      Node with only children i.e., the root.
 *
 * @tparam     KeyType        Typename of the Node index
 * @tparam     N              The Simplex dimension
 * @tparam     NodeDataTypes  A util::type_holder of Node types
 * @tparam     EdgeDataTypes  A util::type_holder of Edge types
 */
template <class KeyType, std::size_t N, class NodeDataTypes,
          class EdgeDataTypes>
struct asc_Node<KeyType, 0, N, NodeDataTypes, EdgeDataTypes>
    : public asc_NodeBase,
      public asc_NodeData<typename util::type_get<0, NodeDataTypes>::type>,
      public asc_NodeUp<KeyType, 0, N, NodeDataTypes, EdgeDataTypes> {
  /// Dimension of the simplex.
  static constexpr std::size_t level = 0;

  /**
   * @brief      Default constructor
   *
   * @param[in]  id    The internal integer identifier.
   */
  asc_Node(std::size_t id) : asc_NodeBase(id) {}

  /**
   * @brief      Print the Node out for debugging only
   *
   * @param      output  The output stream.
   * @param[in]  node    The Node of interest to print.
   *
   * @return     A handle to the output stream.
   */
  friend std ::ostream &operator<<(std::ostream &output, const asc_Node &node) {
    output << "Node(level=" << 0 << ", id=" << node._node;
    if (node._up.size() > 0) {
      for (auto it = node._up.cbegin(); it != node._up.cend(); ++it) {
        output << ", NodeUpID={'" << it->first << "', " << it->second->_node
               << "}";
      }
    }
    output << ")";
    return output;
  }
};

/**
 * @brief      Top level node with only parents
 *
 * @tparam     KeyType        Typename of the Node index
 * @tparam     N              The Simplex dimension
 * @tparam     NodeDataTypes  A util::type_holder of Node types
 * @tparam     EdgeDataTypes  A util::type_holder of Edge types
 */
template <class KeyType, std::size_t N, class NodeDataTypes,
          class EdgeDataTypes>
struct asc_Node<KeyType, N, N, NodeDataTypes, EdgeDataTypes>
    : public asc_NodeBase,
      public asc_NodeData<typename util::type_get<N, NodeDataTypes>::type>,
      public asc_NodeDown<KeyType, N, N, NodeDataTypes, EdgeDataTypes> {
  /// Dimension of the simplex.
  static constexpr std::size_t level = N;

  /**
   * @brief      Default constructor
   *
   * @param[in]  id    The internal integer identifier.
   */
  asc_Node(std::size_t id) : asc_NodeBase(id) {}

  /**
   * @brief      Print the Node out for debugging only
   *
   * @param      output  The output stream.
   * @param[in]  node    The Node of interest to print.
   *
   * @return     A handle to the output stream.
   */
  friend std ::ostream &operator<<(std::ostream &output, const asc_Node &node) {
    output << "Node(level=" << N << ", id=" << node._node;
    if (node._down.size() > 0) {
      for (auto it = node._down.cbegin(); it != node._down.cend(); ++it) {
        output << ", NodeDownID={'" << it->first << "', " << it->second->_node
               << "}";
      }
    }
    output << ")";
    return output;
  }
};

/**
 * @brief      An iterator adapter to iterate over NodeIDs.
 *
 * @tparam     Iter  Typename of the iterator
 * @tparam     Data  Typename of the data
 */
template <typename Iter, typename Data>
struct node_id_iterator
    : public std::iterator<std::bidirectional_iterator_tag, Data> {
public:
  /// Inherit from a bidirectional std::iterator.
  using super = std::iterator<std::bidirectional_iterator_tag, Data>;
  /// Empty constructor
  node_id_iterator() {}
  /// Instantiate with an iterator to wrap
  node_id_iterator(Iter j) : i(j) {}
  /// Increment the iterator
  node_id_iterator &operator++() {
    ++i;
    return *this;
  }
  /// Increment the iterator
  node_id_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }
  /// Decrement the iterator
  node_id_iterator &operator--() {
    --i;
    return *this;
  }
  /// Decrement hte iterator
  node_id_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }
  /// Iterator equality comparison
  bool operator==(node_id_iterator j) const { return i == j.i; }
  /// Iterator inequality comparison
  bool operator!=(node_id_iterator j) const { return !(*this == j); }
  /// Dereferencing the iterator produces a SimplexID.
  Data operator*() { return Data(i->second); }
  /// Const version
  const Data operator*() const { return Data(i->second); }
  /// Dereferencing the iterator produces a SimplexID.
  typename super::pointer operator->() { return Data(i->second); }

protected:
  /// The iterator to wrap.
  Iter i;
};

/**
 * @brief      Convert an iterator into a node_id_iterator.
 *
 * @param[in]  j     The iterator to wrap
 *
 * @tparam     Iter  Typename of the iterator
 * @tparam     Data  Typename of the data
 *
 * @return     An iterator over NodeIDs.
 */
template <typename Iter, typename Data>
inline node_id_iterator<Iter, Data> make_node_id_iterator(Iter j) {
  return node_id_iterator<Iter, Data>(j);
}

/**
 * @brief      An iterator adapter to iterate over Node data.
 *
 * @tparam     Iter  Typename of the iterator
 * @tparam     Data  Typename of the data
 */
template <typename Iter, typename Data>
struct node_data_iterator
    : public std::iterator<std::bidirectional_iterator_tag, Data> {
public:
  /// Inherit from a bidirectional std::iterator.
  using super = std::iterator<std::bidirectional_iterator_tag, Data>;
  /// Empty constructor.
  node_data_iterator() {}
  /// Instantiate with an iterator to wrap.
  node_data_iterator(Iter j) : i(j) {}
  /// Increment the iterator
  node_data_iterator &operator++() {
    ++i;
    return *this;
  }
  /// Increment the iterator
  node_data_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }
  /// Decrement the iterator
  node_data_iterator &operator--() {
    --i;
    return *this;
  }
  /// Decrement the iterator
  node_data_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }
  /// Iterator comparison
  bool operator==(node_data_iterator j) const { return i == j.i; }
  /// Iterator inequality comparison
  bool operator!=(node_data_iterator j) const { return !(*this == j); }
  /// Dereferencing the iterator produces the data.
  typename super::reference operator*() { return i->second->_data; }
  /// Dereferencing the iterator produces the data.
  typename super::pointer operator->() { return i->second->_data; }

protected:
  /// The wrapped iterator.
  Iter i;
};

/**
 * @brief      Convert an iterator into a node_data_iterator.
 *
 * @param[in]  j     The iterator to wrap
 *
 * @tparam     Iter  Typename of the iterator
 * @tparam     Data  Typename of the data
 *
 * @return     An iterator over Node data.
 */
template <typename Iter, typename Data>
inline node_data_iterator<Iter, Data> make_node_data_iterator(Iter j) {
  return node_data_iterator<Iter, Data>(j);
}

/**
 * @brief      Helper to build a traits struct via expanding explicitly
 * specified
 *             traits from AbstractSimplicialComplex.
 *
 * @tparam     K     Typename for the KeyType
 * @tparam     Ts    Types of data to be stored on simplices.
 */
template <typename K, typename... Ts> struct simplicial_complex_traits_default {
  /// Template to assign ints for all levels.
  template <std::size_t k> using all_int = int;
  /// Alias for KeyType
  using KeyType = K;
  /// The typenames of the data to be stored on simplices.
  using NodeTypes = util::type_holder<Ts...>;
  /// Assign all_int type to all edges
  using EdgeTypes = typename util::int_type_map<
      std::size_t, util::type_holder,
      typename std::make_index_sequence<sizeof...(Ts) - 1>, all_int>::type;
};
} // end namespace detail
/// @endcond

/**
 * @class      simplicial_complex
 *
 * @brief      The CASC data structure for representing simplicial complexes of
 *             arbitrary dimensionality with coloring.
 *
 * You can create a CASC object by defining a struct containing the
 * traits of the complex. For example:
 * ~~~~~~~~~~~~~~~{.cpp}
 * struct complex_traits{
 *     using KeyType = int;
 *     using NodeTypes = util::type_holder<int,int,int,int>;
 *     using EdgeTypes = util::type_holder<int,int,int>;
 * };
 *
 * using SurfaceMesh = simplicial_complex<complex_traits>;
 * ~~~~~~~~~~~~~~~
 * This is the preferred method for creating a new CASC type. Alternatively you
 * can use the ::AbstractSimplicialComplex alias to build a struct for you.
 *
 * @tparam     traits  A struct defining the dimension of the complex and data
 *                     to be stored on each node and edge.
 */
template <typename traits> class simplicial_complex {
public:
  /// Typename of simplex keys.
  using KeyType = typename traits::KeyType;
  /// Typenames of the data stored on simplices.
  using NodeDataTypes = typename traits::NodeTypes;
  /// Typenames of the data stored on edges.
  using EdgeDataTypes = typename traits::EdgeTypes;
  /// Type of this
  using type_this = simplicial_complex<traits>;
  /// Total number of levels in the complex.
  static constexpr std::size_t numLevels = NodeDataTypes::size;
  /// Dimension of the simplicial complex.
  static constexpr std::size_t topLevel = numLevels - 1;
  /// Dimension of boundaries.
  static constexpr std::size_t bdryLevel = numLevels - 2;
  /// Index of all simplex dimensions in the complex.
  using LevelIndex = typename std::make_index_sequence<numLevels>;

private:
  /// Alias templated asc_node<...> as Node<k>
  template <std::size_t k>
  using Node =
      detail::asc_Node<KeyType, k, topLevel, NodeDataTypes, EdgeDataTypes>;
  /// Alias Node<k>* as NodePtr<k>
  template <std::size_t k> using NodePtr = Node<k> *;

public:
  /** Convenience alias for the user specified NodeData<k> typename */
  template <std::size_t k>
  using NodeData = typename util::type_get<k, NodeDataTypes>::type;
  /** Convenience alias for the user specified EdgeData<k> typename */
  template <std::size_t k>
  using EdgeData = typename util::type_get<k, EdgeDataTypes>::type;

  friend struct SimplexID; /**< SimplexID is a friend of
                              simplicial_complex */

  /**
   * @brief      A handle for a simplex object in the complex.
   *
   * SimplexID wraps a Node* for external handling. This way
   * the end users are never exposed to a raw pointer. For all general
   * purposes algorithms should use and pass SimplexIDs over raw pointers.
   *
   * @tparam     k     The Simplex dimension.
   */
  template <std::size_t k> struct SimplexID {
    /// Typename of the complex
    using complex = simplicial_complex<traits>;
    /// SimplexID is a friend of the complex
    friend simplicial_complex<traits>;
    /// The dimension of the simplex.
    static constexpr std::size_t level = k;

    /**
     * @brief      Default constructor wraps a nullptr.
     */
    SimplexID() : ptr(nullptr) {}

    /**
     * @brief      Constructor to wrap a NodePtr<k>.
     *
     * @param[in]  p     The NodePtr to wrap
     */
    SimplexID(NodePtr<k> p) : ptr(p) {}

    /**
     * @brief      Copy constructor.
     *
     * @param[in]  rhs   Another SimplexID to copy.
     */
    SimplexID(const SimplexID &rhs) : ptr(rhs.ptr) {}

    /// Assignment operator
    SimplexID &operator=(const SimplexID &rhs) {
      ptr = rhs.ptr;
      return *this;
    }

    /// Equality of wrapped pointers
    friend bool operator==(SimplexID lhs, SimplexID rhs) {
      return lhs.ptr == rhs.ptr;
    }
    /// Inequality of wrapped pointers
    friend bool operator!=(SimplexID lhs, SimplexID rhs) {
      return lhs.ptr != rhs.ptr;
    }
    /// Compare wrapped pointers
    friend bool operator<=(SimplexID lhs, SimplexID rhs) {
      return lhs.ptr <= rhs.ptr;
    }
    /// Compare wrapped pointers
    friend bool operator>=(SimplexID lhs, SimplexID rhs) {
      return lhs.ptr >= rhs.ptr;
    }
    /// Compare wrapped pointers
    friend bool operator<(SimplexID lhs, SimplexID rhs) {
      return lhs.ptr < rhs.ptr;
    }
    /// Compare wrapped pointers
    friend bool operator>(SimplexID lhs, SimplexID rhs) {
      return lhs.ptr > rhs.ptr;
    }

    /// Support casting to uintptr_t for hashing.
    explicit operator std::uintptr_t() const {
      return reinterpret_cast<std::uintptr_t>(ptr);
    }

    /// Dereferencing a SimplexID returns the data stored.
    complex::NodeData<k> const &operator*() const { return ptr->_data; }
    /// Dereferencing a SimplexID returns the data stored.
    complex::NodeData<k> &operator*() { return ptr->_data; }

    /// Get a handle to the stored data.
    complex::NodeData<k> const &data() const { return ptr->_data; }
    /// Get a handle to the stored data.
    complex::NodeData<k> &data() { return ptr->_data; }

    /**
     * @brief      Gets the name of a simplex as an std::Array.
     *
     * @param[in]  id    SimplexID of the simplex of interest.
     *
     * @return     Array containing the name of 'id'.
     */
    std::array<KeyType, k> indices() const {
      std::array<KeyType, k> s;
      std::size_t i = 0;
      for (auto curr : ptr->_down) {
        s[i++] = curr.first;
      }

      return s;
    }

    // Valid in C++17
    // TODO: (0) expose this to modern compilers
    // if constexpr (k < complex::topLevel){
    /**
     * @brief      Insert the coboundary keys of a simple into an inserter.
     *
     * @param[in]  pos       Iterator inserter
     *
     * @tparam     Inserter  Typename of the inserter.
     */
    template <class Inserter> void cover_insert(Inserter pos) const {
      for (auto curr : ptr->_up) {
        *pos++ = curr.first;
      }
    }

    /**
     * @brief      Get the coboundary keys of a simplex.
     *
     * @return     A vector of coboundary indices.
     */
    std::vector<KeyType> cover() const {
      std::vector<KeyType> rval;
      cover_insert(std::back_inserter(rval));
      return rval;
    }
    // }

    /**
     * @brief      Get a coboundary simplex
     *
     * @param[in]  s     Array of keys to follow
     *
     * @tparam     j     Number of keys
     *
     * @return     The simplex up
     */
    template <std::size_t j>
    SimplexID<k + j> get_simplex_up(const KeyType (&s)[j]) const {
      static_assert(k + j <= complex::topLevel,
                    "Cannot get simplex greater than the facets");
      return complex::get_recurse<k, j>::apply(s, this->ptr);
    }

    /**
     * @brief      Get a coboundary simplex
     *
     * @param[in]  arr   Array of keys to follow
     *
     * @tparam     j     Number of keys
     *
     * @return     The simplex up
     */
    template <std::size_t j>
    SimplexID<k + j> get_simplex_up(const std::array<KeyType, j> &arr) const {
      static_assert(k + j <= complex::topLevel,
                    "Cannot get simplex greater than the facets");
      return get_recurse<k, j>::apply(arr.data(), this->ptr);
    }

    /**
     * @brief      Convenience version of get_simplex_up when the name 's'
     *             consists of a single character.
     *
     * @param[in]  id    The identifier of a simplex.
     * @param[in]  s     The relative single character name of the desired
     *                   simplex.
     *
     * @tparam     i     The size of simplex 'id'.
     *
     * @return     SimplexID of node corresponding to \f$id\cup s\f$.
     */
    SimplexID<k + 1> get_simplex_up(const KeyType s) const {
      return get_recurse<k, 1>::apply(&s, this->ptr);
    }

    /**
     * @brief      Gets the simplex down.
     */
    template <std::size_t j>
    SimplexID<k - j> get_simplex_down(const KeyType (&s)[j]) const {
      return get_down_recurse<k, j>::apply(s, this->ptr);
    }

    /**
     * @brief      Gets the simplex down.
     */
    template <std::size_t j>
    SimplexID<k - j> get_simplex_down(const std::array<KeyType, j> &arr) const {
      return get_down_recurse<k, j>::apply(arr.data(), this->ptr);
    }

    /**
     * @brief      Gets the simplex down.
     */
    SimplexID<k - 1> get_simplex_down(const KeyType s) const {
      return get_down_recurse<k, 1>::apply(&s, this->ptr);
    }

    /**
     * @brief      Print the simplex as its name.
     *
     * @param      out   Handle to the stream
     * @param[in]  nid   SimplexID of interest
     *
     * @return     Handle to the stream
     *
     * Example
     * ~~~~~~~~~~~~~~~(.c)
     * mesh.insert<3>({0,1,2});
     * std::cout << s << std::endl;
     * s{0,1,2}"
     * ~~~~~~~~~~~~~~~
     */
    friend std ::ostream &operator<<(std::ostream &out, const SimplexID &nid) {
      // currently no such thing as static_if in c++ so we use a
      // template
      // helper
      out << "s{";
      print_helper<k, 0>::apply(out, nid);
      out << "}";
      return out;
    }

    // NOTE: Manually swap out these print functions for debugging if
    // desired.
    // /**
    //  * @brief      A full debug printout of of the node itself
    //  *
    //  * @param      out   Handle to the stream
    //  * @param[in]  nid   SimplexID of interest
    //  *
    //  * @return     Handle to the stream
    //  */
    // friend std::ostream& operator<<(std::ostream& out, const
    // SimplexID& nid){ out << *nid.ptr; return out; }

    // /**
    //  * @brief      Print the SimplexID as an ID.
    //  *
    //  * Example "0x7fd502402f10"
    //  *
    //  * @param      out   Handle to the stream
    //  * @param[in]  nid   Node of interest
    //  *
    //  * @return     Handle to the stream
    //  */
    // friend std ::ostream &operator<<(std::ostream &out, const
    // SimplexID &nid) { out << nid.ptr; return out; }

  private:
    /**
     * @brief      Base Case helper for printing SimplexIDs.
     *
     * @tparam     l     The Simplex dimension
     * @tparam     foo   Dummy argument to avoid explicit
     * specialization
     *                   in class scope
     */
    template <std::size_t l, std::size_t foo> struct print_helper {
      /**
       * @brief      Print out the name of the simplex.
       *
       * @param      out   Stream to pipe to.
       * @param[in]  nid   The simplex to print.
       *
       * @return     Handle to the output stream.
       */
      static std::ostream &apply(std::ostream &out, const SimplexID<l> &nid) {
        auto down = (*nid.ptr)._down;
        for (auto it = down.cbegin(); it != down.cend() - 1; ++it) {
          out << it->first << ",";
        }
        out << (down.cend() - 1)->first;
        return out;
      }
    };

    /**
     * @brief      Explicit specialization to print 0-Simplices
     *
     * @tparam     foo   Dummy argument to avoid explicit
     * specialization
     *                   in class scope
     */
    template <std::size_t foo> struct print_helper<0, foo> {
      /**
       * @brief      Print the root simplex
       *
       * @param      out   Stream to print to.
       * @param[in]  nid   The simplex to print.
       *
       * @return     Handle to the output stream.
       */
      static std::ostream &apply(std::ostream &out, const SimplexID &nid) {
        out << "root " << nid;
        return out;
      }
    };
    /// The wrapped pointer.
    NodePtr<k> ptr;
  };

  friend struct EdgeID; /**< EdgeID is a friend to simplicial_complex */

  /**
   * @brief      External reference to an edge or a connection within the
   *             complex.
   *
   * @tparam     k     The edge connects a simplex of size k-1 to a
   *                   simplex of size k.
   */
  template <std::size_t k> struct EdgeID {
    /// Typename of the complex
    using complex = simplicial_complex<traits>;
    /// EdgeID is a friend of the complex
    friend simplicial_complex<traits>;
    /// The dimension of the simplex which the edge points to.
    static constexpr std::size_t level = k;

    /**
     * @brief      Default contstructor wraps a nullptr and dummy edge.
     */
    EdgeID() : ptr(nullptr), edge(0) {}

    /**
     * @brief      Constructor to wrap an Edge.
     *
     * @param[in]  p     Pointer to the next Node.
     * @param[in]  e     Key of the edge
     */
    EdgeID(NodePtr<k> p, KeyType e) : ptr(p), edge(e) {}

    /**
     * @brief      Copy constructor
     *
     * @param[in]  rhs   The right hand side
     */
    EdgeID(const EdgeID &rhs) : ptr(rhs.ptr), edge(rhs.edge) {}

    /// Assignment operator
    EdgeID &operator=(const EdgeID &rhs) {
      ptr = rhs.ptr;
      edge = rhs.edge;
      return *this;
    }

    /// Equality of wrapped pointers and edges
    friend bool operator==(EdgeID lhs, EdgeID rhs) {
      return lhs.ptr == rhs.ptr && lhs.edge == rhs.edge;
    }
    /// Compare wrapped pointers and edges.
    friend bool operator!=(EdgeID lhs, EdgeID rhs) { return !(lhs == rhs); }
    /// Compare wrapped pointers and edges.
    friend bool operator<=(EdgeID lhs, EdgeID rhs) {
      return lhs < rhs || lhs == rhs;
    }
    /// Compare wrapped pointers and edges.
    friend bool operator>=(EdgeID lhs, EdgeID rhs) {
      return lhs > rhs || lhs == rhs;
    }
    /// Less than defines an ordering of key types on the edges.
    friend bool operator<(EdgeID lhs, EdgeID rhs) {
      return (lhs.ptr < rhs.ptr) || (lhs.ptr == rhs.ptr && lhs.edge < rhs.edge);
    }
    /// Greater than comparison
    friend bool operator>(EdgeID lhs, EdgeID rhs) { return rhs < lhs; }

    // explicit operator std::size_t () const { return
    // static_cast<std::size_t>(ptr);

    /// Dereferencing an EdgeID gets the data on the edge.
    auto const &operator*() const { return data(); }
    /// Dereferencing an EdgeID gets the data on the edge.
    auto &operator*() { return data(); }

    /// Get the key of the edge.
    KeyType key() const { return edge; }

    /// Return the data stored on the edge.
    auto const &data() const { return ptr->_edge_data[edge]; }
    /// Return the data stored on the edge.
    auto &data() { return ptr->_edge_data[edge]; }

    /**
     * @brief      Get the coboundary simplex.
     *
     * @return     SimplexID of the simplex above the edge.
     */
    SimplexID<k> up() const { return ptr; }

    /**
     * @brief      Get the simplex below.
     *
     * @return     SimplexID of the simplex below the edge.
     */
    SimplexID<k - 1> down() const { return SimplexID<k - 1>(ptr->_down[edge]); }

  private:
    /// Pointer to the next node.
    NodePtr<k> ptr;
    /// The Key of the edge.
    KeyType edge;
  };

  /**
   * @brief      Default constructor
   */
  simplicial_complex() : node_count(0) {
    for (auto &x : level_count) // Initialize level_count to 0 for all
                                // levels
    {
      x = 0;
    }
    // Create a root node
    _root = create_node<0>();
  }

  /**
   * @brief      Destruct the simplicial complex.
   *
   * Recursively go over the simplices and remove them prior to
   * destructing
   * the CASC object itself.
   */
  ~simplicial_complex() {
    std::size_t count;
    remove_recurse<0, 0>::apply(this, &_root, &_root + 1, count);
  }

  /**
   * @brief      Insert a simplex and all sub-simplices into the complex.
   *
   * Example -- insert the simplex {1,2,3}:
   * ~~~~~~~~~~~~~~~{.cpp}
   * mesh.insert<3>({1,2,3});
   * ~~~~~~~~~~~~~~~
   *
   * @param[in]  s     A C style array of vertices of simplex 's'.
   *
   * @tparam     n     Dimension of simplex 's'.
   */
  template <std::size_t n> SimplexID<n> insert(const KeyType (&s)[n]) {
    for (const KeyType *p = s; p < s + n; ++p) {
      unused_vertices.remove(*p);
    }
    return insert_full<0, n>::apply(this, _root, s);
  }

  /**
   * @brief      Insert a simplex and all sub-simplices into the complex
   *             along with data.
   *
   * Example -- insert the simplex {1,2,3} with data:
   * ~~~~~~~~~~~~~~~{.cpp}
   * mesh.insert<3>({1,2,3}, 5);
   * ~~~~~~~~~~~~~~~
   *
   * @param[in]  s     A C style array of vertices of simplex 's'.
   * @param[in]  data  The data to be stored at the simplex 's'.
   *
   * @tparam     n     Dimension of simplex 's'.
   */
  template <std::size_t n>
  SimplexID<n> insert(const KeyType (&s)[n], const NodeData<n> &data) {
    for (const KeyType *p = s; p < s + n; ++p) {
      unused_vertices.remove(*p);
    }
    Node<n> *rval = insert_full<0, n>::apply(this, _root, s);
    rval->_data = data;
    return rval;
  }

  /**
   * @brief      Insert a simplex named and all sub-simplices into the
   * complex.
   *
   * @param[in]  s     Array of vertices comprising 's'.
   *
   * @tparam     n     Dimension of simplex 's'.
   */
  template <std::size_t n>
  SimplexID<n> insert(const std::array<KeyType, n> &s) {
    for (KeyType x : s) {
      unused_vertices.remove(x);
    }
    return insert_full<0, n>::apply(this, _root, s.data());
  }

  /**
   * @brief      Insert a simplex and all sub-simplices into the complex
   *             along with data.
   *
   * @param[in]  s     Array of vertices comprising 's'.
   * @param[in]  data  The data to be stored at the simplex 's'.
   *
   * @tparam     n     Dimension of simplex 's'.
   */
  template <std::size_t n>
  SimplexID<n> insert(const std::array<KeyType, n> &s,
                      const NodeData<n> &data) {
    for (KeyType x : s) {
      unused_vertices.remove(x);
    }
    Node<n> *rval = insert_full<0, n>::apply(this, _root, s.data());
    rval->_data = data;
    return rval;
  }

  /**
   * @brief      Add a new vertex to the complex.
   *
   * A list of currently unused indices are tracked using a B-tree. This
   * function retrieves a currently unused index and creates a new vertex
   * while returning the new key.
   *
   * @return     The key of the new vertex.
   */
  KeyType add_vertex() {
    KeyType v[1] = {unused_vertices.pop()};
    insert<1>(v);
    return v[0];
  }

  /**
   * @brief      Add a new vertex to the complex with data.
   *
   * @return     The key of the new vertex.
   */
  KeyType add_vertex(const NodeData<1> &data) {
    KeyType v[1] = {unused_vertices.pop()};
    insert<1>(v, data);
    return v[0];
  }

  /**
   * @brief      Apply a lambda function the name of a simplex.
   *
   * @param[in]  id      SimplexID of the simplex of interest.
   * @param[in]  fn      Lambda function to apply to the name of 'id'.
   *
   * @tparam     n       Dimension of simplex 'id'.
   * @tparam     Lambda  Functor which supports operator(KeyType).
   */
  template <std::size_t n, typename Lambda>
  void get_name(SimplexID<n> id, Lambda fn) const {
    for (auto curr : id.ptr->_down) {
      fn(curr.first);
    }
  }

  /**
   * @brief      Gets the name of a simplex as an std::Array.
   *
   * @param[in]  id    SimplexID of the simplex of interest.
   *
   * @tparam     n     Size of the simplex referenced by 'id'.
   *
   * @return     Array containing the name of 'id'.
   */
  template <std::size_t n>
  std::array<KeyType, n> get_name(SimplexID<n> id) const {
    std::array<KeyType, n> s;
    std::size_t i = 0;
    for (auto curr : id.ptr->_down) {
      s[i++] = curr.first;
    }
    assert(i == n);
    return s;
  }

  /**
   * @brief      Gets the name of a simplex.
   *
   * This is the explicit specialization which handles the empty set
   * simplex.
   *
   * @param[in]  id    SimplexID of the simplex of interest.
   *
   * @return     Array containing the name of 'id'.
   */
  std::array<KeyType, 0> get_name(SimplexID<0>) const {
    std::array<KeyType, 0> name{};
    return name;
  }

  /**
   * @brief      Gets the simplex with name 's'.
   *
   * @param[in]  s     Name of the simplex to find.
   *
   * @tparam     n     Dimension of simplex s.
   *
   * @return     SimplexID of node corresponding to 's'.
   */
  template <std::size_t n>
  SimplexID<n> get_simplex_up(const KeyType (&s)[n]) const {
    return get_recurse<0, n>::apply(s, _root);
  }

  template <std::size_t n>
  SimplexID<n> get_simplex_up(const std::array<KeyType, n> &arr) const {
    return get_recurse<0, n>::apply(arr.data(), _root);
  }

  /**
   * @brief      Get the simplex identifier which has the name 's'
   *             relative to the simplex 'id'.
   *
   * @param[in]  id    The identifier of a simplex.
   * @param[in]  s     The relative name of the desired simplex.
   *
   * @tparam     i     The size of simplex 'id'.
   * @tparam     j     The length of the name 's'.
   *
   * @return     SimplexID of node corresponding to \f$id\cup s\f$.
   */
  template <std::size_t i, std::size_t j>
  SimplexID<i + j> get_simplex_up(const SimplexID<i> id,
                                  const KeyType (&s)[j]) const {
    return get_recurse<i, j>::apply(s, id);
  }

  template <std::size_t i, std::size_t j>
  SimplexID<i + j> get_simplex_up(const SimplexID<i> id,
                                  const std::array<KeyType, j> &arr) const {
    return get_recurse<i, j>::apply(arr.data(), id);
  }

  /**
   * @brief      Convenience version of get_simplex_up when the name 's'
   *             consists of a single character.
   *
   * @param[in]  id    The identifier of a simplex.
   * @param[in]  s     The relative single character name of the desired
   *                   simplex.
   *
   * @tparam     i     The size of simplex 'id'.
   *
   * @return     SimplexID of node corresponding to \f$id\cup s\f$.
   */
  template <std::size_t i>
  SimplexID<i + 1> get_simplex_up(const SimplexID<i> id,
                                  const KeyType s) const {
    return get_recurse<i, 1>::apply(&s, id.ptr);
  }

  /**
   * @brief      Get the root simplex.
   *
   * @return     The root simplex.
   */
  SimplexID<0> get_simplex_up() const { return _root; }

  /**
   * @brief      Get the sub-simplex of the simplex 'id' which does not
   *             have 's' in the name.
   *
   * @param[in]  id    The identifier of a simplex.
   * @param[in]  s     The relative name of the desired simplex.
   *
   * @tparam     i     The size of simplex 'id'.
   * @tparam     j     The length of the name 's'
   *
   * @return     The node down.
   */
  template <std::size_t i, std::size_t j>
  SimplexID<i - j> get_simplex_down(const SimplexID<i> id,
                                    const KeyType (&s)[j]) const {
    return get_down_recurse<i, j>::apply(s, id.ptr);
  }

  template <std::size_t i, std::size_t j>
  SimplexID<i - j> get_simplex_down(const SimplexID<i> id,
                                    const std::array<KeyType, j> &arr) const {
    return get_down_recurse<i, j>::apply(arr.data(), id.ptr);
  }

  /**
   * @brief      Convenience version of get_simplex_down when the name 's'
   *             consists of a single character.
   *
   * @param[in]  id    The identifier of a simplex.
   * @param[in]  s     The relative single character name of the desired
   *                   simplex.
   *
   * @tparam     i     The size of simplex 'id'.
   *
   * @return     The node down.
   */
  template <std::size_t i>
  SimplexID<i - 1> get_simplex_down(const SimplexID<i> id,
                                    const KeyType s) const {
    return get_down_recurse<i, 1>::apply(&s, id.ptr);
  }

  /**
   * @brief      Get the root simplex.
   *
   * @return     The root simplex.
   */
  SimplexID<0> get_simplex_down() const { return _root; }

  /**
   * @brief      Insert the coboundary keys of a simple into an inserter.
   *
   * @param[in]  id        The identifier of a simplex.
   * @param[in]  pos       Iterator inserter
   *
   * @tparam     k         The dimension of the simplex.
   * @tparam     Inserter  Typename of the inserter.
   */
  template <std::size_t k, class Inserter>
  void get_cover_insert(const SimplexID<k> id, Inserter pos) const {
    for (auto curr : id.ptr->_up) {
      *pos++ = curr.first;
    }
  }

  /**
   * @brief      Apply a lambda function to the coboundary keys.
   *
   * @param[in]  id      The identifier
   * @param[in]  fn      The function
   *
   * @tparam     k       The dimension of the simplex.
   * @tparam     Lambda  Typename of a functor which supports
   * operator(KeyType).
   */
  template <std::size_t k, class Lambda>
  void get_cover(const SimplexID<k> id, Lambda fn) const {
    for (auto curr : id.ptr->_up) {
      fn(curr.first);
    }
  }

  /**
   * @brief      Get the coboundary keys of a simplex.
   *
   * @param[in]  id    The identifier of a simplex.
   *
   * @tparam     k     The dimension of the simplex.
   *
   * @return     A vector of coboundary indices.
   */
  template <std::size_t k>
  std::vector<KeyType> get_cover(const SimplexID<k> id) const {
    std::vector<KeyType> rval;
    get_cover_insert(id, std::back_inserter(rval));
    return rval;
  }

  /**
   * @brief      Get the coboundary of a set of simplices.
   *
   * @param      simplices  The set of simplices
   *
   * @tparam     k          The dimension of the simplices.
   *
   * @return     The set of coboundary simplices.
   */
  template <std::size_t k>
  std::set<SimplexID<k + 1>>
  up(const std::set<SimplexID<k>> &&simplices) const {
    std::set<SimplexID<k + 1>> rval;
    for (auto simplex : simplices) {
      for (auto p : simplex.ptr->_up) {
        rval.insert(SimplexID<k + 1>(p.second));
      }
    }
    return rval;
  }

  /**
   * @brief      Get the coboundary of a set of simplices.
   *
   * @param      simplices  The set of simplices
   *
   * @tparam     k          The dimension of the simplices.
   *
   * @return     The set of coboundary simplices.
   */
  template <std::size_t k>
  std::set<SimplexID<k + 1>> up(const std::set<SimplexID<k>> &simplices) const {
    std::set<SimplexID<k + 1>> rval;
    for (auto simplex : simplices) {
      for (auto p : simplex.ptr->_up) {
        rval.insert(SimplexID<k + 1>(p.second));
      }
    }
    return rval;
  }

  /**
   * @brief      Get the coboundary of a simplex.
   *
   * @param      nid   The simplex of interest
   *
   * @tparam     k     The dimension of the simplex.
   *
   * @return     Set of (k+1)-simplices of which 'nid' is a face of.
   */
  template <std::size_t k>
  std::set<SimplexID<k + 1>> up(const SimplexID<k> nid) const {
    std::set<SimplexID<k + 1>> rval;
    for (auto p : nid.ptr->_up) {
      rval.insert(SimplexID<k + 1>(p.second));
    }
    return rval;
  }

  template <std::size_t k, class InsertIter>
  void up(const std::set<SimplexID<k>> &&simplices, InsertIter iter) const {
    for (auto simplex : simplices) {
      for (auto p : simplex.ptr->_up) {
        *iter++ = SimplexID<k + 1>(p.second);
      }
    }
  }

  template <std::size_t k, class InsertIter>
  void up(const std::set<SimplexID<k>> &simplices, InsertIter iter) const {
    for (auto simplex : simplices) {
      for (auto p : simplex.ptr->_up) {
        *iter++ = SimplexID<k + 1>(p.second);
      }
    }
  }

  template <std::size_t k, class InsertIter>
  void up(const SimplexID<k> simplex, InsertIter iter) const {
    for (auto p : simplex.ptr->_up) {
      *iter++ = SimplexID<k + 1>(p.second);
    }
  }

  /**
   * @brief      Get the boundary of a set of simplices.
   *
   * @param      simplices  The set of simplicies.
   *
   * @tparam     k      The dimension of the simplices.
   *
   * @return     The set of boundary simplices.
   */
  template <std::size_t k>
  std::set<SimplexID<k - 1>>
  down(const std::set<SimplexID<k>> &&simplices) const {
    std::set<SimplexID<k - 1>> rval;
    for (auto nid : simplices) {
      for (auto p : nid.ptr->_down) {
        rval.insert(SimplexID<k - 1>(p.second));
      }
    }
    return rval;
  }

  /**
   * @brief      Get the boundary of a set of simplices.
   *
   * @param      simplices  The set of simplicies.
   *
   * @tparam     k      The dimension of the simplices.
   *
   * @return     The set of boundary simplices.
   */
  template <std::size_t k>
  std::set<SimplexID<k - 1>>
  down(const std::set<SimplexID<k>> &simplices) const {
    std::set<SimplexID<k - 1>> rval;
    for (auto simplex : simplices) {
      for (auto p : simplex.ptr->_down) {
        rval.insert(SimplexID<k - 1>(p.second));
      }
    }
    return rval;
  }

  /**
   * @brief      Get the boundary of a simplex.
   *
   * @param      simplex   The simplex of interest.
   *
   * @tparam     k     The dimension of the simplex.
   *
   * @return     Set of (k-1)-simplices of which 'simplex' is a coface of.
   */
  template <std::size_t k>
  std::set<SimplexID<k - 1>> down(const SimplexID<k> simplex) const {
    std::set<SimplexID<k - 1>> rval;
    for (auto p : simplex.ptr->_down) {
      rval.insert(SimplexID<k - 1>(p.second));
    }
    return rval;
  }

  template <std::size_t k, class InsertIter>
  void down(const std::set<SimplexID<k>> &&simplices, InsertIter iter) const {
    for (auto simplex : simplices) {
      for (auto p : simplex.ptr->_down) {
        *iter++ = SimplexID<k - 1>(p.second);
      }
    }
  }

  template <std::size_t k, class InsertIter>
  void down(const std::set<SimplexID<k>> &simplices, InsertIter iter) const {
    for (auto simplex : simplices) {
      for (auto p : simplex.ptr->_down) {
        *iter++ = SimplexID<k - 1>(p.second);
      }
    }
  }

  template <std::size_t k, class InsertIter>
  void down(const SimplexID<k> simplex, InsertIter iter) const {
    for (auto p : simplex.ptr->_down) {
      *iter++ = SimplexID<k - 1>(p.second);
    }
  }

  /**
   * @brief      Gets the edge up from a simplex.
   *
   * @param[in]  simplex   The simplex of interest.
   * @param[in]  a     Key of the edge to get.
   *
   * @tparam     k     The level of the simplex of interest
   *
   * @return     The edge up.
   */
  template <std::size_t k>
  EdgeID<k + 1> get_edge_up(SimplexID<k> simplex, KeyType a) {
    return EdgeID<k + 1>(simplex.ptr->_up.at(a), a);
  }

  /**
   * @brief      Gets the edge down from a simplex.
   *
   * @param[in]  simplex   The simplex of interest.
   * @param[in]  a     Key of the edge to get.
   *
   * @tparam     k     The level of the simplex of interest
   *
   * @return     The edge down.
   */
  template <std::size_t k>
  EdgeID<k> get_edge_down(SimplexID<k> simplex, KeyType a) {
    return EdgeID<k>(simplex.ptr, a);
  }

  /**
   * @brief      Gets the edge up from a simplex.
   *
   * @param[in]  simplex    The simplex of interest.
   * @param[in]  a      Key of the edge to get.
   *
   * @tparam     k      The level of the simplex of interest
   *
   * @return     The edge up.
   */
  template <std::size_t k>
  EdgeID<k + 1> get_edge_up(SimplexID<k> simplex, KeyType a) const {
    return EdgeID<k + 1>(simplex.ptr->_up.at(a), a);
  }

  /**
   * @brief      Gets the edge down from a simplex.
   *
   * @param[in]  simplex   The simplex of interest.
   * @param[in]  a     Key of the edge to get.
   *
   * @tparam     k     The level of the simplex of interest
   *
   * @return     The edge down.
   */
  template <std::size_t k>
  EdgeID<k> get_edge_down(SimplexID<k> simplex, KeyType a) const {
    return EdgeID<k>(simplex.ptr, a);
  }

  /**
   * @brief      Check whether a simplex with some name exists.
   *
   * @param[in]  s     C-style array of the name
   *
   * @tparam     k     The dimension of the simplex.
   *
   * @return     True if the simplex is in the complex.
   */
  template <std::size_t k> bool exists(const KeyType (&s)[k]) const {

    return get_recurse<0, k>::apply(s, _root) != nullptr;
  }

  /**
   * @brief      Get the number of simplices of dimension 'k'.
   *
   * @tparam     k     The dimension of interest.
   *
   * @return     Integer number of k-simplices in the complex.
   */
  template <std::size_t k> std::size_t size() const {
    return std::get<k>(levels).size();
  }

  // template <std::size_t k> using SimplexIDIterator =
  // detail::node_id_iterator<typename detail::map<NodePtr<k>>::iterator,
  // SimplexID<k>>;
  /**
   * @brief      Create an iterator to traverse the SimplexIDs of a
   *             dimension.
   *
   * @tparam     k     The simplex dimension to traverse.
   *
   * @return     An iterator across all k-simplices of the complex.
   */
  template <std::size_t k> auto get_level_id() {
    auto begin = std::get<k>(levels).begin();
    auto end = std::get<k>(levels).end();
    auto data_begin =
        detail::make_node_id_iterator<decltype(begin), SimplexID<k>>(begin);
    auto data_end =
        detail::make_node_id_iterator<decltype(end), SimplexID<k>>(end);
    return util::make_range(data_begin, data_end);
  }

  /**
   * @brief      Create an iterator to traverse the SimplexIDs of a
   *             dimension.
   *
   * @tparam     k     The simplex dimension to traverse.
   *
   * @return     An iterator across all k-simplices of the complex.
   */
  template <std::size_t k> auto get_level_id() const {
    auto begin = std::get<k>(levels).cbegin();
    auto end = std::get<k>(levels).cend();
    auto data_begin =
        detail::make_node_id_iterator<decltype(begin), const SimplexID<k>>(
            begin);
    auto data_end =
        detail::make_node_id_iterator<decltype(end), const SimplexID<k>>(end);
    return util::make_range(data_begin, data_end);
  }

  // template <std::size_t k> using DataIterator =
  // detail::node_data_iterator<typename std::map<std::size_t,
  // NodePtr<k>>::iterator, NodeData<k>>;
  /**
   * @brief      Create an iterator to traverse the simplex data of a
   *             dimension.
   *
   * @tparam     k     The simplex dimension to traverse.
   *
   * @return     An iterator across the data of all k-simplices in the
   * complex.
   */
  template <std::size_t k> auto get_level() {
    auto begin = std::get<k>(levels).begin();
    auto end = std::get<k>(levels).end();
    auto data_begin =
        detail::make_node_data_iterator<decltype(begin), NodeData<k>>(begin);
    auto data_end =
        detail::make_node_data_iterator<decltype(end), NodeData<k>>(end);
    return util::make_range(data_begin, data_end);
  }

  /**
   * @brief      Create an iterator to traverse the simplex data of a
   *             dimension.
   *
   * @tparam     k     The simplex dimension to traverse.
   *
   * @return     An iterator across the data of all k-simplices in the
   * complex.
   */
  template <std::size_t k> auto get_level() const {
    auto begin = std::get<k>(levels).cbegin();
    auto end = std::get<k>(levels).cend();
    auto data_begin =
        detail::make_node_data_iterator<decltype(begin), const NodeData<k>>(
            begin);
    auto data_end =
        detail::make_node_data_iterator<decltype(end), const NodeData<k>>(end);
    return util::make_range(data_begin, data_end);
  }

  /**
   * @brief      Remove a simplex and all dependent simplices by name.
   *
   * @param[in]  s     C-style array with the name of the simplex to
   *                   remove.
   *
   * @tparam     k     The dimension of the simplex.
   *
   * @return     Integer corresponding to the number of simplices removed.
   */
  template <std::size_t k> std::size_t remove(const KeyType (&s)[k]) {
    Node<k> *root = get_recurse<0, k>::apply(s, _root);
    std::size_t count = 0;
    return remove_recurse<k, 0>::apply(this, &root, &root + 1, count);
  }

  /**
   * @brief      Remove a simplex and all dependent simplices by name.
   *
   * @param[in]  s     std::array with the name of the simplex to remove.
   *
   * @tparam     k     The dimension of the simplex.
   *
   * @return     Integer corresponding to the number of simplices removed.
   */
  template <std::size_t k> std::size_t remove(const std::array<KeyType, k> &s) {
    Node<k> *root = get_recurse<0, k>::apply(s.data(), _root);
    std::size_t count = 0;
    return remove_recurse<k, 0>::apply(this, &root, &root + 1, count);
  }

  /**
   * @brief      Remove a simplex and all dependent simplices by
   *             SimplexID.
   *
   * @param[in]  s     SimplexID of the simplex to remove.
   *
   * @tparam     k     The dimension of the simplex.
   *
   * @return     Integer corresponding to the number of simplices removed.
   */
  template <std::size_t k> std::size_t remove(SimplexID<k> s) {
    std::size_t count = 0;
    return remove_recurse<k, 0>::apply(this, &s.ptr, &s.ptr + 1, count);
  }


  /**
   * @brief      Checks whether a simplex is near a boundary.
   *
   * @param[in]  s      SimplexID of interest
   *
   * @tparam     level  Dimension of the simplex
   *
   * @return     True if the simplex or any subsimplices are onBoundary.
   */
  template <std::size_t level>
  bool nearBoundary(const SimplexID<level> s) const {
    return nearBoundaryH<level, 0>::apply(s);
  }

  /**
   * @brief      Checks whether a simplex is on a boundary.
   *
   * @param[in]  s     SimplexID of interest
   *
   * @tparam     k     Dimension of the simplex
   *
   * @return     True if the simplex is a member of a topLevel-1 simplex
   *             on the boundary or if the simplex is on a boundary or if
   *             the simplex is a coboundary of a boundary topLevel-1
   *             simplex.
   */
  template <std::size_t k> bool onBoundary(const SimplexID<k> s) const {
    return onBoundaryH<k, 0>::apply(s);
  }

  //** Reintroduce this code block when this is resolved
  // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#727

  // /**
  //  * @brief      Checks whether a simplex is on a boundary.
  //  *
  //  * @param[in]  s     SimplexID of interest
  //  *
  //  * @tparam     k     Dimension of the simplex
  //  *
  //  * @return     True if the simplex interacts with a
  //  *             topLevel-1 simplex which is on a boundary.
  //  */
  // template <std::size_t k>
  // bool onBoundary(const SimplexID<k> s) const
  // {
  //     for(auto p : s.ptr->_up)
  //     {
  //         if(onBoundary(SimplexID<k+1>(p.second)))
  //             return true;
  //     }
  //     return false;
  // }

  // /**
  //  * @brief      Specialization of the facets
  //  *
  //  * @param[in]  s     SimplexID of interest
  //  *
  //  * @tparam     k     Dimension of the simplex
  //  *
  //  * @return     True if s is on a boundary
  //  */
  // template<>
  // bool onBoundary(const SimplexID<topLevel> s) const
  // {
  //     for(auto p : s.ptr->_down){
  //         if(onBoundary(SimplexID<topLevel-1>(p.second)))
  //             return true;
  //     }
  //     return false;
  // }

  // /**
  //  * @brief      Specialization of the topLevel-1 simplices
  //  *
  //  * @param[in]  s     SimplexID of interest
  //  *
  //  * @tparam     k     Dimension of the simplex
  //  *
  //  * @return     True if s is on a boundary
  //  */
  // template<>
  // bool onBoundary(const SimplexID<topLevel-1> s) const
  // {
  //     return s.ptr->_up.size() != 2;
  // }

  /**
   * @brief      Less than or equal to comparison operator of two
   *             SimplexIDs.
   *
   * @param[in]  lhs   The left hand side
   * @param[in]  rhs   The right hand side
   *
   * @tparam     L     Dimension of lhs simplex.
   * @tparam     R     Dimension of rhs simplex.
   *
   * @return     True if lhs is rhs or a proper face of rhs.
   */
  template <std::size_t L, std::size_t R>
  bool leq(SimplexID<L> lhs, SimplexID<R> rhs) const {
    auto name_lhs = get_name(lhs);
    auto name_rhs = get_name(rhs);

    std::size_t i = 0;
    for (std::size_t j = 0; i < L && j < R; ++j) {
      if (name_lhs[i] == name_rhs[j]) {
        ++i;
      }
    }
    return (i == L);
  }

  /**
   * @brief      Equality comparison of two simplices.
   *
   * @param[in]  lhs   The left hand side
   * @param[in]  rhs   The right hand side
   *
   * @tparam     L     Dimension of lhs simplex.
   * @tparam     R     Dimension of rhs simplex.
   *
   * @return     Always false as L != R. The L==R case is overloaded by
   *             partial specialization.
   */
  template <std::size_t L, std::size_t R>
  bool eq(SimplexID<L>, SimplexID<R>) const {
    return false;
  }

  /**
   * @brief      Equality comparison of two simplices.
   *
   * @param[in]  lhs   The left hand side
   * @param[in]  rhs   The right hand side
   *
   * @tparam     k     Dimension of the simplices.
   *
   * @return     True if the names are the same.
   */
  template <std::size_t k> bool eq(SimplexID<k> lhs, SimplexID<k> rhs) const {
    auto name_lhs = get_name(lhs);
    auto name_rhs = get_name(rhs);

    for (std::size_t i = 0; i < k; ++i) {
      if (name_lhs[i] != name_rhs[i]) {
        return false;
      }
    }
    return true;
  }

  /**
   * @brief      Less than comparison of simplices.
   *
   * @param[in]  lhs   The left hand side
   * @param[in]  rhs   The right hand side
   *
   * @tparam     L     Dimension of lhs simplex.
   * @tparam     R     Dimension of rhs simplex.
   *
   * @return     True if lhs is a proper subface of rhs.
   */
  template <std::size_t L, std::size_t R>
  bool lt(SimplexID<L> lhs, SimplexID<R> rhs) const {
    return L < R && leq(lhs, rhs);
  }

private:
  /**
   * @brief      Base case for checking if simplex is near a boundary
   *
   * @tparam     level  Dimension of the simplex
   * @tparam     foo    Dummy argument to avoid explicit specialization in
   *                    class scope
   */
  template <std::size_t level, std::size_t foo> struct nearBoundaryH {
    static bool apply(const SimplexID<level> s) {
      auto name = s.indices();
      KeyType down[level - 1];

      for (std::size_t i = 0; i < level; ++i) {
        std::size_t k = 0;
        for (std::size_t j = 0; j < level; ++j) {
          if (i != j) {
            down[k++] = name[j];
          }
        }
        if (onBoundaryH<1, 0>::apply(
                get_down_recurse<level, level - 1>::apply(down, s.ptr)))
          return true;
      }
      return false;
    }
  };

  /**
   * @brief      Specialization of vertices
   *
   * @tparam     foo    Dummy argument to avoid explicit specialization in
   *                    class scope
   */
  template <std::size_t foo> struct nearBoundaryH<1, foo> {
    static bool apply(const SimplexID<1> s) {
      if (onBoundaryH<1, 0>::apply(s))
        return true;
      return false;
    }
  };

  /**
   * @brief      Base case for checking if simplex is on a boundary
   *
   * @tparam     level  Dimension of the simplex
   * @tparam     foo    Dummy argument to avoid explicit specialization in
   *                    class scope
   */
  template <std::size_t level, std::size_t foo> struct onBoundaryH {
    /**
     * @brief      Recurse up complex to find boundary.
     *
     * @param[in]  s     Simplex of interest
     *
     * @return     True if on boundary
     */
    static bool apply(const SimplexID<level> s) {
      for (auto p : s.ptr->_up) {
        if (onBoundaryH<level + 1, foo>::apply(SimplexID<level + 1>(p.second)))
          return true;
      }
      return false;
    }
  };

  /**
   * @brief      Specialization for if facets are on boundary.
   *
   * @tparam     foo   Dummy argument to avoid explicit specialization in
   *                   class scope}
   */
  template <std::size_t foo> struct onBoundaryH<topLevel, foo> {
    /**
     * @brief      Check if a face is on a boundary
     *
     * @param[in]  s     SimplexID<topLevel> of interest
     *
     * @return     True if a member SimplexID<topLevel-1> is a boundary.
     */
    static bool apply(const SimplexID<topLevel> s) {
      for (auto p : s.ptr->_down) {
        if (onBoundaryH<topLevel - 1, foo>::apply(
                SimplexID<topLevel - 1>(p.second)))
          return true;
      }
      return false;
    }
  };

  /**
   * @brief      Specialization for topLevel-1 simplices
   *
   * @tparam     foo   Dummy argument to avoid explicit specialization in
   *                   class scope}
   */
  template <std::size_t foo> struct onBoundaryH<bdryLevel, foo> {
    /**
     * @brief      Check if SimplexID<topLevel-1> is on a boundary
     *
     * @param[in]  s     SimplexID of interest
     *
     * @return     True if simplex has less than 2 coboundary faces.
     */
    static bool apply(const SimplexID<bdryLevel> s) {
      return s.ptr->_up.size() < 2;
    }
  };

  /**
   * @brief      Base case for recursively deleting simplices.
   *
   * @tparam     level  Simplex dimension to operate at.
   * @tparam     foo    Dummy argument to avoid explicit specialization in
   *                    class scope
   */
  template <std::size_t level, std::size_t foo> struct remove_recurse {
    /**
     * @brief      Recursively remove simplices.
     *
     * @param      that   The CASC object
     * @param[in]  begin  Iterator to beginning of the set of simplices
     *                    to remove.
     * @param[in]  end    Iterator to the end of the set.
     * @param      count  Number of simplices removed already.
     *
     * @tparam     T      Typename of the iterator.
     *
     * @return     Recurse to the next level and remove coboundary
     *             simplices.
     */
    template <typename T>
    static std::size_t apply(type_this *that, T begin, T end,
                             std::size_t &count) {
      std::set<Node<level + 1> *> next;
      // for each node of interest...
      for (auto i = begin; i != end; ++i) {
        auto up = (*i)->_up;
        for (auto j = up.begin(); j != up.end(); ++j) {
          next.insert(j->second);
        }
        that->remove_node(*i);
        ++count;
      }
      return remove_recurse<level + 1, foo>::apply(that, next.begin(),
                                                   next.end(), count);
    }
  };

  /**
   * @brief      Terminal condition for remove_recurse.
   *
   * @tparam     foo   Dummy argument to avoid explicit specialization in
   *                   class scope
   */
  template <std::size_t foo> struct remove_recurse<topLevel, foo> {
    /**
     * @brief      Remove the facets of the complex.
     *
     * @param      that   The CASC object
     * @param[in]  begin  Iterator to beginning of the set of simplices
     *                    to remove.
     * @param[in]  end    Iterator to the end of the set.
     * @param      count  Number of simplices removed already.
     *
     * @tparam     T      Typename of the iterator.
     *
     * @return     The number of simplices removed
     */
    template <typename T>
    static std::size_t apply(type_this *that, T begin, T end,
                             std::size_t &count) {
      for (auto i = begin; i != end; ++i) {
        that->remove_node(*i);
        ++count;
      }
      return count;
    }
  };

  /**
   * @brief      Recursively retrieve a simplex of interest.
   *
   * @tparam     level  The current simplex dimension.
   * @tparam     n      Number of remaining times to recurse.
   */
  template <std::size_t level, std::size_t n> struct get_recurse {
    /**
     * @brief      Get the simplex of interest.
     *
     * @param[in]  that  The simplicial complex to search.
     * @param[in]  s     Pointer to an array of Keys.
     * @param      root  The current simplex
     *
     * @return     Returns a pointer to the node.
     */
    static Node<level + n> *apply(const KeyType *s, Node<level> *root) {
      // TODO: We probably don't need to check if root is a valid
      // simplex (10)
      if (root) {
        auto p = root->_up.find(*s);
        if (p != root->_up.end()) {
          return get_recurse<level + 1, n - 1>::apply(s + 1, root->_up.at(*s));
        } else {
          return nullptr;
        }
      } else {
        return nullptr;
      }
    }
  };
  /**
   * @brief      Recursively retrieve a simplex of interest.
   *
   * @tparam     level  The current simplex dimension.
   */
  template <std::size_t level> struct get_recurse<level, 0> {
    /**
     * @brief      Get the simplex of interest.
     *
     * @param[in]  that  The simplicial complex to search.
     * @param[in]  s     Pointer to an array of Keys.
     * @param      root  The current simplex
     *
     * @return     Returns a pointer to the node.
     */
    static Node<level> *apply(const KeyType *, Node<level> *root) {
      return root;
    }
  };

  /**
   * @brief      Recursively retrieve a simplex of interest going down.
   *
   * @tparam     level  The current simplex dimension.
   * @tparam     n      Number of remaining times to recurse.
   */
  template <std::size_t level, std::size_t n> struct get_down_recurse {
    /**
     * @brief      Get the simplex of interest.
     *
     * @param[in]  that  The simplicial complex to search.
     * @param[in]  s     Pointer to an array of Keys.
     * @param      root  The current simplex
     *
     * @return     Returns a pointer to the node.
     */
    static Node<level - n> *apply(const KeyType *s, Node<level> *root) {
      if (root) {
        auto p = root->_down.find(*s);
        if (p != root->_down.end()) {
          return get_down_recurse<level - 1, n - 1>::apply(s + 1,
                                                           root->_down[*s]);
        } else {
          return nullptr;
        }
      } else {
        return nullptr;
      }
    }
  };

  /**
   * @brief      Recursively retrieve a simplex of interest going down.
   *
   * @tparam     level  The current simplex dimension.
   */
  template <std::size_t level> struct get_down_recurse<level, 0> {
    /**
     * @brief      Get the simplex of interest.
     *
     * @param[in]  this  The simplicial complex to search.
     * @param[in]  s     Pointer to an array of Keys.
     * @param      root  The current simplex
     *
     * @return     Returns a pointer to the node.
     */
    static Node<level> *apply(const KeyType *, Node<level> *root) {
      return root;
    }
  };

  /**
   * @brief      Insert a simplex and all dependent simplices into the
   *             complex.
   *
   * @tparam     level  Dimension of the current root simplex
   * @tparam     n      The number of times to recurse.
   */
  template <std::size_t level, std::size_t n> struct insert_full {
    /**
     * @brief      Kick off a for loop to insert all cofaces.
     *
     * @param      that   The simplicial complex
     * @param      root   The current simplex to insert at.
     * @param[in]  begin  Pointer to an array of Keys.
     *
     * @return     Returns the node to insert.
     */
    static Node<level + n> *apply(type_this *that, Node<level> *root,
                                  const KeyType *begin) {
      return insert_for<level, n, n>::apply(that, root, begin);
    }
  };

  /**
   * @brief      Insert a simplex and all dependent simplices into the
   *             complex.
   *
   * @tparam     level  Dimension of the current root simplex
   */
  template <std::size_t level> struct insert_full<level, 0> {
    /**
     * @brief      Terminal case.
     *
     * @param      that   The simplicial complex
     * @param      root   The current simplex to insert at.
     * @param[in]  begin  Pointer to an array of Keys.
     *
     * @return     Returns the node to insert.
     */
    static Node<level> *apply(type_this *, Node<level> *root, const KeyType *) {
      return root;
    }
  };

  /**
   * @brief      Iterate over antistep
   *
   * @tparam     level     Dimension of the current root simplex
   * @tparam     antistep  Antistep to track which indices to append to root.
   * @tparam     n         Original antistep.
   */
  template <std::size_t level, std::size_t antistep, std::size_t n>
  struct insert_for {
    /**
     * @brief      Call insert_raw and continue for loop
     *
     * @param      that   The simplicial complex
     * @param      root   The current simplex to insert at.
     * @param[in]  begin  Pointer to an array of Keys.
     *
     * @return     Returns the node to insert.
     */
    static Node<level + n> *apply(type_this *that, Node<level> *root,
                                  const KeyType *begin) {
      insert_raw<level, n - antistep>::apply(that, root, begin);
      return insert_for<level, antistep - 1, n>::apply(that, root, begin);
    }
  };

  /**
   * @brief      Terminal case.
   *
   * @tparam     level  Dimension of the current root simplex.
   * @tparam     n      Original antistep.
   */
  template <std::size_t level, std::size_t n> struct insert_for<level, 1, n> {
    /**
     * @brief      Call insert_raw and stop loop
     *
     * @param      that   The simplicial complex
     * @param      root   The current simplex to insert at.
     * @param[in]  begin  Pointer to an array of Keys.
     *
     * @return     Returns the node to insert.
     */
    static Node<level + n> *apply(type_this *that, Node<level> *root,
                                  const KeyType *begin) {
      return insert_raw<level, n - 1>::apply(that, root, begin);
    }
  };

  /**
   * @brief      Actually insert the node and connect up and down.
   *
   * @tparam     level  Dimension of the current root simplex.
   * @tparam     n      The index to append to root.
   */
  template <std::size_t level, std::size_t n> struct insert_raw {
    /**
     * @brief      Create the node and connect up and down.
     *
     * @param      that   The simplicial complex
     * @param      root   The current simplex to insert at.
     * @param[in]  begin  Pointer to an array of Keys.
     *
     * @return     Returns the node to insert.
     */
    static Node<level + n + 1> *apply(type_this *that, Node<level> *root,
                                      const KeyType *begin) {

      KeyType v = *(begin + n);
      Node<level + 1> *nn;
      // if root->v doesn't exist then create it
      auto iter = root->_up.find(v);
      if (iter == root->_up.end()) {
        nn = that->create_node<level + 1>();

        nn->_down[v] = root;
        root->_up[v] = nn;
        that->backfill(root, nn, v);
      } else {
        nn = iter->second; // otherwise get it
      }
      return insert_full<level + 1, n>::apply(that, nn, begin);
    }
  };

  /**
   * @brief      Backfill in the pointers from prior nodes to the new node
   *
   * @param      root   is a parent node
   * @param      nn     is the new child node
   * @param      value  is the exposed id of nn
   * @return     void
   *
   * @tparam     level  Dimension of the current root simplex.
   */
  template <std::size_t level>
  void backfill(Node<level> *root, Node<level + 1> *nn, KeyType value) {
    for (auto curr = root->_down.begin(); curr != root->_down.end(); ++curr) {
      int v = curr->first;

      Node<level - 1> *parent = curr->second;
      Node<level> *child = parent->_up[value];

      nn->_down[v] = child;
      child->_up[v] = nn;
    }
  }

  /**
   * @brief      Fill in the pointers from level 1 to 0.
   *
   * @param      root   is a level 0 node
   * @param      nn     is a level 1 node
   * @param      value  is the exposed id of nn
   * @return     void
   */
  void backfill(Node<0> *, Node<1> *, int) { return; }

  /**
   * @brief      Creates a new node of some dimension.
   *
   * @param[in]  x      Argument to help deduce the new node dimension
   *
   * @tparam     level  Simplex dimension
   *
   * @return     A pointer to the new node.
   */
  template <std::size_t level> Node<level> *create_node() {
    // Create the new node
    auto p = new Node<level>(node_count++);
    ++(level_count[level]); // Increment the count in the level

    // node_count-1 to match the internal IDs correctly.
    MAYBE_UNUSED bool ret =
        std::get<level>(levels)
            .insert(std::pair<std::size_t, NodePtr<level>>(node_count - 1, p))
            .second;
    assert(ret);
    /*
       // sanity check to make sure there aren't duplicate keys...
       if (ret==false) {
        std::cout << "Error: Node '" << node_count << "' already existed
           with value " << *p << std::endl;
       }
     */
    return p;
  }

  /**
   * @brief      Removes a node.
   *
   * @param      p      Simplex to remove
   *
   * @tparam     level  Dimension of the simplex
   */
  template <std::size_t level> void remove_node(Node<level> *p) {
    for (auto curr = p->_down.begin(); curr != p->_down.end(); ++curr) {
      curr->second->_up.erase(curr->first);
    }
    for (auto curr = p->_up.begin(); curr != p->_up.end(); ++curr) {
      curr->second->_down.erase(curr->first);
    }
    --(level_count[level]);
    std::get<level>(levels).erase(p->_node);
    delete p;
  }

  /**
   * @brief      Removes a node.
   *
   * @param      p     Simplex to remove
   */
  void remove_node(Node<1> *p) {
    // This for loop should only have a single iteration.
    for (auto curr = p->_down.begin(); curr != p->_down.end(); ++curr) {
      unused_vertices.insert(curr->first);
      curr->second->_up.erase(curr->first);
    }
    for (auto curr = p->_up.begin(); curr != p->_up.end(); ++curr) {
      curr->second->_down.erase(curr->first);
    }
    --(level_count[1]);
    std::get<1>(levels).erase(p->_node);
    delete p;
  }

  /**
   * @brief      Removes a node.
   *
   * @param      p     Simplex to remove
   */
  void remove_node(Node<0> *p) {
    for (auto curr = p->_up.begin(); curr != p->_up.end(); ++curr) {

      curr->second->_down.erase(curr->first);
    }
    --(level_count[0]);
    std::get<0>(levels).erase(p->_node);
    delete p;
  }

  /**
   * @brief      Removes a node.
   *
   * @param      p     Simplex to remove
   */
  void remove_node(Node<topLevel> *p) {
    for (auto curr = p->_down.begin(); curr != p->_down.end(); ++curr) {
      curr->second->_up.erase(curr->first);
    }
    --(level_count[topLevel]);
    std::get<topLevel>(levels).erase(p->_node);
    delete p;
  }

  /// The root node
  NodePtr<0> _root;
  /// A counter of the total number of nodes.
  std::size_t node_count;
  /// A counter of the number of simplices per level.
  std::array<std::size_t, numLevels> level_count;
  /// Typename of a tuple of LevelIndex broadcasted with NodePtr<k>.
  using NodePtrLevel = typename util::int_type_map<std::size_t, std::tuple,
                                                   LevelIndex, NodePtr>::type;
  /// Typename of a map of levels to NodePtr<k>*'s.
  typename util::type_map<NodePtrLevel, detail::map>::type levels;
  /// B-tree of unused vertex indices.
  index_tracker::index_tracker<KeyType> unused_vertices;
};

/**
 * Alias to generate a CASC from a list of traits.
 * See also simplicial_complex_traits_default. Example -- To create a
 * tetrahedral mesh with integer data on all simplices:
 * ~~~~~~~~~~~~~~~{.cpp}
 * auto mesh = AbstractSimplicialComplex<
 *     int, // KEYTYPE
 *     int, // Root data
 *     int, // Vertex data
 *     int, // Edge data
 *     int, // Face data
 *     int  // Volume data
 * >();
 * ~~~~~~~~~~~~~~~
 */
template <typename KeyType, typename... Ts>
using AbstractSimplicialComplex = simplicial_complex<
    detail::simplicial_complex_traits_default<KeyType, Ts...>>;

/// @cond detail
namespace simplex_set_detail {
/**
 * @brief      Template to compute a hash for a SimplexID.
 *
 * Since SimplexID is actually a wrapper around a Node* we have to hash it
 * accordingly. The static_cast calls the defined explicit operator which
 * reinterprets the stored Node* pointer as a uintptr_t which we can hash
 * directly.
 *
 * @tparam     SimplexID  Typename of the SimplexID.
 */
template <typename SimplexID> struct hashSimplexID {
  /**
   * @brief   Compute the hash.
   *
   * ~~~~~~~~~~~~~~~~~(.cpp)
   * std::cout << hashSimplexID<decltype(nid)>{}(nid) << std::endl;
   * ~~~~~~~~~~~~~~~~~
   *
   * @param[in]  nid   The simplex of interest.
   * @return     Resultant hash.
   */
  std::size_t operator()(const SimplexID nid) const {
    return std::hash<std::uintptr_t>()(static_cast<uintptr_t>(nid));
  }
};
} // end namespace simplex_set_detail
/// @endcond

/// Helpful alias defining a unordered_set of simplices. See also hashSimplexID.
template <typename T>
using NodeSet = std::unordered_set<T, simplex_set_detail::hashSimplexID<T>>;
} // end namespace casc
