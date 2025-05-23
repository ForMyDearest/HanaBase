#include "attributes_str.hpp"

namespace hana::graphviz
{
	std::string_view to_string(GraphAttr attr) noexcept {
		return graph_str[static_cast<int>(attr)];
	}

	std::string_view to_string(NodeAttr attr) noexcept {
		return node_str[static_cast<int>(attr)];
	}

	std::string_view to_string(SubgraphAttr attr) noexcept {
		return subgraph_str[static_cast<int>(attr)];
	}

	std::string_view to_string(EdgeAttr attr) noexcept {
		return edge_str[static_cast<int>(attr)];
	}

	static_assert(std::size(graph_str) == static_cast<std::size_t>(GraphAttr::__count__));
	static_assert(std::size(node_str) == static_cast<std::size_t>(NodeAttr::__count__));
	static_assert(std::size(edge_str) == static_cast<std::size_t>(EdgeAttr::__count__));
	static_assert(std::size(subgraph_str) == static_cast<std::size_t>(SubgraphAttr::__count__));
}
