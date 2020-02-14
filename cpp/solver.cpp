#include <pizza.hpp>
#include <solvers.hpp>

#include <exception>

Solver
createSolver(const std::string &name) {
    if (name == "shuffle") {
        return shuffleSolver;
    }
    throw new std::invalid_argument("Unknown solver: " + name);
}