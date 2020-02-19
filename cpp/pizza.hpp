#pragma once

#include <cstdint>
#include <functional>
#include <istream>
#include <map>
#include <ostream>
#include <variant>
#include <vector>

using PizzaIndex_t = unsigned int;
using PizzaSliceCount_t = unsigned int;

struct PizzaProblem {
    PizzaSliceCount_t maxSliceCount{0};
    PizzaIndex_t pizzeCount{0};
    std::vector<PizzaSliceCount_t> pizze;
};

std::istream & operator>>(std::istream &, PizzaProblem &);

struct PizzaSolution {
    PizzaSliceCount_t sliceCount{0};
    std::vector<PizzaIndex_t> pizze;
};

bool operator>(const PizzaSolution &, const PizzaSolution &);
std::ostream &operator<<(std::ostream &, const PizzaSolution &);

using SolverOptionValue = std::variant<std::intmax_t, double, float, std::string>;
using SolverOptionKey = std::string;
using SolverOptions = std::map<SolverOptionKey, SolverOptionValue>;
using Solver = std::function<PizzaSolution(const PizzaProblem &, const SolverOptions &)>;

Solver createSolver(const std::string &);