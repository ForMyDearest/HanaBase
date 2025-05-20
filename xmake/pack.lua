set_project("HanaBase")

set_languages("cxx20", "c11")
add_rules("mode.debug", "mode.release", "mode.releasedbg")

if (is_os("windows")) then
    add_defines("UNICODE", "NOMINMAX", "_WINDOWS", "WIN32_LEAN_AND_MEAN")
    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_defines("_ENABLE_EXTENDED_ALIGNED_STORAGE")
    add_defines("_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
    add_defines("HANA_OS_WINDOWS")
elseif (is_os("linux")) then
    add_defines("HANA_OS_LINUX")
elseif (is_os("android")) then
    add_defines("HANA_OS_ANDROID")
elseif (is_os("macosx")) then
    add_defines("HANA_OS_MACOSX")
elseif (is_os("IOS")) then
    add_defines("HANA_OS_IOS")
end

default_unity_batch = 16

includes("compile_flags.lua")
includes("../modules/xmake.lua")