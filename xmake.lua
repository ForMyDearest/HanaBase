set_project("HanaBase")

set_warnings("all")
set_policy("build.ccache", true)
set_policy("build.warning", true)
set_policy("check.auto_ignore_flags", true)
set_policy("package.install_locally", false)

set_languages("cxx20", "c11")
add_rules("mode.debug", "mode.release", "mode.releasedbg")

if (is_os("windows")) then
    add_defines("UNICODE", "NOMINMAX", "_WINDOWS", "WIN32_LEAN_AND_MEAN")
    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_defines("_ENABLE_EXTENDED_ALIGNED_STORAGE")
    add_defines("_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
    add_defines("OS_WINDOWS")
    if (is_mode("release")) then
        set_runtimes("MD")
    else
        set_runtimes("MDd")
    end
elseif (is_os("linux")) then
    add_defines("OS_LINUX")
elseif (is_os("android")) then
    add_defines("OS_ANDROID")
elseif (is_os("macosx")) then
    add_defines("OS_MACOSX")
elseif (is_os("IOS")) then
    add_defines("OS_IOS")
end

default_unity_batch = 16

includes("xmake/compile_flags.lua")
includes("modules/xmake.lua")
includes("tests/xmake.lua")