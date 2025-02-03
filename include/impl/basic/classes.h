#ifndef BA_GRAPH_CLASSES_H
#define BA_GRAPH_CLASSES_H
#include <type_traits>
#include <concepts>

namespace ba_graph {
class Vertex;
class Edge;
class HalfEdge;

class Number;
class Location;

class Incidence;
class IncidenceIterator;
class Rotation;
class RotationIterator;
class Graph;

class Factory;

template<typename T>
concept IncidencePredicate = requires(T a, const Incidence &i) {
    { a(i) } -> std::convertible_to<bool>;
};

template<typename T>
concept RotationPredicate = requires(T a, const Rotation &i) {
    {a(i)} -> std::convertible_to<bool>;
};


template<typename T>
concept IncidenceFunction = requires(T a, IncidenceIterator it) {
    {a(it)} -> std::same_as<decltype(a(it))>;
};


template<typename T>
concept RotationFunction = requires(T a, RotationIterator it) {
    {a(it)} -> std::same_as<decltype(a(it))>;
};


template<typename T>
concept IncidenceComparator = requires(T a, IncidenceIterator i1, IncidenceIterator i2) {
    {a(i1,i2)} -> std::convertible_to<bool>;
};

template<typename T>
concept RotationComparator = requires(T a, RotationIterator r1, RotationIterator r2) {
    {a(r1, r2)} -> std::convertible_to<bool>;
};



namespace RP {
template<typename T> class RP;
}
namespace IP {
template<typename T> class IP;
}

template<typename T>
concept RotationLocatable = requires(T a, const Incidence &b) {
    {(IP::IP<typename std::decay<T>::type>(a))(b)} -> std::convertible_to<bool>;
};

template<typename T>
concept GraphLocatable = requires(T a, const Rotation &b) {
    {(RP::RP<typename std::decay<T>::type>(a))(b)} -> std::convertible_to<bool>;
};


}

#endif
