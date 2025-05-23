#include "node_impl.hpp"

namespace hana::graphviz
{
    NodeImpl::NodeImpl(std::string_view id) noexcept: id_(id) {}

    NodeImpl::~NodeImpl() noexcept {
        delete attributes_;
    }

    Node* NodeImpl::add_attribute(NodeAttr key, HStringView value) noexcept {
        if (!attributes_) {
            attributes_ = new attr_map_t<NodeAttr>;
        }
        attributes_->emplace(key, value.raw_view());
        return this;
    }

    Node* NodeImpl::remove_attribute(NodeAttr key) noexcept {
        if (attributes_) {
            attributes_->erase(key);
        }
        return this;
    }

    Node* NodeImpl::clear_attributes() noexcept {
        if (attributes_) {
            attributes_->clear();
        }
        return this;
    }
}
