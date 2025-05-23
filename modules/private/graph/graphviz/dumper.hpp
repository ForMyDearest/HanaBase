#pragma once

#include "fwd.hpp"
#include "attributes_str.hpp"

#include <fstream>

namespace hana::graphviz
{
	struct Dumper {
		explicit Dumper(HStringView filename, bool is_digraph) noexcept;

		std::ofstream& print_indent() noexcept;

		template< typename U>
		void dump_attributes(std::string_view head, const attr_map_t<U>& attrs) {
			if (attrs.empty()) return;

			if (attrs.size() > 1) {
				print_indent() << head << " [\n";
				indent_++;
				for (const auto& [key, value]: attrs) {
					print_indent() << graphviz::to_string(key) << " = \"" << value << "\"\n";
				}
				indent_--;
				print_indent() << "]\n";
			} else {
				auto&& [key, value] = *attrs.begin();
				print_indent() << head << " [ " << graphviz::to_string(key) << " = \"" << value << "\" ]\n";
			}
		}

		template<bool IsSubgraph>
		void dump_header(const Cluster& cluster) noexcept;

		template<bool IsSubgraph>
		void dump_cluster(const Cluster& cluster) noexcept;

		bool is_digraph_;
		std::ofstream ofs_;
		std::size_t indent_{0};
	};
}
