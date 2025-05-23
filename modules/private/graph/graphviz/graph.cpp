#include "dumper.hpp"
#include "graph_impl.hpp"

namespace hana::graphviz
{
    GraphImpl::GraphImpl(std::string_view id) noexcept: cluster_(id) {}

    GraphImpl::~GraphImpl() noexcept {
        delete graph_attributes_;
    }

    Graph* GraphImpl::attach_node(const Node* node) noexcept {
        cluster_.attach_node(reinterpret_cast<const NodeImpl*>(node));
        return this;
    }

    Graph* GraphImpl::detach_node(const Node* node) noexcept {
        cluster_.detach_node(reinterpret_cast<const NodeImpl*>(node));
        return this;
    }

    Graph* GraphImpl::clear_nodes() noexcept {
        cluster_.clear_nodes();
        return this;
    }

    Graph* GraphImpl::attach_edge(const Edge* edge) noexcept {
        cluster_.attach_edge(reinterpret_cast<const EdgeImpl*>(edge));
        return this;
    }

    Graph* GraphImpl::detach_edge(const Edge* edge) noexcept {
        cluster_.detach_edge(reinterpret_cast<const EdgeImpl*>(edge));
        return this;
    }

    Graph* GraphImpl::clear_edges() noexcept {
        cluster_.clear_edges();
        return this;
    }

    Graph* GraphImpl::attach_subgraph(const Subgraph* subgraph) noexcept {
        cluster_.attach_subgraph(reinterpret_cast<const SubgraphImpl*>(subgraph));
        return this;
    }

    Graph* GraphImpl::detach_subgraph(const Subgraph* subgraph) noexcept {
        cluster_.detach_subgraph(reinterpret_cast<const SubgraphImpl*>(subgraph));
        return this;
    }

    Graph* GraphImpl::clear_subgraphs() noexcept {
        cluster_.clear_subgraphs();
        return this;
    }

    Graph* GraphImpl::add_graph_attribute(GraphAttr key, HStringView value) noexcept {
        if (!graph_attributes_) {
            graph_attributes_ = new attr_map_t<GraphAttr>;
        }
        graph_attributes_->emplace(key, value.raw_view());
        return this;
    }

    Graph* GraphImpl::remove_graph_attribute(GraphAttr key) noexcept {
        if (graph_attributes_) {
            graph_attributes_->erase(key);
        }
        return this;
    }

    Graph* GraphImpl::clear_graph_attributes() noexcept {
        if (graph_attributes_) {
            graph_attributes_->clear();
        }
        return this;
    }

    Graph* GraphImpl::add_node_attribute(NodeAttr key, HStringView value) noexcept {
        cluster_.add_node_attribute(key, value.raw_view());
        return this;
    }

    Graph* GraphImpl::remove_node_attribute(NodeAttr key) noexcept {
        cluster_.remove_node_attribute(key);
        return this;
    }

    Graph* GraphImpl::clear_node_attributes() noexcept {
        cluster_.clear_node_attributes();
        return this;
    }

    Graph* GraphImpl::add_edge_attribute(EdgeAttr key, HStringView value) noexcept {
        cluster_.add_edge_attribute(key, value.raw_view());
        return this;
    }

    Graph* GraphImpl::remove_edge_attribute(EdgeAttr key) noexcept {
        cluster_.remove_edge_attribute(key);
        return this;
    }

    Graph* GraphImpl::clear_edge_attributes() noexcept {
        cluster_.clear_edge_attributes();
        return this;
    }

    void GraphImpl::dump(HStringView filename, bool is_digraph) const noexcept {
        Dumper dumper(filename, is_digraph);

        if (is_digraph) {
            dumper.ofs_ << "strict di";
        } else {
            dumper.ofs_ << "strict ";
        }
        dumper.dump_header<false>(cluster_);

        if (graph_attributes_) {
            dumper.dump_attributes("graph", *graph_attributes_);
        }
        if (cluster_.node_attributes_) {
            dumper.dump_attributes("node", *cluster_.node_attributes_);
        }
        if (cluster_.edge_attributes_) {
            dumper.dump_attributes("edge", *cluster_.edge_attributes_);
        }

        dumper.dump_cluster<false>(cluster_);
    }
}
