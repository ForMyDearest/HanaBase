#pragma once

namespace hana::graphviz
{
    struct NodeImpl final : Node {
        explicit NodeImpl(std::string_view id) noexcept;
        ~NodeImpl() noexcept override;

        Node* add_attribute(NodeAttr key, HStringView value) noexcept override;
        Node* remove_attribute(NodeAttr key) noexcept override;
        Node* clear_attributes() noexcept override;

        std::string_view id_;
        attr_map_t<NodeAttr>* attributes_{nullptr};
    };
}
