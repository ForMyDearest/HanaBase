#pragma once

#include <map>

namespace hana::graphviz
{
    template<typename T>
    using attr_map_t = std::map<T, std::string_view, std::less<>>;

    struct Dumper;
    struct Cluster;
    struct RegistryImpl;
    struct NodeImpl;
    struct EdgeImpl;
    struct SubgraphImpl;
    struct GraphImpl;
}
