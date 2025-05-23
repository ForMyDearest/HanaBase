#include "port.hpp"

namespace hana::graphviz
{
    Port::Port(std::string_view id, Compass compass) noexcept: id_(id), compass_(compass) {}
}
