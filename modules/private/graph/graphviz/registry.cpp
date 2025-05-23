#include "port.hpp"
#include "node_impl.hpp"
#include "edge_impl.hpp"
#include "graph_impl.hpp"
#include "subgraph_impl.hpp"
#include "registry_impl.hpp"

#include <cassert>

namespace hana::graphviz
{
	RegistryImpl::~RegistryImpl() noexcept {
		for (const auto* ptr: ports_) delete ptr;
		for (const auto* ptr: nodes_) delete ptr;
		for (const auto* ptr: edges_) delete ptr;
		for (const auto* ptr: graphs_) delete ptr;
		for (const auto* ptr: subgraphs_) delete ptr;
	}

	Node* RegistryImpl::create_node(HStringView id) noexcept {
		auto* ret = new NodeImpl(id.raw_view());
		nodes_.emplace_back(ret);
		return ret;
	}

	Edge* RegistryImpl::create_edge(const Node* from, const Node* to) noexcept {
		assert(from && "from node can't be nullptr");
		assert(to && "to node can't be nullptr");

		auto* ret = new EdgeImpl(reinterpret_cast<const NodeImpl*>(from), reinterpret_cast<const NodeImpl*>(to));
		edges_.emplace_back(ret);
		return ret;
	}

	Port* RegistryImpl::create_port(HStringView id) noexcept {
		auto* ret = new Port(id.raw_view());
		ports_.emplace_back(ret);
		return ret;
	}

	Port* RegistryImpl::create_port(HStringView id, Compass compass) noexcept {
		auto* ret = new Port(id.raw_view(), compass);
		ports_.emplace_back(ret);
		return ret;
	}

	Subgraph* RegistryImpl::create_subgraph(HStringView id) noexcept {
		auto* ret = new SubgraphImpl(id.raw_view());
		subgraphs_.emplace_back(ret);
		return ret;
	}

	Graph* RegistryImpl::create_graph(HStringView id) noexcept {
		auto* ret = new GraphImpl(id.raw_view());
		graphs_.emplace_back(ret);
		return ret;
	}
}

namespace hana::graphviz
{
	std::unique_ptr<Registry> Registry::create() noexcept {
		return std::make_unique<RegistryImpl>();
	}
}
