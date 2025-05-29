function SAMPLE(name)
    target("sample." .. name)
    do
        set_kind("binary")
        set_group("sample")
        add_deps("HanaBase")
        add_files(name .. ".cpp")
    end
end

SAMPLE("log")
SAMPLE("crash")
SAMPLE("process")

includes("module/xmake.lua")
includes("graphviz/xmake.lua")