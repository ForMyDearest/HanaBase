function GRAPHVIZ_SAMPLE(name)
    target("graphviz." .. name)
    do
        set_kind("binary")
        set_group("sample/graphviz")
        add_deps("HanaBase")
        add_files(name .. ".cpp")
    end
end

GRAPHVIZ_SAMPLE("basic")
GRAPHVIZ_SAMPLE("attr")
GRAPHVIZ_SAMPLE("subgraph")
GRAPHVIZ_SAMPLE("port")