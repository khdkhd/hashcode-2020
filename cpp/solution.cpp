#include "pizza.hpp"

std::ostream &
operator<<(std::ostream &out, const PizzaSolution &solution) {
    out << solution.pizze.size() << std::endl;
    std::copy(
        solution.pizze.begin(),
        solution.pizze.end(),
        std::ostream_iterator<PizzaIndex_t>(out, " ")
    );
    return out;
}

bool
operator>(const PizzaSolution &lhs, const PizzaSolution &rhs) {
    return lhs.sliceCount > rhs.sliceCount;
}
