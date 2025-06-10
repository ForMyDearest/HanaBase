# Introduction

[![License](https://img.shields.io/github/license/ForMyDearest/HanaBase?label=license&style=flat-square)](./LICENSE)

Foundation for hana

# Main Components

| Module          |                            Description                             | Reference                                                                                         |
|-----------------|:------------------------------------------------------------------:|---------------------------------------------------------------------------------------------------|
| `fmt`           |                   Modern formatter with char8_t                    | [fmt](https://github.com/fmtlib/fmt) (MIT),  [STL](https://github.com/microsoft/STL) (Apache-2.0) |
| `crash`         |               Default crash handler with stacktrace                | [SakuraEngine](https://github.com/SakuraEngine/SakuraEngine) (MIT)                                |
| `graphviz`      |                       .dot generator for C++                       | [UGraphviz](https://github.com/Ubpa/UGraphviz) (MIT)                                              |
| `HString`       |         A string compatible with all base character types          | [OpenString](https://github.com/1762757171/OpenString) (MIT)                                      |
| `LogSystem`     |                    High-performance log system                     | [fmtlog](https://github.com/MengRao/fmtlog) (MIT)                                                 |
| `ModuleManager` | Load dependency libraries into dependency graphs using JSON format | [SakuraEngine](https://github.com/SakuraEngine/SakuraEngine) (MIT)                                |

# Dependencies

- [cr](https://github.com/fungos/cr) (MIT)
- [xxhash](https://github.com/Cyan4973/xxHash) (BSD 2-Clause License)
- [yyjson](https://github.com/ibireme/yyjson) (MIT)
- [parallel-hashmap](https://github.com/greg7mdp/parallel-hashmap) (Apache-2.0)

# Hana Log Performance

hana::LogSystem is modified from fmtlog::logOnce.
It just pushes static info along with formatted msg body onto the queue, which causes smaller program size but higher front-end latency.
In my test, it has a front-end latency of approximately 75ns, while static fmtlog::log has a front-end latency of approximately 10ns.

# RC

An implementation of intrusive smart pointers, which stuffing an 8-byte counter block into the class header.
Unlike std, only RCWeak will add weak reference, and resources of RC will be destroyed when both strong and weak reference counter reach 0.

# TODO

* [ ] Fix the bug that log pattern can't support "{{}}"
* [ ] Set up callback interfaces of crash handler for users
* [ ] Cross-platform completion