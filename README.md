# fishjoy

High-performance C++ server framework

```
 ______   __     ______     __  __       __     ______     __  __    
/\  ___\ /\ \   /\  ___\   /\ \_\ \     /\ \   /\  __ \   /\ \_\ \   
\ \  __\ \ \ \  \ \___  \  \ \  __ \   _\_\ \  \ \ \/\ \  \ \____ \  
\ \_\    \ \_\  \/\_____\  \ \_\ \_\ /\_____\  \ \_____\  \/\_____\
\/_/     \/_/   \/_____/   \/_/\/_/ \/_____/   \/_____/   \/_____/

```


## Project Structure

Based on a template from [filipdutescu / modern-cpp-template](https://github.com/filipdutescu/modern-cpp-template)

* `.github`: workflows & template based on GitHub Actions
* `cmake`: cmake files
* `include`: The header files required for the project
* `introduction`: Introduction and design files for the project
* `src`: The source files required for the project
* `test`:The test files required for the project. (Google Test)
## Getting Started

### Prerequisites

* CMake v3.15+ - found at https://cmake.org/

* C++ Compiler - needs to support at least the C++20 standard, i.e. MSVC, GCC, Clang

## Contributing

* 我需要 1 位熟悉 Github Action 和 C++ 环境配置的伙伴来搭建 Github workflow 工作流，实现自动化
* 我需要 1 位熟悉 googletest 和 测试理论的伙伴来丰富我们的测试用例，并进行相关性能分析测试

如果您感兴趣可以通过邮箱：851892190@qq.com 联系我

## Authors
* 周思乐 
## Thanks
* [sylar-yin](https://github.com/sylar-yin) 项目主要模仿了 sylar 的服务器框架设计
* [filipdutescu](https://github.com/filipdutescu)
* [陈硕](https://github.com/chenshuo)
* [zhongluqiang](https://github.com/zhongluqiang)
* 学校教测试的老师（寻找中...）

## ToDo
6月目标
完成线程模块、协程模块、协程调度模块、IO模块

备忘
* 优化一下日志模块(看一下 muduo 相关的)
* 找 NEFU 测试相关的老师补一下测试理论，如何表述该项目测试
* 提升已完成模块的测试覆盖率 >= 90
* 消除 Clang-Tidy 和 Warning