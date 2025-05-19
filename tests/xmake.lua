add_requires("doctest 2.4.11")

function TEST(name)
    target(name)
    do
        set_kind("binary")
        set_group("test")
        add_deps("HanaBase")
        add_files(name .. ".cpp")
    end
end

function UNIT_TEST(name)
    target(name)
    do
        set_kind("binary")
        set_group("unit_test")
        add_deps("HanaBase")
        add_packages("doctest")
        add_defines("DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN")
        add_files(name .. ".cpp")
    end
end

UNIT_TEST("misc")
UNIT_TEST("json")
UNIT_TEST("graph")
UNIT_TEST("string")
UNIT_TEST("format")
UNIT_TEST("algorithm")
UNIT_TEST("string_view")
UNIT_TEST("intrusive_ptr")
UNIT_TEST("compressed_pair")

TEST("log")

includes("module/xmake.lua")