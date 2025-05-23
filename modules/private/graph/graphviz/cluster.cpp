#include "cluster.hpp"

namespace hana::graphviz
{
    Cluster::Cluster(std::string_view id) noexcept: id_(id) {}

    Cluster::~Cluster() noexcept {
        delete node_attributes_;
        delete edge_attributes_;
    }

    void Cluster::attach_node(const NodeImpl* node) noexcept {
        nodes_.insert(node);
    }

    void Cluster::detach_node(const NodeImpl* node) noexcept {
        nodes_.erase(node);
    }

    void Cluster::clear_nodes() noexcept {
        nodes_.clear();
    }

    void Cluster::attach_edge(const EdgeImpl* edge) noexcept {
        edges_.insert(edge);
    }

    void Cluster::detach_edge(const EdgeImpl* edge) noexcept {
        edges_.erase(edge);
    }

    void Cluster::clear_edges() noexcept {
        edges_.clear();
    }

    void Cluster::attach_subgraph(const SubgraphImpl* subgraph) noexcept {
        subgraphs_.insert(subgraph);
    }

    void Cluster::detach_subgraph(const SubgraphImpl* subgraph) noexcept {
        subgraphs_.erase(subgraph);
    }

    void Cluster::clear_subgraphs() noexcept {
        subgraphs_.clear();
    }

    void Cluster::add_node_attribute(NodeAttr key, std::string_view value) noexcept {
        if (!node_attributes_) {
            node_attributes_ = new attr_map_t<NodeAttr>;
        }
        node_attributes_->emplace(key, value);
    }

    void Cluster::remove_node_attribute(NodeAttr key) noexcept {
        if (node_attributes_) {
            node_attributes_->erase(key);
        }
    }

    void Cluster::clear_node_attributes() noexcept {
        if (node_attributes_) {
            node_attributes_->clear();
        }
    }

    void Cluster::add_edge_attribute(EdgeAttr key, std::string_view value) noexcept {
        if (!edge_attributes_) {
            edge_attributes_ = new attr_map_t<EdgeAttr>;
        }
        edge_attributes_->emplace(key, value);
    }

    void Cluster::remove_edge_attribute(EdgeAttr key) noexcept {
        if (edge_attributes_) {
            edge_attributes_->erase(key);
        }
    }

    void Cluster::clear_edge_attributes() noexcept {
        if (edge_attributes_) {
            edge_attributes_->clear();
        }
    }
}
