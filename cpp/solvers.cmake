separate_arguments(SOLVERS)

file(WRITE "${EXPORT_SOLVERS_H}" [=[
#pragma once

#include <pizza.hpp>

]=])

foreach(SOLVER ${SOLVERS})
    get_filename_component(SOLVER_NAME "${SOLVER}" NAME_WE)
    file(APPEND "${EXPORT_SOLVERS_H}" "extern Solver ${SOLVER_NAME}Solver;")
endforeach()
