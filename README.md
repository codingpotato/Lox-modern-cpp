# Lox Modern Cpp

![build](https://github.com/codingpotato/Lox-modern-cpp/workflows/build/badge.svg)
[![codecov](https://codecov.io/gh/codingpotato/Lox-modern-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/codingpotato/Lox-modern-cpp)

Lox Modern Cpp is a modern cpp (C++17) implement of Lox programming language described in Bob Nystrom's book: [Crafting Interpreters](http://craftinginterpreters.com). The bytecode virtual machine is implemented by the modern cpp features to make it fast, cache friendly, lightweight and modular. 

## Lox language
Lox is a programming language described in Bob Nystrom's book: [Crafting Interpreters](http://craftinginterpreters.com). The syntax is similiar to C, with dynamic types and garbage collector. Folowing is a example to calcute the sum of 10000000.

```
var sum = 0.0;
var i = 0;
while (i < 10000000) {
    sum = sum + i;
    i = i + 1;
}
print sum;
```

For more details on Lox's syntax, check out the [description](http://craftinginterpreters.com/the-lox-language.html)
in Bob's book.

## Release Build
Dependency:
- C++17 standard compaitible compiler (gcc and clang tested)
- cmake (> 3.13)

Build steps:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Usage
Run an interactive REPL interpreter:

```
./lox
```

Execute lox source files:

```
./lox <filename.lox>
```

## Unit tests
Lox Modern Cpp use [doctest](https://github.com/onqtam/doctest) for unit tests. All lox function tests come from [Bob Nystrom's implemenations of Lox](https://github.com/munificent/craftinginterpreters). Use following sciprt to run all unit tests and generate the code coverage result:

```
./coverage.sh
```

## Benchmark
Lox Modern Cpp uses google [benchmark](https://github.com/google/benchmark) for benchmarking. Run lox-benchmark after release build to run the benchmarking:

```
./benchmark/lox-benchmark
```

Test on Mac OS X, compiled with Apple clang 11.0.3:

```
(8 X 3600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
```
#### Result of Lox Modern Cpp (Unit: second)

| **Benchmark** | **Min.** | **Max.** | **Average** |
| ------------- | -------: | -------: | ----------: |
| sum.lox       |     0.33 |     0.35 |        0.34 |
| fib.lox       |     1.02 |     1.03 |        1.02 |
| equality.lox  |     1.48 |     1.49 |        1.48 |

#### Result of Clox, Bob Nystrom's C implementation (Unit: second)

| **Benchmark** | **Min.** | **Max.** | **Average** |
| ------------- | -------: | -------: | ----------: |
| sum.lox       |     0.38 |     0.38 |        0.38 |
| fib.lox       |     0.91 |     0.91 |        0.91 |
| equality.lox  |     1.45 |     1.46 |        1.46 |

## Todo
- [ ] Classes and Instances
- [ ] Methods and Initializers
- [ ] Superclasses

## Contributing
If you find any error or have a suggestion, please do file an issue or send a pull request. Thank you!
