#pragma once

#include "hana/memory/rc.hpp"
#include "hana/platform/macros.h"

#include <functional>

namespace hana
{
	struct GraphNode {
		GraphNode() = default;
		virtual ~GraphNode() noexcept = default;
	};

	class GraphEdge {
	public:
		GraphEdge() noexcept = default;
		virtual ~GraphEdge() noexcept = default;

		GraphNode* from() noexcept;
		GraphNode* to() noexcept;

		const GraphNode* from() const noexcept;
		const GraphNode* to() const noexcept;

	private:
		friend struct GraphImpl;
		struct GraphEdgeID* id = nullptr;
	};

	class HANA_BASE_API Graph : RCUniqueInterface {
	public:
		static RCUnique<Graph> create();

		void add_edge(GraphNode* from, GraphNode* to, GraphEdge* edge = nullptr);
		void remove_vertex(GraphNode* node) noexcept;
		void remove_edge(GraphNode* from, GraphNode* to) noexcept;
		void remove_edge(GraphEdge* edge) noexcept;
		void clear() noexcept;

		void foreach_neighbors(GraphNode* node, const std::function<void(GraphNode*)>& func);
		void foreach_inv_neighbors(GraphNode* node, const std::function<void(GraphNode*)>& func);
		void foreach_outgoing_edges(GraphNode* node, const std::function<void(GraphEdge*)>& func);
		void foreach_incoming_edges(GraphNode* node, const std::function<void(GraphEdge*)>& func);
		void foreach_vertex(const std::function<void(GraphNode*)>& func);

	private:
		template<typename> friend class RCUnique;
		void rc_delete() noexcept;

		Graph() = default;
		~Graph() = default;
	};
}
