#include <hana/graph/graph.hpp>

#include <parallel_hashmap/phmap.h>

#include <ranges>

namespace hana
{
	struct GraphEdgeID {
		GraphNode* from;
		GraphNode* to;
		GraphEdge* edge;
	};

	GraphNode* GraphEdge::from() noexcept {
		return id->from;
	}

	GraphNode* GraphEdge::to() noexcept {
		return id->to;
	}

	const GraphNode* GraphEdge::from() const noexcept {
		return id->from;
	}

	const GraphNode* GraphEdge::to() const noexcept {
		return id->to;
	}
}

namespace hana
{
	struct GraphImpl {
		using VertexMap = phmap::flat_hash_map<GraphNode*, phmap::flat_hash_map<GraphNode*, GraphEdgeID*>>;

		~GraphImpl() noexcept {
			clear();
		}

		void clear() noexcept {
			for (auto&& target: outgoings | std::views::values) {
				for (auto&& edge: target | std::views::values)
					delete edge;
			}
			incomings.clear();
			outgoings.clear();
		}

		void add_edge(GraphNode* from, GraphNode* to, GraphEdge* edge = nullptr) {
			auto edge_id = new GraphEdgeID;
			edge_id->from = from;
			edge_id->to = to;
			edge_id->edge = edge;
			if (edge) {
				edge->id = edge_id;
			}
			outgoings[from].emplace(to, edge_id);
			incomings[to].emplace(from, edge_id);
		}

		void remove_edge(GraphNode* from, GraphNode* to) {
			const auto out_iter = outgoings.find(from);
			if (out_iter == outgoings.end()) return;

			if (auto&& edge_iter = out_iter->second.find(to); edge_iter != out_iter->second.end()) {
				delete edge_iter->second;
				out_iter->second.erase(edge_iter);
			}

			incomings[to].erase(from);
		}

		void remove_edge(GraphEdge* edge) noexcept {
			remove_edge(edge->id->from, edge->id->to);
		}

		void remove_vertex(GraphNode* node) noexcept {
			auto&& out_iter = outgoings.find(node);
			if (out_iter == outgoings.end()) return;

			for (auto [to, edge]: out_iter->second) {
				delete edge;
				incomings[to].erase(node);
			}

			outgoings.erase(out_iter);

			auto&& in_iter = incomings.find(node);
			if (in_iter == incomings.end()) return;
			for (auto from: in_iter->second | std::views::keys) {
				outgoings[from].erase(node);
			}

			incomings.erase(in_iter);
		}

		void foreach_neighbors(GraphNode* node, const std::function<void(GraphNode*)>& func) {
			auto&& iter = outgoings.find(node);
			if (iter == outgoings.end()) return;

			for (const auto target: iter->second | std::views::keys) {
				func(target);
			}
		}

		void foreach_inv_neighbors(GraphNode* node, const std::function<void(GraphNode*)>& func) {
			auto&& iter = incomings.find(node);
			if (iter == incomings.end()) return;

			for (const auto target: iter->second | std::views::keys) {
				func(target);
			}
		}

		void foreach_outgoing_edges(GraphNode* node, const std::function<void(GraphEdge*)>& func) {
			auto&& iter = outgoings.find(node);
			if (iter == outgoings.end()) return;

			for (const auto target: iter->second | std::views::values) {
				func(target->edge);
			}
		}

		void foreach_incoming_edges(GraphNode* node, const std::function<void(GraphEdge*)>& func) {
			auto&& iter = incomings.find(node);
			if (iter == incomings.end()) return;

			for (const auto target: iter->second | std::views::values) {
				func(target->edge);
			}
		}

		void foreach_vertex(const std::function<void(GraphNode*)>& func) {
			for (auto* node: incomings | std::views::keys) {
				func(node);
			}
		}

		VertexMap incomings;
		VertexMap outgoings;
	};
}

namespace hana
{
	RCUnique<Graph> Graph::create() {
		return RCUnique<Graph>{reinterpret_cast<Graph*>(new GraphImpl)};
	}

	void Graph::rc_delete() noexcept {
		delete reinterpret_cast<GraphImpl*>(this);
	}

	void Graph::add_edge(GraphNode* from, GraphNode* to, GraphEdge* edge) {
		reinterpret_cast<GraphImpl*>(this)->add_edge(from, to, edge);
	}

	void Graph::remove_vertex(GraphNode* node) noexcept {
		reinterpret_cast<GraphImpl*>(this)->remove_vertex(node);
	}

	void Graph::remove_edge(GraphEdge* edge) noexcept {
		reinterpret_cast<GraphImpl*>(this)->remove_edge(edge);
	}

	void Graph::remove_edge(GraphNode* from, GraphNode* to) noexcept {
		reinterpret_cast<GraphImpl*>(this)->remove_edge(from, to);
	}

	void Graph::clear() noexcept {
		reinterpret_cast<GraphImpl*>(this)->clear();
	}

	void Graph::foreach_neighbors(GraphNode* node, const std::function<void(GraphNode*)>& func) {
		reinterpret_cast<GraphImpl*>(this)->foreach_neighbors(node, func);
	}

	void Graph::foreach_inv_neighbors(GraphNode* node, const std::function<void(GraphNode*)>& func) {
		reinterpret_cast<GraphImpl*>(this)->foreach_inv_neighbors(node, func);
	}

	void Graph::foreach_outgoing_edges(GraphNode* node, const std::function<void(GraphEdge*)>& func) {
		reinterpret_cast<GraphImpl*>(this)->foreach_outgoing_edges(node, func);
	}

	void Graph::foreach_incoming_edges(GraphNode* node, const std::function<void(GraphEdge*)>& func) {
		reinterpret_cast<GraphImpl*>(this)->foreach_incoming_edges(node, func);
	}

	void Graph::foreach_vertex(const std::function<void(GraphNode*)>& func) {
		reinterpret_cast<GraphImpl*>(this)->foreach_vertex(func);
	}
}
