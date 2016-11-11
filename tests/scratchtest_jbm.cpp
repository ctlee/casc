/**
 * This is a scratch space for testing new functions and such on the fly.
 */

#include "SurfaceMesh.h"
#include "Vertex.h"
#include <vector>
#include <iostream>
#include <string>
#include <type_traits>
#include <list>

/*

template <std::size_t dimension>
auto getTangentH(SurfaceMesh& mesh, const tensor<double, dimension, 1>& origin, SurfaceMesh::NodeID<SurfaceMesh::topLevel> curr)
{
    return 1.0;
}

template <std::size_t level, std::size_t dimension>
auto getTangentH(SurfaceMesh& mesh, const tensor<double, dimension, 1>& origin, SurfaceMesh::NodeID<level> curr)
{
    tensor<double, dimension, SurfaceMesh::topLevel - level> rval;

    for(auto alpha : mesh.get_cover(curr))
    {
        auto edge = *mesh.get_edge_up(curr, alpha);
        const auto& v = (*mesh.get_node_up({alpha})).position;
        auto next = mesh.get_node_up(curr,alpha);
        rval += edge.orientation * (v-origin) * getTangentH(mesh, origin, next);
    }

    return rval;
}

auto getTangent_jbm(SurfaceMesh& mesh, SurfaceMesh::NodeID<1> vertexID)
{
    return getTangentH(mesh, (*vertexID).position, vertexID);
}

template <class Complex>
void SimplexCollapse(const Complex& F)
{

}
*/

template <typename T, std::size_t k>
std::ostream& operator<<(std::ostream& out, const std::array<T,k>& A)
{
    out << "[";
    for(int i = 0; i + 1 < k; ++i)
    {
        out << A[i] << " ";
    }
    if(k > 0)
    {
        out << A[k-1];
    }
    out << "]";
    return out;
}

template <typename T>
void print_name(const T& name)
{
    for(auto x : name)
    {
        std::cout << x << " ";
    }
}

template <typename NodeID>
void grab(const typename NodeID::complex& F, NodeID s)
{
    std::cout << F.get_name(s) << std::endl;
}

template <typename Visitor, typename Traits, typename Complex, typename K>
struct Visit_UpBFS {};

template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct Visit_UpBFS<Visitor, Traits, Complex, std::integral_constant<std::size_t,k>>
{
    static constexpr auto level = k;
    using CurrNodeID = typename Complex::template NodeID<level>;
    using NextNodeID = typename Complex::template NodeID<level+1>;

    template <typename T> using Container = typename Traits::template Container<T>;

    template <typename Iterator>
    static void apply(Visitor& v, const Complex& F, Iterator begin, Iterator end)
    {
        Container<NextNodeID> next;
        std::vector<typename Complex::KeyType> cover;

        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);

            F.get_cover(*curr, std::back_inserter(cover));
            for(auto a : cover)
            {
                NextNodeID id = F.get_node_up(*curr,a);
                next.insert(next.end(), id);
            }
            cover.clear();
        }

        Visit_UpBFS<Visitor,Traits,Complex,std::integral_constant<std::size_t,level+1>>::apply(v, F, next.begin(), next.end());
    }
};

template <typename Visitor, typename Traits, typename Complex>
struct Visit_UpBFS<Visitor, Traits, Complex, std::integral_constant<std::size_t, Complex::topLevel>>
{
    static constexpr auto level = Complex::topLevel;
    using CurrNodeID = typename Complex::template NodeID<level>;

    template <typename Iterator>
    static void apply(Visitor& v, const Complex& F, Iterator begin, Iterator end)
    {
        std::vector<typename Complex::KeyType> cover;

        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);
        }
    }
};




template <typename Visitor, typename Traits, typename Complex, typename K>
struct Visit_Edge {};

template <typename Visitor, typename Traits, typename Complex, std::size_t k>
struct Visit_Edge<Visitor, Traits, Complex, std::integral_constant<std::size_t,k>>
{
    static constexpr auto level = k;
    using CurrEdgeID = typename Complex::template EdgeID<level>;
    using NextEdgeID = typename Complex::template EdgeID<level+1>;
    using CurrNodeID = typename Complex::template NodeID<level>;

    template <typename T> using Container = typename Traits::template Container<T>;

    template <typename Iterator>
    static void apply(Visitor& v, const Complex& F, Iterator begin, Iterator end)
    {
        Container<NextEdgeID> next;
        std::vector<typename Complex::KeyType> cover;

        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);

            CurrNodeID n = curr->up();
            F.get_cover(n, std::back_inserter(cover));
            for(auto a : cover)
            {
                NextEdgeID id = F.get_edge_up(n,a);
                next.insert(next.end(), id);
            }
            cover.clear();
        }

        Visit_Edge<Visitor,Traits,Complex,std::integral_constant<std::size_t,level+1>>::apply(v, F, next.begin(), next.end());
    }
};

template <typename Visitor, typename Traits, typename Complex>
struct Visit_Edge<Visitor, Traits, Complex, std::integral_constant<std::size_t, Complex::topLevel>>
{
    static constexpr auto level = Complex::topLevel;
    using CurrEdgeID = typename Complex::template EdgeID<level>;

    template <typename Iterator>
    static void apply(Visitor& v, const Complex& F, Iterator begin, Iterator end)
    {
        std::vector<typename Complex::KeyType> cover;

        for(auto curr = begin; curr != end; ++curr)
        {
            v.visit(F, *curr);
        }
    }
};




template <typename T> using AllowRepeat = std::vector<T>;

struct VisitUpBFS_NoRepeat_Traits
{
    template <typename T> using Container = std::set<T>;
};

template <typename Visitor, typename NodeID>
void work_up(Visitor v, const typename NodeID::complex& F, NodeID s)
{
    Visit_UpBFS<Visitor, VisitUpBFS_NoRepeat_Traits, typename NodeID::complex, std::integral_constant<std::size_t,NodeID::level>>::apply(v,F,&s,&s+1);
}

template <typename Visitor, typename NodeID>
void edge_up(Visitor v, const typename NodeID::complex& F, NodeID s)
{
    Visit_Edge<Visitor, VisitUpBFS_NoRepeat_Traits, typename NodeID::complex, std::integral_constant<std::size_t,NodeID::level>>::apply(v,F,&s,&s+1);
}


template <typename Complex>
struct PrintEdgeVisitor
{
    template <std::size_t level>
    void visit(const Complex& F, typename Complex::template EdgeID<level> s)
    {
        auto down = s.down();
        std::cout << F.get_name(down);
        std::cout << " -> ";
        std::cout << F.get_name(s.up()) << std::endl;
    }
};
template <typename Complex>
PrintEdgeVisitor<Complex> make_print_edge_visitor(const Complex& F)
{
    return PrintEdgeVisitor<Complex>();
}


template <typename Complex>
struct PrintVisitor
{
    template <std::size_t level>
    void visit(const Complex& F, typename Complex::template NodeID<level> s)
    {
        std::cout << F.get_name(s) << std::endl;
    }
};

template <typename Complex>
PrintVisitor<Complex> make_print_visitor(const Complex& F)
{
    return PrintVisitor<Complex>();
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "Wrong arguments passed" << std::endl;
        return -1;
    }
    std::cout << "Begin reading Mesh..." << std::endl;
    auto result = readOFF(argv[1]);

    if(result.second == false)
    {
        std::cout << "Something bad happened...";
        exit(1);
    }
    auto mesh = result.first;

    init_orientation(*mesh);
    clear_orientation(*mesh);
    compute_orientation(*mesh);
    
    {
        auto s = *(++++mesh->get_level_id<1>().begin());

        edge_up(make_print_edge_visitor(*mesh), *mesh, mesh->get_edge_up(mesh->get_node_up(),1));
//        edge_up(make_print_edge_visitor(*mesh), *mesh, mesh->get_edge_up(s,1));//mesh->get_node_up(),1));
        /*
        auto edges = mesh->up(v);
        auto faces = mesh->up(edges);

        std::cout << v << std::endl;
        for(auto f : faces)
        {
            std::cout << "    " << f << std::endl;
        }
        */
    }
}
