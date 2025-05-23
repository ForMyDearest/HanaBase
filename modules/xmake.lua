add_requires("cr")
add_requires("parallel-hashmap")
add_requires("xxhash", { configs = { shared = true } })
add_requires("yyjson", { configs = { shared = true } })

target("HanaBase")
do
    set_kind("$(kind)")
    set_group("01.libraries")
    add_rules("c++.unity_build", { batchsize = default_unity_batch })

    add_deps("compile-flags", { public = true })
    add_packages("cr")
    add_packages("xxhash")
    add_packages("yyjson")
    add_packages("parallel-hashmap")

    add_files("private/*.cpp")
    add_includedirs("private")
    add_includedirs("public", { public = true })
    add_headerfiles("public/(**)")

    after_load(function(target, opt)
        import("core.project.project")
        target:add("defines", "HANA_LOG_ENABLE")
        if (target:get("kind") == "shared") then
            target:add("defines", "HANA_BASE_API=HANA_IMPORTS", { public = true })
            -- Bad for cmake build
            target:add("defines", "HANA_BASE_API=HANA_EXPORTS")
            target:add("defines", "HANA_BASE_DLL=")
        else
            target:add("defines", "HANA_BASE_API=", { public = true })
        end
    end)

    target_end()
end
