[中文版在这里](./README_zh-CN.md)

LittleQuant is an algorithmic trading application/framework
===========================================================

Status
======

- This project is a very naive and primitive implementation, it is still in progress
- By now the following exchanges are supported
    - Shanghai Stock Exchange
        - Stock option trading, two implementations(CTP, Kingstar)
        - Stock trading, by hacking a stockjobber's web trading client

Known issues
------------

- Some implementations of exchange depend on the 3rd party C++/CLI wrapper of native trading API, these 3rd party wrappers are all outdated due to the update of native trading API, I'v been updated a part of each wrappers, some functionalities of these wrappers may not work correctly.

Build
=====

- Visual Studio 2015
- Restore nuget packages
- Build the projects you need, the build artifacts are all in the `target/` folder
