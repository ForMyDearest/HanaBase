add_requires("doctest")

function UNIT_TEST(name)
    target("unit_test." .. name)
    do
        set_kind("binary")
        set_group("unit_test")
        add_deps("HanaBase")
        add_packages("doctest")
        add_defines("DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN")
        add_files(name .. ".cpp")
    end
end

UNIT_TEST("rc")
UNIT_TEST("misc")
UNIT_TEST("json")
UNIT_TEST("graph")
UNIT_TEST("string")
UNIT_TEST("format")
UNIT_TEST("algorithm")
UNIT_TEST("string_view")
UNIT_TEST("compressed_pair")