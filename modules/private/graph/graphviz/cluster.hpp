#pragma once

#include "fwd.hpp"

#include <unordered_set>

namespace hana::graphviz
{
    struct Cluster {
        explicit Cluster(std::string_view id) noexcept;
        ~Cluster() noexcept;

        void attach_node(const NodeImpl* node) noexcept;
        void detach_node(const NodeImpl* node) noexcept;
        void clear_nodes() noexcept;

        void attach_edge(const EdgeImpl* edge) noexcept;
        void detach_edge(const EdgeImpl* edge) noexcept;
        void clear_edges() noexcept;

        void attach_subgraph(const SubgraphImpl* subgraph) noexcept;
        void detach_subgraph(const SubgraphImpl* subgraph) noexcept;
        void clear_subgraphs() noexcept;

        void add_node_attribute(NodeAttr key, std::string_view value) noexcept;
        void remove_node_attribute(NodeAttr key) noexcept;
        void clear_node_attributes() noexcept;

        void add_edge_attribute(EdgeAttr key, std::string_view value) noexcept;
        void remove_edge_attribute(EdgeAttr key) noexcept;
        void clear_edge_attributes() noexcept;

        std::string_view id_;
        attr_map_t<NodeAttr>* node_attributes_{nullptr};
        attr_map_t<EdgeAttr>* edge_attributes_{nullptr};
        std::unordered_set<const NodeImpl*> nodes_;
        std::unordered_set<const EdgeImpl*> edges_;
        std::unordered_set<const SubgraphImpl*> subgraphs_;
    };
}
