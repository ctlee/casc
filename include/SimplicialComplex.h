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
 * @file SimplicialComplex.h
 * @brief This header contains the main CASC data structure.
 */

#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <iterator>
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
#include <ostream>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <assert.h>
#include "index_tracker.h"
#include "util.h"

namespace detail {
	/**< Data structure to store simplices by level*/
	template <class T> using map = std::map<size_t,T>;

	/*
	 * asc_Node must be defined outside of simplicial_complex because C++ does 
	 * not allow internal templates to be partially specialized. This template
	 * prototype is later specialized to represent various Node roles.
	 */
	template <class KeyType, size_t k, size_t N, typename DataTypes, class> struct asc_Node;

	/**
	 * @brief      This is the base Node class.
	 */
	struct asc_NodeBase {
		/**
		 * @brief      Construct a Node
		 * 
		 * @param[in]  id    An internal integer identifier of the Node.
		 */
		asc_NodeBase(int id) : _node(id) {}
		
		/**
		 * @brief      Destructor
		 */
		virtual ~asc_NodeBase() {};

		size_t _node;	/**< Internal Node ID*/
	};

	/**
	 * @brief      Base class for Node with some data.
	 *
	 * @tparam     DataType  Typename of the data to be stored.
	 */
	template <class DataType>
	struct asc_NodeData {
		DataType _data; /**< stored data with type DataType */
	};

	/**
	 * @brief      Explicit specialization for Nodes without data.
	 * 
	 * This exists so that the compiler knows to not allocate any memory to
	 * store data when void is specified.
	 */
	template <>
	struct asc_NodeData<void> {};

	/**
	 * @brief      Base class for Nodes with edge data.
	 *
	 * @tparam     KeyType   Typename of index for indexing Nodes.
	 * @tparam     DataType  Typename of the data stored on the edge.
	 */
	template <class KeyType, class DataType>
	struct asc_EdgeData {
		/** The map of NodeIDs to stored edge data. */
		std::unordered_map<KeyType, DataType> _edge_data;
	};	

	/**
	 * @brief      Explicit specialization for Nodes with no edge data.
	 *
	 * @tparam     KeyType   Typename of index for indexing Nodes.
	 */
	template <class KeyType>
	struct asc_EdgeData<KeyType,void> {};

	/**
	 * @brief      Base class for Node with parent nodes
	 *
	 * @tparam     KeyType        Typename of the Node index
	 * @tparam     k              The Simplex dimension
	 * @tparam     N              Dimension of the Complex
	 * @tparam     NodeDataTypes  A util::type_holder array of Node types
	 * @tparam     EdgeDataTypes  A util::type_holder array of Edge types
	 */
	template <	class KeyType, 
				size_t k, 
				size_t N, 
				class NodeDataTypes, 
				class EdgeDataTypes>
	struct asc_NodeDown : 
			public asc_EdgeData<KeyType, 
					typename util::type_get<k-1,EdgeDataTypes>::type> {
		/** Alias the typename of the parent Node */
		using DownNodeT = asc_Node<KeyType,k-1,N,NodeDataTypes,EdgeDataTypes>;
		/** Map of indices to parent Node pointers*/
		std::map<KeyType, DownNodeT*> _down;
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
	template <	class KeyType, 
				size_t k, 
				size_t N, 
				class NodeDataTypes, 
				class EdgeDataTypes>
	struct asc_NodeUp {
		using UpNodeT = asc_Node<KeyType,k+1,N,NodeDataTypes,EdgeDataTypes>;
		std::unordered_map<KeyType, UpNodeT*> _up;	/**< @brief Map of pointers to children */
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
	template <class KeyType, size_t k, size_t N, class NodeDataTypes, class EdgeDataTypes>
	struct asc_Node : public asc_NodeBase,
					  public asc_NodeData<typename util::type_get<k,NodeDataTypes>::type>,
					  public asc_NodeDown<KeyType, k, N, NodeDataTypes, EdgeDataTypes>,
					  public asc_NodeUp<KeyType, k, N, NodeDataTypes, EdgeDataTypes>
	{
		static constexpr size_t level = k;
		/**
		 * @brief      Default constructor
		 *
		 * @param[in]  id    The internal integer identifier.
		 */
		asc_Node(int id) : asc_NodeBase(id) {}

       	/**
       	 * @brief      Print the Node out for debugging only
       	 *
       	 * @param      output  The output stream.
       	 * @param[in]  node    The Node of interest to print.
       	 *
       	 * @return     A handle to the output stream.
       	 */
        friend std::ostream& operator<<(std::ostream& output, const asc_Node& node){
            output  << "Node(level=" << k << ", " << "id=" << node._node;
            if(node._down.size() > 0)
                for(auto it=node._down.cbegin(); it!=node._down.cend(); ++it)
                    output  << ", NodeDownID={'"
                            << it->first << "', "
                            << it->second->_node << "}";
            if(node._up.size() > 0)
               for(auto it=node._up.cbegin(); it!=node._up.cend(); ++it)
                    output  << ", NodeUpID={'"
                            << it->first << "', "
                            << it->second->_node << "}";
            output  << ")";
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
	template <class KeyType, size_t N, class NodeDataTypes, class EdgeDataTypes>
	struct asc_Node<KeyType,0,N,NodeDataTypes,EdgeDataTypes> : 
			public asc_NodeBase,
			public asc_NodeData<typename util::type_get<0,NodeDataTypes>::type>,
			public asc_NodeUp<KeyType, 0, N, NodeDataTypes, EdgeDataTypes>
	{
		static constexpr size_t level = 0;

		/**
		 * @brief      Default constructor
		 *
		 * @param[in]  id    The internal integer identifier.
		 */
		asc_Node(int id) : asc_NodeBase(id) {}
  
       	/**
       	 * @brief      Print the Node out for debugging only
       	 *
       	 * @param      output  The output stream.
       	 * @param[in]  node    The Node of interest to print.
       	 *
       	 * @return     A handle to the output stream.
       	 */ 
        friend std::ostream& operator<<(std::ostream& output, const asc_Node& node){
            output  << "Node(level=" << 0
                    << ", id=" << node._node;
            if(node._up.size() > 0)
                for(auto it=node._up.cbegin(); it!=node._up.cend(); ++it)
                    output  << ", NodeUpID={'"
                            << it->first << "', "
                            << it->second->_node << "}";
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
	template <class KeyType, size_t N, class NodeDataTypes, class EdgeDataTypes>
	struct asc_Node<KeyType,N,N,NodeDataTypes,EdgeDataTypes> : public asc_NodeBase,
											 public asc_NodeData<typename util::type_get<N,NodeDataTypes>::type>,
					  						 public asc_NodeDown<KeyType, N, N, NodeDataTypes, EdgeDataTypes>
	{
		static constexpr size_t level = N;
		/**
		 * @brief      Default constructor
		 *
		 * @param[in]  id    The internal integer identifier.
		 */
		asc_Node(int id) : asc_NodeBase(id) {}
       
        /**
       	 * @brief      Print the Node out for debugging only
       	 *
       	 * @param      output  The output stream.
       	 * @param[in]  node    The Node of interest to print.
       	 *
       	 * @return     A handle to the output stream.
       	 */
        friend std::ostream& operator<<(std::ostream& output, const asc_Node& node){
            output  << "Node(level=" << N
                    << ", id=" << node._node;
            if(node._down.size() > 0)
                for(auto it=node._down.cbegin(); it!=node._down.cend(); ++it)
                    output  << ", NodeDownID={'"
                            << it->first << "', "
                            << it->second->_node << "}";
            output << ")";
            return output;
        }
	};


	/*
	 * @brief      An iterator adapter to iterate over NodeIDs 
	 *
	 * @tparam     Iter  Typename of the iterator
	 * @tparam     Data  Typename of the data
	 */
	template <typename Iter, typename Data>
	struct node_id_iterator : public std::iterator<std::bidirectional_iterator_tag, Data> {
	public:
		using super = std::iterator<std::bidirectional_iterator_tag, Data>;
		node_id_iterator() {}
		node_id_iterator(Iter j) : i(j) {}
		node_id_iterator& operator++() { ++i; return *this; }
		node_id_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
		node_id_iterator& operator--() { --i; return *this; }
		node_id_iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }
		bool operator==(node_id_iterator j) const { return i == j.i; }
		bool operator!=(node_id_iterator j) const { return !(*this == j); }
		Data operator*() { return Data(i->second); }
        typename super::pointer operator->() { return Data(i->second); }
	protected:
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
	inline node_id_iterator<Iter,Data> make_node_id_iterator(Iter j) { 
		return node_id_iterator<Iter,Data>(j);
	}

	/**
	 * @brief      An iterator adapter to iterate over Node data. 
	 *
	 * @tparam     Iter  Typename of the iterator
	 * @tparam     Data  Typename of the data
	 */
	template <typename Iter, typename Data>
	struct node_data_iterator : public std::iterator<std::bidirectional_iterator_tag, Data> {
	public:
		using super = std::iterator<std::bidirectional_iterator_tag, Data>;
		node_data_iterator() {}
		node_data_iterator(Iter j) : i(j) {}
		node_data_iterator& operator++() { ++i; return *this; }
		node_data_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
		node_data_iterator& operator--() { --i; return *this; }
		node_data_iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }
		bool operator==(node_data_iterator j) const { return i == j.i; }
		bool operator!=(node_data_iterator j) const { return !(*this == j); }
		typename super::reference operator*() { return i->second->_data; }
		typename super::pointer operator->() { return i->second->_data; }
	protected:
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
	inline node_data_iterator<Iter,Data> make_node_data_iterator(Iter j) { 
		return node_data_iterator<Iter,Data>(j);
	}
} // end namespace detail

/**
 * @brief      Helper to expand traits for AbstractSimplicialComplex
 *
 * @tparam     K     Typename of the simplex KeyType
 * @tparam     Ts    Types for nodes and edges.
 */
template <typename K, typename... Ts>
struct simplicial_complex_traits_default
{
	template <std::size_t k> using all_void = int;
	using KeyType = K;
	using NodeTypes = util::type_holder<Ts...>;
	using EdgeTypes = typename util::int_type_map<std::size_t,
				util::type_holder,
				typename std::make_index_sequence<sizeof...(Ts)-1>,
				all_void>::type;
};


/**
 * @class simplicial_complex
 *
 * @brief The CASC data structure for representing simplicial complexes of 
 * arbitrary dimensionlity with coloring.
 *
 * @tparam     traits  The traits defining the KeyType, NodeTypes and EdgeTypes.
 */
template <typename traits>
class simplicial_complex {
public:
	using KeyType                   = typename traits::KeyType;
	using NodeDataTypes             = typename traits::NodeTypes;
	using EdgeDataTypes             = typename traits::EdgeTypes;
	using type_this                 = simplicial_complex<traits>;
	static constexpr auto numLevels = NodeDataTypes::size;
	static constexpr auto topLevel  = numLevels-1;
	using LevelIndex                = typename std::make_index_sequence<numLevels>;
private:
	template <std::size_t k> using Node     = detail::asc_Node<KeyType,k,topLevel,NodeDataTypes,EdgeDataTypes>;
	template <std::size_t k> using NodePtr  = Node<k>*;

public:
	template <std::size_t k> using NodeData = typename util::type_get<k,NodeDataTypes>::type;
	template <std::size_t k> using EdgeData = typename util::type_get<k,EdgeDataTypes>::type;

	friend struct SimplexID;
	/**
	 * @brief      A handle for a simplex in the complex.
	 * 
	 * SimplexID wraps a Node* for external handling.
	 *
	 * @tparam     k     Simplex dimension
	 */
	template <std::size_t k>
	struct SimplexID {
		using complex = simplicial_complex<traits>;
		friend simplicial_complex<traits>;
		static constexpr size_t level = k;

		SimplexID() : ptr(nullptr) {}
		SimplexID(NodePtr<k> p) : ptr(p) {}
		SimplexID(const SimplexID& rhs) : ptr(rhs.ptr) {}

		SimplexID& operator=(const SimplexID& rhs) { ptr = rhs.ptr; return *this;}
		friend bool operator==(SimplexID lhs, SimplexID rhs) { return lhs.ptr == rhs.ptr; }
		friend bool operator!=(SimplexID lhs, SimplexID rhs) { return lhs.ptr != rhs.ptr; }
		friend bool operator<=(SimplexID lhs, SimplexID rhs) { return lhs.ptr <= rhs.ptr; }
		friend bool operator>=(SimplexID lhs, SimplexID rhs) { return lhs.ptr >= rhs.ptr; }
		friend bool operator<(SimplexID lhs, SimplexID rhs)  { return lhs.ptr < rhs.ptr; }
		friend bool operator>(SimplexID lhs, SimplexID rhs)  { return lhs.ptr > rhs.ptr; }

		explicit operator std::uintptr_t () const { return reinterpret_cast<std::uintptr_t>(ptr); }

		auto const&& operator*() const { return ptr->_data; }
		auto&& operator*() { return ptr->_data; }

		auto const&& data() const { return ptr->_data; }
		auto&& data() { return ptr->_data; }

		/**
		 * @brief      A full debug printout of of the node itself
		 *
		 * @param      out   Handle to the stream
		 * @param[in]  nid   Node of interest
		 *
		 * @return     Handle to the stream
		 */
		//friend std::ostream& operator<<(std::ostream& out, const SimplexID& nid){ out << *nid.ptr; return out; }
		
		/**
		 * @brief      Print basic information about the simplex.
		 *
		 * @param      out   Handle to the stream
		 * @param[in]  nid   Node of interest
		 *
		 * @return     Handle to the stream
		 */
		// friend std::ostream& operator<<(std::ostream& out, 
		// 		const simplexid& nid){
		// 	// we really need a static_if here...
		// 	out << "s{";
		// 	print_helper<k,0>::apply(out, nid);
		// 	out << "}";
		// 	return out;
		// }

		/**
		 * @brief      Print the SimplexID as if it were an ID
		 *
		 * @param      out   Handle to the stream
		 * @param[in]  nid   Node of interest
		 *
		 * @return     Handle to the stream
		 */
		friend std::ostream& operator<<(std::ostream& out, const SimplexID& nid) { out << nid.ptr; return out; }

	private:
		template <size_t l, size_t foo>
		struct print_helper
		{
			static std::ostream& apply(std::ostream& out, 
					const SimplexID<l>& nid){
				auto down = (*nid.ptr)._down;
				for(auto it=down.cbegin(); it!= --down.cend(); ++it){
					out << it->first << ",";
				}
				out << (--down.cend())->first;
				return out;
			}
		};

		template <size_t foo>
		struct print_helper<0, foo>
		{
			static std::ostream& apply(std::ostream& out, 
					const SimplexID& nid){
				out << "root";
				return out;
			}
		};

		NodePtr<k> ptr;
	};


	friend struct EdgeID;
	/**
	 * @brief      External reference to an edge or a connection within the complex.
	 *
	 * @tparam     k     The edge connects a simplex of size k-1 to a simplex of size k.
	 */
	template <std::size_t k>
	struct EdgeID {
		using complex = simplicial_complex<traits>;
		friend simplicial_complex<traits>;
		static constexpr size_t level = k;

		EdgeID() : ptr(nullptr), edge(0) {}
		EdgeID(NodePtr<k> p, KeyType e) : ptr(p), edge(e) {}
		EdgeID(const EdgeID& rhs) : ptr(rhs.ptr), edge(rhs.edge) {}

		EdgeID& operator=(const EdgeID& rhs) { ptr = rhs.ptr; edge = rhs.edge; return *this;}
		friend bool operator==(EdgeID lhs, EdgeID rhs) { return lhs.ptr == rhs.ptr && lhs.edge == rhs.edge; }
		friend bool operator!=(EdgeID lhs, EdgeID rhs) { return !(lhs == rhs); }
		friend bool operator<=(EdgeID lhs, EdgeID rhs) { return lhs < rhs || lhs == rhs; }
		friend bool operator>=(EdgeID lhs, EdgeID rhs) { return lhs > rhs || lhs == rhs; }
		friend bool operator<(EdgeID lhs, EdgeID rhs)
		{
			return (lhs.ptr < rhs.ptr) || (lhs.ptr == rhs.ptr && lhs.edge < rhs.edge);
		}
		friend bool operator>(EdgeID lhs, EdgeID rhs)  { return rhs < lhs; }

//		explicit operator std::size_t () const { return static_cast<std::size_t>(ptr); }

		auto const& operator*() const { return data(); }
		auto& operator*() { return data(); }

		KeyType key() const { return edge; }

		auto const& data() const { return ptr->_edge_data[edge]; }
		auto& data() { return ptr->_edge_data[edge]; }

		SimplexID<k>   up() const { return ptr; }
		SimplexID<k-1> down() const { return SimplexID<k-1>(ptr->_down[edge]); }

	private:
		NodePtr<k> ptr;
		KeyType edge;
	};

	/**
	 * @brief      Default constructor
	 */
	simplicial_complex()
		: node_count(0)
	{
		_root = create_node(std::integral_constant<std::size_t,0>());
		for(auto& x : level_count)
		{
			x = 0;
		}
	}

	/**
	 * @brief      Destroys the object.
	 */
	~simplicial_complex()
	{
		size_t count;
		remove_recurse<0,0>::apply(this, &_root, &_root + 1, count);
	}

	/**
	 * @brief      Insert the simplex named 's', and all sub-simplices, into the complex.
	 *
	 * @param[in]  s     The simplex to be inserted.
	 *
	 * @tparam     n     Size of simplex 's'.
	 */
	template <size_t n>
	void insert(const KeyType (&s)[n])
	{
		for(const KeyType* p = s; p < s + n; ++p)
		{
			unused_vertices.remove(*p);
		}
		insert_full<0,n>::apply(this, _root, s);	
	}

	/**
	 * @brief      Insert the simplex named 's', and all sub-simplices, into the complex. 'data' is stored in the complex at 's'.
	 *
	 * @param[in]  s     The simplex to be inserted.
	 * @param[in]  data  The data to be stored at the simplex 's'.
	 *
	 * @tparam     n     Size of simplex 's'.
	 */
	template <size_t n>
	void insert(const KeyType (&s)[n], const NodeData<n>& data)
	{
		for(const KeyType* p = s; p < s + n; ++p)
		{
			unused_vertices.remove(*p);
		}
		Node<n>* rval = insert_full<0,n>::apply(this, _root, s);
		rval->_data = data;
	}

	/**
	 * @brief      Insert the simplex named 's', and all sub-simplices, into the complex.
	 *
	 * @param[in]  s     The simplex to be inserted.
	 *
	 * @tparam     n     Size of simplex 's'.
	 */
	template <size_t n>
	void insert(const std::array<KeyType,n>& s)
	{
		for(KeyType x : s)
		{
			unused_vertices.remove(x);
		}
		insert_full<0,n>::apply(this, _root, s.data());
	}

	/**
	 * @brief      Insert the simplex named 's', and all sub-simplices, into the complex. 'data' is stored in the complex at 's'.
	 *
	 * @param[in]  s     The simplex to be inserted.
	 * @param[in]  data  The data to be stored at the simplex 's'.
	 *
	 * @tparam     n     Size of simplex 's'.
	 */
	template <size_t n>
	void insert(const std::array<KeyType,n>& s, const NodeData<n>& data)
	{
		for(KeyType x : s)
		{
			unused_vertices.remove(x);
		}
		Node<n>* rval = insert_full<0,n>::apply(this, _root, s.data());
		rval->_data = data;
	}

	KeyType add_vertex()
	{
		KeyType v[1] = {unused_vertices.pop()};
		insert<1>(v);
		return v[0];
	}

	/**
	 * @brief      Gets the name of the simplex referenced by 'id'.
	 *
	 * @param[in]  id      The identifier of a simplex.
	 * @param[in]  fn      Function will be called with characters of 'name'.
	 *
	 * @tparam     n       Size of the simplex referenced by 'id'.
	 * @tparam     Lambda  Type which supports operator(KeyType).
	 */
	template <size_t n, typename Lambda>
	void get_name(SimplexID<n> id, Lambda fn) const
	{
		for(auto curr : id.ptr->_down)
		{
			fn(curr.first);
		}
	}

	/**
	 * @brief      Gets the name of the simplex referenced by 'id'.
	 *
	 * @param[in]  id      The identifier of a simplex.
	 *
	 * @tparam     n       Size of the simplex referenced by 'id'.
	 *
	 * @return     The name.
	 */
	template <size_t n>
	std::array<KeyType,n> get_name(SimplexID<n> id) const
	{
		std::array<KeyType,n> s;

		int i = 0;
		for(auto curr : id.ptr->_down)
		{
			s[i++] = curr.first;
		}
		
		return s;
	}

	/**
	 * @brief      Gets the name of the simplex referenced by 'id'. This is a special case which handles the empty set simplex.
	 *
	 * @param[in]  id    The identifier
	 *
	 * @return     The name.
	 */
	std::array<KeyType,0> get_name(SimplexID<0> id) const
	{
		std::array<KeyType,0> name;
		return name;
	}


	/**
	 * @brief      Gets the simplex identifier which has the name 's'.
	 *
	 * @param[in]  s     Name of the simplex to find.
	 *
	 * @tparam     n     Size of simplex s.
	 *
	 * @return     The node up.
	 */
	template <size_t n>
	SimplexID<n> get_simplex_up(const KeyType (&s)[n]) const
	{
		return get_recurse<0,n>::apply(this, s, _root);
	}

	/**
	 * @brief      Get the simplex identifier which has the name 's' relative to the simplex 'id'.
	 *
	 * @param[in]  id   The identifier of a simplex.
	 * @param[in]  s    The relative name of the desired simplex.
	 *
	 * @tparam     i    The size of simplex 'id'.
	 * @tparam     j    The length of the name 's'.
	 *
	 * @return     The node up.
	 */
	template <size_t i, size_t j>
	SimplexID<i+j> get_simplex_up(const SimplexID<i> id, const KeyType (&s)[j]) const
	{
		return get_recurse<i,j>::apply(this, s, id);
	}

	/**
	 * @brief      Convenience version of get_simplex_up when the name 's' consists of a single character.
	 *
	 * @param[in]  nid   The identifier of a simplex.
	 * @param[in]  s     The relative single character name of the desired simplex.
	 *
	 * @tparam     i     The size of simplex 'id'.
	 *
	 * @return     The node up.
	 */
	template <size_t i>
	SimplexID<i+1> get_simplex_up(const SimplexID<i> id, const KeyType s) const
	{
		return get_recurse<i,1>::apply(this, &s, id.ptr);
	}

	/**
	 * @brief      Get the root simplex.
	 * 
	 * @return     The root simplex.
	 */
	SimplexID<0> get_simplex_up() const
	{
		return _root;
	}


	/**
	 * @brief      Get the sub-simplex of the simplex 'id' which does not have 's' in the name.
	 *
	 * @param[in]  id    The identifier of a simplex.
	 * @param[in]  s     The relative name of the desired simplex.
	 *
	 * @tparam     i     The size of simplex 'id'.
	 * @tparam     j     The length of the name 's'
	 *
	 * @return     The node down.
	 */
	template <size_t i, size_t j>
	SimplexID<i-j> get_simplex_down(const SimplexID<i> id, const KeyType (&s)[j]) const
	{
		return get_down_recurse<i,j>::apply(this, s, id.ptr);
	}

	/**
	 * @brief      Convenience version of get_simplex_down when the name 's' consists of a single character.
	 *
	 * @param[in]  id   The identifier of a simplex.
	 * @param[in]  s    The relative single character name of the desired simplex.
	 *
	 * @tparam     i    The size of simplex 'id'.
	 *
	 * @return     The node down.
	 */
	template <size_t i>
	SimplexID<i-1> get_simplex_down(const SimplexID<i> nid, const KeyType s) const
	{
		return get_down_recurse<i,1>::apply(this, &s, nid.ptr);
	}

	/**
	 * @brief      Get the root simplex.
	 *
	 * @return     The root simplex.
	 */
	SimplexID<0> get_simplex_down() const
	{
		return _root;
	}

	template <size_t k, class Inserter>
	void get_cover_insert(const SimplexID<k> id, Inserter pos) const
	{
		for(auto curr : id.ptr->_up)
		{
			*pos++ = curr.first;
		}
	}

	template <size_t k, class Lambda>
	void get_cover(const SimplexID<k> id, Lambda fn) const
	{
		for(auto curr : id.ptr->_up)
		{
			fn(curr.first);
		}
	}

	template <size_t k>
	auto get_cover(const SimplexID<k> id) const
	{
		std::vector<KeyType> rval;
		get_cover_insert(id, std::back_inserter(rval));
		return std::move(rval);
	}

	/**
	 *  @brief      Get the set of simplices of which the provided simplices are sub-simplices of.
	 *  
	 *  @param 		simplices The set of sub-simplices.
	 *  
	 *  @return 	Set of simplices which contain all sub-simplices 'simplices'.
	 */
	template <size_t k>
	std::set<SimplexID<k+1>> up(const std::set<SimplexID<k>>& simplices) const
	{
		std::set<SimplexID<k+1>> rval;
		for(auto simplex : simplices)
		{
			for(auto p : simplex.ptr->_up)
			{
				rval.insert(SimplexID<k+1>(p.second));
			}
		}
		return rval;
	}

	/**
	 *  @brief      Get the set of simplices of which the provided simplex is a sub-simplex.
	 *  
	 *  @param 		nid The sub-simplex
	 *  
	 *  @return 	Set of simplices which contain all sub-simplices 'nid'.
	 */
	template <size_t k>
	std::set<SimplexID<k+1>> up(const SimplexID<k> nid) const
	{
		std::set<SimplexID<k+1>> rval;
		for(auto p : nid.ptr->_up)
		{
			rval.insert(SimplexID<k+1>(p.second));
		}
		return rval;
	}

	/**
	 *  @brief      Get the sub-simplices of simplicies 'nodes'.
	 *  
	 *  @param 		nodes The set of simplicies.
	 *  
	 *  @return 	Sub-simplices of 'nodes'.
	 */
	template <size_t k>
	std::set<SimplexID<k-1>> down(const std::set<SimplexID<k>>& nodes) const
	{
		std::set<SimplexID<k-1>> rval;
		for(auto nid : nodes)
		{
			for(auto p : nid.ptr->_down)
			{
				rval.insert(SimplexID<k-1>(p.second));
			}
		}
		return rval;
	}

	/**
	 *  @brief      Get the sub-simplices of simplex 'nid'.
	 *  
	 *  @param 		nid the simplex of interest
	 *  
	 *  @return 	Sub-simplices of 'nid'.
	 */
	template <size_t k>
	std::set<SimplexID<k-1>> down(const SimplexID<k> nid) const
	{
		std::set<SimplexID<k-1>> rval;
		for(auto p : nid.ptr->_down)
		{
			rval.insert(SimplexID<k-1>(p.second));
		}
		return rval;
	}


	/**
	 * @brief      Gets the edge up.
	 *
	 * @param[in]  nid   The nid
	 * @param[in]  a     Key of the edge to get.
	 *
	 * @tparam     k     The level of the simplex of interest
	 *
	 * @return     The edge up.
	 */
	template <size_t k>
	auto get_edge_up(SimplexID<k> nid, KeyType a)
	{
		return EdgeID<k+1>(nid.ptr->_up[a], a);
	}

	template <size_t k>
	auto get_edge_down(SimplexID<k> nid, KeyType a)
	{
		return EdgeID<k>(nid.ptr, a);
	}

	template <size_t k>
	auto get_edge_up(SimplexID<k> nid, KeyType a) const
	{
		return EdgeID<k+1>(nid.ptr->_up[a], a);
	}

	template <size_t k>
	auto get_edge_down(SimplexID<k> nid, KeyType a) const
	{
		return EdgeID<k>(nid.ptr, a);
	}

	template <size_t k>
	bool exists(const KeyType (&s)[k]) const
	{
		
		return get_recurse<0,k>::apply(this, s, _root) != 0;
	}

	/**
	 * @brief      Get the number of simplices of level 'k'.
	 *
	 * @tparam     k     The level of interest
	 *
	 * @return     { description_of_the_return_value }
	 */
	template <std::size_t k>
	auto size() const
	{
		return std::get<k>(levels).size();
	}

	template <std::size_t k>
	auto get_level_id()
	{
		auto begin = std::get<k>(levels).begin();
		auto end = std::get<k>(levels).end();
		auto data_begin = detail::make_node_id_iterator<decltype(begin),SimplexID<k>>(begin);
		auto data_end = detail::make_node_id_iterator<decltype(end),SimplexID<k>>(end);
		return util::make_range(data_begin, data_end);
	}

	template <std::size_t k>
	auto get_level_id() const
	{
		auto begin = std::get<k>(levels).cbegin();
		auto end = std::get<k>(levels).cend();
		auto data_begin = detail::make_node_id_iterator<decltype(begin), const SimplexID<k>>(begin);
		auto data_end = detail::make_node_id_iterator<decltype(end), const SimplexID<k>>(end);
		return util::make_range(data_begin, data_end);
	}

	template <std::size_t k>
	auto get_level()
	{
		auto begin = std::get<k>(levels).begin();
		auto end = std::get<k>(levels).end();
		auto data_begin = detail::make_node_data_iterator<decltype(begin),NodeData<k>>(begin);
		auto data_end = detail::make_node_data_iterator<decltype(end),NodeData<k>>(end);
		return util::make_range(data_begin, data_end);
	}

	template <std::size_t k>
	auto get_level() const
	{
		auto begin = std::get<k>(levels).cbegin();
		auto end = std::get<k>(levels).cend();
		auto data_begin = detail::make_node_data_iterator<decltype(begin), const NodeData<k>>(begin);
		auto data_end = detail::make_node_data_iterator<decltype(end), const NodeData<k>>(end);
		return util::make_range(data_begin, data_end);
	}

	template <std::size_t k>
	size_t remove(const KeyType (&s)[k])
	{
		Node<k>* root = get_recurse<0,k>::apply(this, s, _root);
		size_t count = 0;
		return remove_recurse<k,0>::apply(this, &root, &root + 1, count);
	}

	template <std::size_t k>
	size_t remove(const std::array<KeyType,k>& s)
	{
		Node<k>* root = get_recurse<0,k>::apply(this, s.data(), _root);
		size_t count = 0;
		return remove_recurse<k,0>::apply(this, &root, &root + 1, count);
	}

	template <std::size_t k>
	std::size_t remove(SimplexID<k> s)
	{
		size_t count = 0;
		return remove_recurse<k,0>::apply(this, &s.ptr, &s.ptr + 1, count);
	}

	template <std::size_t L, std::size_t R>
	bool leq(SimplexID<L> lhs, SimplexID<R> rhs) const
	{
		auto name_lhs = get_name(lhs);
		auto name_rhs = get_name(rhs);

		std::size_t i = 0;
		for(std::size_t j = 0; i < L && j < R; ++j)
		{
			if(name_lhs[i] == name_rhs[j])
			{
				++i;
			}
		}

		return (i == L);
	}

	template <std::size_t L, std::size_t R>
	bool eq(SimplexID<L> lhs, SimplexID<R> rhs) const
	{
		return false;
	}

	template <std::size_t k>
	bool eq(SimplexID<k> lhs, SimplexID<k> rhs) const
	{
		auto name_lhs = get_name(lhs);
		auto name_rhs = get_name(rhs);

		for(std::size_t i = 0; i < k; ++i)
		{
			if(name_lhs[i] != name_rhs[i])
			{
				return false;
			}
		}

		return true;
	}

	template <std::size_t L, std::size_t R>
	bool lt(SimplexID<L> lhs, SimplexID<R> rhs) const
	{
		return L < R && leq(lhs,rhs);
	}

private:
	/**
	 * Recursively deletes dependent nodes.
	 *
	 * @tparam     level  { description }
	 * @tparam     foo    A junk argument to get the compiler to play nicely
	 */
	template <size_t level, size_t foo>
	struct remove_recurse
	{
		template <typename T>
		static size_t apply(type_this* that, T begin, T end, size_t& count)
		{
			std::set<Node<level+1>*> next;
			// for each node of interest...
			for(auto i = begin; i != end; ++i)
			{
				auto up = (*i)->_up;
				for(auto j = up.begin(); j != up.end(); ++j)
				{
					next.insert(j->second);
				}
				that->remove_node(*i);
				++count;
			}
			return remove_recurse<level+1,foo>::apply(that, next.begin(), next.end(), count);
		}
	};

    // Terminal condition for remove_recurse
	template <size_t foo>
	struct remove_recurse<numLevels-1,foo>
	{
		template <typename T>
		static size_t apply(type_this* that, T begin, T end, size_t& count)
		{
			for(auto i = begin; i != end; ++i)
			{
				that->remove_node(*i);
				++count;
			}
			return count;
		}
	};

	template <size_t i, size_t n>
	struct get_recurse
	{
		static Node<i+n>* apply(const type_this* that, const KeyType* s, Node<i>* root)
		{
			// TODO: We probably don't need to check if root is a valid simplex (10)
			if(root)
			{
				auto p = root->_up.find(*s);
				if(p != root->_up.end())
				{
					return get_recurse<i+1,n-1>::apply(that, s+1, root->_up[*s]);
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}
	};

	template <size_t i>
	struct  get_recurse<i,0>
	{
		static Node<i>* apply(const type_this* that, const KeyType* s, Node<i>* root)
		{
			return root;
		}
	};

	template <size_t i, size_t n>
	struct get_down_recurse
	{
		static Node<i-n>* apply(const type_this* that, const KeyType* s, Node<i>* root)
		{
			if(root)
			{
				auto p = root->_down.find(*s);
				if(p != root->_down.end())
				{
					return get_recurse<i-1,n-1>::apply(that, s+1, root->_down[*s]);
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}
	};

	template <size_t i>
	struct  get_down_recurse<i,0>
	{
		static Node<i>* apply(const type_this* that, const KeyType* s, Node<i>* root)
		{
			return root;
		}
	};

	template <size_t level, size_t n>
	struct insert_full
	{
		static Node<level+n>* apply(type_this* that, Node<level>* root, const KeyType* begin)
		{
			return insert_for<level, n, n>::apply(that, root, begin);
		}
	};

	template <size_t level>
	struct insert_full<level,0>
	{
		static Node<level>* apply(type_this* that, Node<level>* root, const KeyType* begin)
		{
			return root;
		}
	};

	template <size_t level, size_t antistep, size_t n>
	struct insert_for
	{
		static Node<level+n>* apply(type_this* that, Node<level>* root, const KeyType* begin)
		{
			insert_raw<level, n-antistep>::apply(that, root, begin);
			return insert_for<level, antistep-1, n>::apply(that, root, begin);
		}
	};

	template <size_t level, size_t n>
	struct insert_for<level,1,n>
	{
		static Node<level+n>* apply(type_this* that, Node<level>* root, const KeyType* begin){
			return insert_raw<level, n-1>::apply(that, root, begin);
		}
	};

	/**
	 * @brief      Actually insert the node and connect up and down. 
	 *
	 * @tparam     level  { description }
	 * @tparam     n      { description }
	 */
	template <size_t level, size_t n>
	struct insert_raw
	{
		static Node<level+n+1>* apply(type_this* that, Node<level>* root, const KeyType* begin)
		{

			KeyType v = *(begin+n);
			Node<level+1>* nn;
			// if root->v doesn't exist then create it
			auto iter = root->_up.find(v);
			if(iter == root->_up.end())
			{
				nn = that->create_node(std::integral_constant<std::size_t,level+1>());

				nn->_down[v] = root;
				root->_up[v] = nn;
				that->backfill(root, nn, v);
			}
			else
				nn = iter->second;	// otherwise get it
			return insert_full<level+1,n>::apply(that, nn, begin);
		}
	};

    /**
     *  @brief Backfill in the pointers from prior nodes to the new node
     *  @param root is a parent node
     *  @param nn is the new child node
     *  @param value is the exposed id of nn
     *  @return void
     */
	template <size_t level>
	void backfill(Node<level>* root, Node<level+1>* nn, KeyType value)
	{
		for(auto curr = root->_down.begin(); curr != root->_down.end(); ++curr)
		{
			int v = curr->first;

			Node<level-1>* parent = curr->second;
			Node<level>* child = parent->_up[value]; 

			nn->_down[v] = child;
			child->_up[v] = nn;
		}
	}

    /**
     *  @brief Fill in the pointers from level 1 to 0.
     *  @param root is a level 0 node
     *  @param nn is a level 1 node
     *  @param value is the exposed id of nn
     *  @return void
     */
	void backfill(Node<0>* root, Node<1>* nn, int value)
	{
		return;
	}

	template <size_t level>
	Node<level>* create_node(std::integral_constant<std::size_t,level> x)
	{
		auto p = new Node<level>(node_count++);
		++(level_count[level]);

	    bool ret = std::get<level>(levels).insert(
	    		std::pair<size_t,NodePtr<level>>(node_count-1, p)).second; // node_count-1 to match the id's correctly
	    assert(ret);
	    /*
        // sanity check to make sure there aren't duplicate keys... 
        if (ret==false) {
            std::cout << "Error: Node '" << node_count << "' already existed with value " << *p << std::endl;
        }
        */
		return p;
	}

	template <size_t level>
	void remove_node(Node<level>* p)
	{
		for(auto curr = p->_down.begin(); curr != p->_down.end(); ++curr)
		{
			curr->second->_up.erase(curr->first);
		}
		for(auto curr = p->_up.begin(); curr != p->_up.end(); ++curr)
		{
			curr->second->_down.erase(curr->first);
		}
		--(level_count[level]);
		std::get<level>(levels).erase(p->_node);
		delete p;
	}

	void remove_node(Node<1>* p)
	{
		// This for loop should only have a single iteration.
		for(auto curr = p->_down.begin(); curr != p->_down.end(); ++curr)
		{
			unused_vertices.insert(curr->first);
			curr->second->_up.erase(curr->first);
		}
		for(auto curr = p->_up.begin(); curr != p->_up.end(); ++curr)
		{
			curr->second->_down.erase(curr->first);
		}
		--(level_count[1]);
		std::get<1>(levels).erase(p->_node);
		delete p;
	}

	void remove_node(Node<0>* p)
	{
		for(auto curr = p->_up.begin(); curr != p->_up.end(); ++curr)
		{
			curr->second->_down.erase(curr->first);
		}
		--(level_count[0]);
		std::get<0>(levels).erase(p->_node);
		delete p;
	}

	void remove_node(Node<topLevel>* p)
	{
		for(auto curr = p->_down.begin(); curr != p->_down.end(); ++curr)
		{
			curr->second->_up.erase(curr->first);
		}
		--(level_count[topLevel]);
		std::get<topLevel>(levels).erase(p->_node);
		delete p;
	}

	NodePtr<0> _root;
	size_t node_count;
	std::array<size_t,numLevels> level_count;
	
	using NodePtrLevel = typename util::int_type_map<std::size_t, std::tuple, LevelIndex, NodePtr>::type;
	typename util::type_map<NodePtrLevel, detail::map>::type levels;
	index_tracker<KeyType> unused_vertices;
};


/**
 * Definition to help unpack explicit call of 
 * AbstractSimplicalComplex with full traits list.
 */
template <typename KeyType, typename... Ts>
using AbstractSimplicialComplex = simplicial_complex<simplicial_complex_traits_default<KeyType,Ts...>>;



/**
 * @brief      Push the immediate face neighbors into the provided iterator
 *
 * @param      F           The full complex
 * @param[in]  nid  	   Simplex to get neighbors of
 * @param[in]  iter        The iterator
 *
 * @tparam     Complex     Type of the simplicial complex
 * @tparam     level       The integral level of the node
 * @tparam     InsertIter  Iterator type
 */
template <class Complex, std::size_t level, class InsertIter>
void neighbors(Complex &F, typename Complex::template SimplexID<level> nid, InsertIter iter)
{
	for (auto a : F.get_name(nid))
	{
		auto id = F.get_simplex_down(nid,a);
		for(auto b : F.get_cover(id))
		{
			auto nbor = F.get_simplex_up(id,b);
			if(nbor != nid)
			{
				*iter++ = nbor;
			}
		}
	}
}

/**
 * @brief      This is a helper function to assist neighbors to automatically deduce the integral level.
 */
template <class Complex, class SimplexID, class InsertIter>
void neighbors(Complex& F, SimplexID nid, InsertIter iter)
{
	neighbors<Complex, SimplexID::level, InsertIter>(F,nid,iter);
}


/**
 * @brief      Push the immediate coface neighbors into the provided iterator 
 *
 * @param      F           The full complex
 * @param[in]  nid  	   Simplex to get neighbors of
 * @param[in]  iter        The iterator
 *
 * @tparam     Complex     Type of the simplicial complex
 * @tparam     level       The integral level of the node
 * @tparam     InsertIter  Iterator type
 */
template <class Complex, std::size_t level, class InsertIter>
void neighbors_up(Complex &F, typename Complex::template SimplexID<level> nid, InsertIter iter)
{
	for (auto a : F.get_cover(nid))
	{
		auto id = F.get_simplex_up(nid,a);
		for(auto b : F.get_name(id))
		{
			auto nbor = F.get_simplex_down(id,b);
			if(nbor != nid)
			{
				*iter++ = nbor;
			}
		}
	}
}

/**
 * @brief      This is a helper function to assist neighbors to automatically deduce the integral level.
 */
template <class Complex, class SimplexID, class InsertIter>
void neighbors_up(Complex& F, SimplexID nid, InsertIter iter)
{
	neighbors_up<Complex, SimplexID::level, InsertIter>(F,nid,iter);
}

/**
 * TODO: convert this to use an iterator inserter (1)
 * Code for returning a set of k-ring neighbors. Currently obseleted by neighbors_up visitor pattern
 */
template <class Complex, std::size_t level>
std::set<typename Complex::template SimplexID<level>> kneighbors_up(Complex &F, 
			std::set<typename Complex::template SimplexID<level>>& nodes,
			std::set<typename Complex::template SimplexID<level>> next,
			int ring)
{
	if (ring == 0)
		return nodes;
	std::set<typename Complex::template SimplexID<level>> tmp;
	for (auto nid : next){
		for (auto a : F.get_cover(nid))
		{
			auto id = F.get_simplex_up(nid,a);
			for(auto b : F.get_name(id))
			{
				auto nbor = F.get_simplex_down(id,b);
				if(nodes.insert(nbor).second){
					tmp.insert(nbor);
				}
			}
		}
	}
	return neighbors_up<Complex, level>(F, nodes, tmp, ring-1);
}

template <class Complex, class SimplexID>
std::set<SimplexID> kneighbors_up(Complex& F, SimplexID nid, int ring)
{
	std::set<SimplexID> nodes{nid};
	return neighbors_up<Complex, SimplexID::level>(F,nodes,nodes,ring);
}

template <typename SimplexID>
struct hashSimplexID{
   size_t operator()(const SimplexID nid) const
   {
       return std::hash<std::uintptr_t>()(static_cast<uintptr_t>(nid));
   }
};

template <typename T> using NodeSet = std::unordered_set<T,hashSimplexID<T>>;
