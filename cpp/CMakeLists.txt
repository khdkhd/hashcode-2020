cmake_minimum_required(VERSION 3.0)

project(hashcode-2020-pizza)

list(APPEND SOLVERS
    "${CMAKE_CURRENT_SOURCE_DIR}/solvers/shuffle.cpp"
)

set(EXPORT_SOLVERS_H "${CMAKE_BINARY_DIR}/solvers.hpp")
set(EXPORT_SOLVERS_H_GENERATOR "${CMAKE_CURRENT_SOURCE_DIR}/solvers.cmake")

add_custom_command(
    OUTPUT "${EXPORT_SOLVERS_H}"
    COMMAND ${CMAKE_COMMAND}
        -D SOLVERS="${SOLVERS}"
        -D EXPORT_SOLVERS_H="${EXPORT_SOLVERS_H}"
        -P "${EXPORT_SOLVERS_H_GENERATOR}"
    DEPENDS
        "${EXPORT_SOLVERS_H_GENERATOR}"
)

add_executable(${PROJECT_NAME}
    ${EXPORT_SOLVERS_H}
    pizza.hpp
    main.cpp
    problem.cpp
    solution.cpp
    solver.cpp
    solvers/shuffle.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}"
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)