target("static0")
do
    set_kind("static")
    add_deps("HanaBase")
    add_files("static0.cpp")
end

target("dynamic0")
do
    set_kind("shared")
    add_deps("HanaBase")
    add_files("dynamic0.cpp")
end

target("dynamic1")
do
    set_kind("shared")
    add_deps("HanaBase")
    add_files("dynamic1.cpp")
end

target("dynamic2")
do
    set_kind("shared")
    add_deps("HanaBase")
    add_files("dynamic2.cpp")
end

target("dynamic3")
do
    set_kind("shared")
    add_deps("HanaBase")
    add_files("dynamic3.cpp")
end

target("hotfix")
do
    set_kind("shared")
    add_deps("HanaBase")
    add_files("hotfix.cpp")
end

target("main")
do
    set_kind("binary")
    add_deps("static0")
    add_deps("HanaBase")
    add_files("main.cpp")
end