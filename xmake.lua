set_languages("c++20")

-- set_warnings("everything", "error")
add_cxxflags("gcc::-Wduplicated-cond")
add_cxxflags("clang::-Wno-c++98-compat")
-- add_cxxflags("cl::/wd4668 /wd4710 /wd4711 /wd4868")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "src"})

add_rules("mode.debug", "mode.release")
if is_mode("debug") then
    add_defines("DEBUG")
    set_optimize("none")
elseif is_mode("release") then
    add_defines("RELEASE")
    set_optimize("fastest")
end

set_defaultmode("debug")

target("dynamic")
    set_kind("shared")
    if is_os("windows") then
        add_defines("WINDOWS")
    end
    add_includedirs("src/dynamic-lib")
    add_files("src/dynamic-lib/**.cpp")

target("static")
    set_kind("static")
    add_files("src/static-lib/**.cpp")

target("app")
    set_kind("binary")
    add_deps("static")
    add_includedirs("src")
    add_files("src/main.cpp")
    if is_os("windows") then
        add_files("src/platforms/platform-windows.cpp")
    elseif is_os("linux") then
        add_files("src/platforms/platform-linux.cpp")
    end
