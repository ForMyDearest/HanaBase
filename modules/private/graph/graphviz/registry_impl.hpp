#pragma once

#include "fwd.hpp"

#include <vector>

namespace hana::graphviz
{
    struct RegistryImpl final : Registry {
        ~RegistryImpl() noexcept override;

        Node* create_node(HStringView id) noexcept override;
        Edge* create_edge(const Node* from, const Node* to) noexcept override;
        Port* create_port(HStringView id) noexcept override;
        Port* create_port(HStringView id, Compass compass) noexcept override;
        Subgraph* create_subgraph(HStringView id) noexcept override;
        Graph* create_graph(HStringView id) noexcept override;

        std::vector<Port*> ports_;
        std::vector<NodeImpl*> nodes_;
        std::vector<EdgeImpl*> edges_;
        std::vector<GraphImpl*> graphs_;
        std::vector<SubgraphImpl*> subgraphs_;
    };
}
