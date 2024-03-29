# fishjoy

[![C++](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Code Style](https://img.shields.io/badge/code%20style-google-blue.svg)](https://google.github.io/styleguide/cppguide.html)
[![Platform](https://img.shields.io/badge/platform-linux%20-lightgrey.svg)](https://www.kernel.org/)

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

### Prerequisites

* CMake v3.15+ - found at https://cmake.org/

* C++ Compiler - needs to support at least the C++20 standard, i.e. MSVC, GCC, Clang

##  Getting Started

### Building

### Debugging

### Testing

### Example

## More Complex Examples


## Contributing

* 我需要 1 位熟悉 Github Action 和 C++ 环境配置的伙伴来搭建 Github workflow 工作流，实现自动化
* 我需要 1 位熟悉 googletest 和 测试理论的伙伴来丰富我们的测试用例，并进行相关性能分析测试

如果您感兴趣可以通过邮箱：851892190@qq.com 联系我

## Performance


## Credits 
* [sylar-yin](https://github.com/sylar-yin) 项目主要模仿了 sylar 的服务器框架设计
* [filipdutescu](https://github.com/filipdutescu)
* [陈硕](https://github.com/chenshuo)
* [zhongluqiang](https://github.com/zhongluqiang)
* 学校教测试的老师（寻找中...）

## To Do
8 - 9 月目标
* 实现双 Buffer 异步日志，提供mmap写文件功能
* 提供专门的 threadpoll 包括 fixed 模式和 cached 模式
* 完成日志模块梳理
* 完善测试和注释