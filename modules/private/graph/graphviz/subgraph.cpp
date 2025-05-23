#include "dumper.hpp"
#include "subgraph_impl.hpp"

namespace hana::graphviz
{
	SubgraphImpl::SubgraphImpl(std::string_view id) noexcept: cluster_(id) {}

	SubgraphImpl::~SubgraphImpl() noexcept {
		delete subgraph_attributes_;
	}

	Subgraph* SubgraphImpl::attach_node(const Node* node) noexcept {
		cluster_.attach_node(reinterpret_cast<const NodeImpl*>(node));
		return this;
	}

	Subgraph* SubgraphImpl::detach_node(const Node* node) noexcept {
		cluster_.detach_node(reinterpret_cast<const NodeImpl*>(node));
		return this;
	}

	Subgraph* SubgraphImpl::clear_nodes() noexcept {
		cluster_.clear_nodes();
		return this;
	}

	Subgraph* SubgraphImpl::attach_edge(const Edge* edge) noexcept {
		cluster_.attach_edge(reinterpret_cast<const EdgeImpl*>(edge));
		return this;
	}

	Subgraph* SubgraphImpl::detach_edge(const Edge* edge) noexcept {
		cluster_.detach_edge(reinterpret_cast<const EdgeImpl*>(edge));
		return this;
	}

	Subgraph* SubgraphImpl::clear_edges() noexcept {
		cluster_.clear_edges();
		return this;
	}

	Subgraph* SubgraphImpl::attach_subgraph(const Subgraph* subgraph) noexcept {
		cluster_.attach_subgraph(reinterpret_cast<const SubgraphImpl*>(subgraph));
		return this;
	}

	Subgraph* SubgraphImpl::detach_subgraph(const Subgraph* subgraph) noexcept {
		cluster_.detach_subgraph(reinterpret_cast<const SubgraphImpl*>(subgraph));
		return this;
	}

	Subgraph* SubgraphImpl::clear_subgraphs() noexcept {
		cluster_.clear_subgraphs();
		return this;
	}

	Subgraph* SubgraphImpl::add_subgraph_attribute(SubgraphAttr key, HStringView value) noexcept {
		if (!subgraph_attributes_) {
			subgraph_attributes_ = new attr_map_t<SubgraphAttr>;
		}
		subgraph_attributes_->emplace(key, value.raw_view());
		return this;
	}

	Subgraph* SubgraphImpl::remove_subgraph_attribute(SubgraphAttr key) noexcept {
		if (subgraph_attributes_) {
			subgraph_attributes_->erase(key);
		}
		return this;
	}

	Subgraph* SubgraphImpl::clear_subgraph_attributes() noexcept {
		if (subgraph_attributes_) {
			subgraph_attributes_->clear();
		}
		return this;
	}

	Subgraph* SubgraphImpl::add_node_attribute(NodeAttr key, HStringView value) noexcept {
		cluster_.add_node_attribute(key, value.raw_view());
		return this;
	}

	Subgraph* SubgraphImpl::remove_node_attribute(NodeAttr key) noexcept {
		cluster_.remove_node_attribute(key);
		return this;
	}

	Subgraph* SubgraphImpl::clear_node_attributes() noexcept {
		cluster_.clear_node_attributes();
		return this;
	}

	Subgraph* SubgraphImpl::add_edge_attribute(EdgeAttr key, HStringView value) noexcept {
		cluster_.add_edge_attribute(key, value.raw_view());
		return this;
	}

	Subgraph* SubgraphImpl::remove_edge_attribute(EdgeAttr key) noexcept {
		cluster_.remove_edge_attribute(key);
		return this;
	}

	Subgraph* SubgraphImpl::clear_edge_attributes() noexcept {
		cluster_.clear_edge_attributes();
		return this;
	}

	void SubgraphImpl::dump(Dumper& dumper) const noexcept {
		dumper.dump_header<true>(cluster_);

		if (subgraph_attributes_) {
			dumper.dump_attributes("graph", *subgraph_attributes_);
		}
		if (cluster_.node_attributes_) {
			dumper.dump_attributes("node", *cluster_.node_attributes_);
		}
		if (cluster_.edge_attributes_) {
			dumper.dump_attributes("edge", *cluster_.edge_attributes_);
		}

		dumper.dump_cluster<true>(cluster_);
	}
}
