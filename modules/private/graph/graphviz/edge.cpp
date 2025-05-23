#include "edge_impl.hpp"

#include <cassert>

namespace hana::graphviz
{
    Vertex::Vertex(const NodeImpl* node) noexcept: node_(node) {}

    EdgeImpl::EdgeImpl(const NodeImpl* from, const NodeImpl* to) noexcept: from_(from), to_(to) {}

    EdgeImpl::~EdgeImpl() noexcept {
        delete attributes_;
    }

    Edge* EdgeImpl::add_attribute(EdgeAttr key, HStringView value) noexcept {
        if (!attributes_) {
            attributes_ = new attr_map_t<EdgeAttr>;
        }
        attributes_->emplace(key, value.raw_view());
        return this;
    }

    Edge* EdgeImpl::remove_attribute(EdgeAttr key) noexcept {
        if (attributes_) {
            attributes_->erase(key);
        }
        return this;
    }

    Edge* EdgeImpl::clear_attributes() noexcept {
        if (attributes_) {
            attributes_->clear();
        }
        return this;
    }

    Edge* EdgeImpl::set_ports(const Port* from, const Port* to) noexcept {
        assert(from && to && "port can't be nullptr");
        from_.port_ = from;
        to_.port_ = to;
        return this;
    }

    Edge* EdgeImpl::reset_ports() noexcept {
        from_.port_ = nullptr;
        to_.port_ = nullptr;
        return this;
    }
}
