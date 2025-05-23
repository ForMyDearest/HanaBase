#include "dumper.hpp"
#include "cluster.hpp"
#include "edge_impl.hpp"
#include "subgraph_impl.hpp"

namespace hana::graphviz
{
	Dumper::Dumper(HStringView filename, bool is_digraph) noexcept
		: is_digraph_(is_digraph), ofs_(filename.raw_data()) {}

	std::ofstream& Dumper::print_indent() noexcept {
		for (std::size_t i = 0; i < indent_; i++) {
			ofs_ << '\t';
		}
		return ofs_;
	}

	template<bool IsSubgraph>
	void Dumper::dump_header(const Cluster& cluster) noexcept {
		if constexpr (!IsSubgraph) {
			if (cluster.subgraphs_.empty() && cluster.nodes_.empty() && cluster.edges_.empty()) {
				return;
			}
		}
		if constexpr (IsSubgraph) {
			print_indent() << "subgraph \"" << cluster.id_ << "\" {\n";
		} else {
			print_indent() << "graph \"" << cluster.id_ << "\" {\n";
		}
		indent_++;
	}

	template<bool IsSubgraph>
	void Dumper::dump_cluster(const Cluster& cluster) noexcept {
		for (const auto* subgraph: cluster.subgraphs_) {
			subgraph->dump(*this);
		}

		for (auto* node: cluster.nodes_) {
			if (!node->attributes_) {
				print_indent() << node->id_ << '\n';
			} else {
				dump_attributes(node->id_, *node->attributes_);
			}
		}

		auto dump_edge = [&](const EdgeImpl* edge) {
			std::string str;

			if (!edge->from_.port_) {
				str.append("\"").append(edge->from_.node_->id_).append("\"");
				str += (is_digraph_ ? " -> " : " -- ");
				str.append("\"").append(edge->to_.node_->id_).append("\"");
			} else {
				constexpr const char* compass2name[] = {
					"n", "ne", "e", "se", "s", "sw", "w", "nw", "c"
				};

				auto dump_vertex = [&](const Vertex& vertex) {
					str.append("\"").append(vertex.node_->id_).append("\"");
					if (!vertex.port_->id_.empty()) {
						str.append(":\"").append(vertex.port_->id_).append("\"");
					}
					if (vertex.port_->compass_ != Compass::None) {
						str.append(":").append(compass2name[static_cast<int>(vertex.port_->compass_)]);
					}
				};

				dump_vertex(edge->from_);
				str += (is_digraph_ ? " -> " : " -- ");
				dump_vertex(edge->to_);
			}

			return str;
		};

		for (auto* edge: cluster.edges_) {
			std::string head = dump_edge(edge);
			if (!edge->attributes_) {
				print_indent() << head << '\n';
			} else {
				dump_attributes(head, *edge->attributes_);
			}
		}

		indent_--;
		if constexpr (IsSubgraph) {
			print_indent() << "}\n";
		} else {
			print_indent() << '}';
		}
		ofs_.flush();
	}

	template void Dumper::dump_header<true>(const Cluster& cluster) noexcept;
	template void Dumper::dump_header<false>(const Cluster& cluster) noexcept;
	template void Dumper::dump_cluster<true>(const Cluster& cluster) noexcept;
	template void Dumper::dump_cluster<false>(const Cluster& cluster) noexcept;
}
