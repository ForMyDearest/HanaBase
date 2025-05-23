#pragma once

#include "cluster.hpp"

namespace hana::graphviz
{
    struct SubgraphImpl : Subgraph {
        explicit SubgraphImpl(std::string_view id) noexcept;
        ~SubgraphImpl() noexcept override;

        Subgraph* attach_node(const Node* node) noexcept override;
        Subgraph* detach_node(const Node* node) noexcept override;
        Subgraph* clear_nodes() noexcept override;

        Subgraph* attach_edge(const Edge* edge) noexcept override;
        Subgraph* detach_edge(const Edge* edge) noexcept override;
        Subgraph* clear_edges() noexcept override;

        Subgraph* attach_subgraph(const Subgraph* subgraph) noexcept override;
        Subgraph* detach_subgraph(const Subgraph* subgraph) noexcept override;
        Subgraph* clear_subgraphs() noexcept override;

        Subgraph* add_subgraph_attribute(SubgraphAttr key, HStringView value) noexcept override;
        Subgraph* remove_subgraph_attribute(SubgraphAttr key) noexcept override;
        Subgraph* clear_subgraph_attributes() noexcept override;

        Subgraph* add_node_attribute(NodeAttr key, HStringView value) noexcept override;
        Subgraph* remove_node_attribute(NodeAttr key) noexcept override;
        Subgraph* clear_node_attributes() noexcept override;

        Subgraph* add_edge_attribute(EdgeAttr key, HStringView value) noexcept override;
        Subgraph* remove_edge_attribute(EdgeAttr key) noexcept override;
        Subgraph* clear_edge_attributes() noexcept override;

        void dump(Dumper& dumper) const noexcept;

        Cluster cluster_;
        attr_map_t<SubgraphAttr>* subgraph_attributes_{nullptr};
    };
}
