# Hash Code 2020

see https://hashcodejudge.withgoogle.com/#/home

## Problem statement

Problem statement lives [here](statement)

## Inputs

Input files given with the problem statement live [here](inputs)

## JavaScript

```shell
$ cd js
$ node index.js
```

## C++

### Build

**CMake** version 3.0 minimum is required to build.
```shell
$ cd cpp
$ mkdir Build
$ cd Build
$ cmake ..
$ make
```

### Run

#### Read problem from standard input 
```shell
$ cd cpp/Build
$ cat ../../inputs/a_example.in | ./hascode-2020-pizza shuffle
```

#### Read from input files
```shell
$ cd cpp/Build
$ ./hascode-2020-pizza \
    shuffle \
    ../../inputs/a_example.in \
    ../../inputs/b_small.in
```

### Implement a solver

* Put your solver implementation in the `cpp/solvers` folders.
* Your solver must be convertible to a `Solver` a.k.a `std::function<PizzaSolution(const PizzaProblem &)>` (see `pizza.hpp`),
* Edit `CMakeLists.txt` to add your solver implementation to the `SOLVERS` list.