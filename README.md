# Comp-club-test-task

## Requirements
- `g++-14` or `clang-19`
- `CMake 3.24`

## Building
    $ cmake -S . -B build
    $ cmake --build build

## Running tests
    $ ctest --test-dir build --failure-on-output

## Execute binary
    $ ./build/task <input_file>
