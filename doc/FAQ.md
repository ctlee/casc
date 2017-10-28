# Frequently Asked Questions

1. Why is my simplex data not storing correctly?

If you are retrieving the data from the `SimplexID` using the dereference
operator, you must retrieve the result as a reference in order to modify it. See
the following example.

```
MeshType mesh = MeshType();
int key = mesh.insert({1}, 10);
auto data = *mesh.get_simplex_up({key});
data = 5;
std::cout <<*mesh.get_simplex_up({key}); << std::endl // Prints 10

auto &dataRef = *mesh.get_simplex_up({key});
dataRef = 5;
std::cout << *mesh.get_simplex_up({key}) << std::endl // Prints 5
```
