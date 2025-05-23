#pragma once

#include "cluster.hpp"

namespace hana::graphviz
{
    struct GraphImpl final : Graph {
        explicit GraphImpl(std::string_view id) noexcept;
        ~GraphImpl() noexcept override;

        Graph* attach_node(const Node* node) noexcept override;
        Graph* detach_node(const Node* node) noexcept override;
        Graph* clear_nodes() noexcept override;

        Graph* attach_edge(const Edge* edge) noexcept override;
        Graph* detach_edge(const Edge* edge) noexcept override;
        Graph* clear_edges() noexcept override;

        Graph* attach_subgraph(const Subgraph* subgraph) noexcept override;
        Graph* detach_subgraph(const Subgraph* subgraph) noexcept override;
        Graph* clear_subgraphs() noexcept override;

        Graph* add_graph_attribute(GraphAttr key, HStringView value) noexcept override;
        Graph* remove_graph_attribute(GraphAttr key) noexcept override;
        Graph* clear_graph_attributes() noexcept override;

        Graph* add_node_attribute(NodeAttr key, HStringView value) noexcept override;
        Graph* remove_node_attribute(NodeAttr key) noexcept override;
        Graph* clear_node_attributes() noexcept override;

        Graph* add_edge_attribute(EdgeAttr key, HStringView value) noexcept override;
        Graph* remove_edge_attribute(EdgeAttr key) noexcept override;
        Graph* clear_edge_attributes() noexcept override;

        void dump(HStringView filename, bool is_digraph) const noexcept override;

        Cluster cluster_;
        attr_map_t<GraphAttr>* graph_attributes_{nullptr};
    };
}
