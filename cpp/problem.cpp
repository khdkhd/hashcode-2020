#include "pizza.hpp"

std::istream &
operator>>(std::istream &in, PizzaProblem &problem) {
    PizzaIndex_t pizzeCount{0};

    in >> problem.maxSliceCount;
    in >> pizzeCount;

    std::copy_n(
        std::istream_iterator<PizzaSliceCount_t>(in),
        pizzeCount,
        std::back_inserter(problem.pizze)
    );

    return in;
}