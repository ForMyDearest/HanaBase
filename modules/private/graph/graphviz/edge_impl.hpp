#pragma once

#include "fwd.hpp"

namespace hana::graphviz
{
    struct Vertex {
        explicit Vertex(const NodeImpl* node) noexcept;

        const NodeImpl* node_;
        const Port* port_{nullptr};
    };

    struct EdgeImpl final : Edge {
        EdgeImpl(const NodeImpl* from, const NodeImpl* to) noexcept;
        ~EdgeImpl() noexcept override;

        Edge* add_attribute(EdgeAttr key, HStringView value) noexcept override;
        Edge* remove_attribute(EdgeAttr key) noexcept override;
        Edge* clear_attributes() noexcept override;

        Edge* set_ports(const Port* from, const Port* to) noexcept override;
        Edge* reset_ports() noexcept override;

        Vertex from_;
        Vertex to_;
        attr_map_t<EdgeAttr>* attributes_{nullptr};
    };
}
