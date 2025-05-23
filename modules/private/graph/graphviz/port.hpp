#pragma once

#include "fwd.hpp"

namespace hana::graphviz
{
    struct Port {
        explicit Port(std::string_view id, Compass compass = Compass::None) noexcept;

        std::string_view id_;
        Compass compass_;
    };
}
