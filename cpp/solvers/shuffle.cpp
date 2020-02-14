#include <pizza.hpp>

#include <algorithm>
#include <random>
#include <tuple>

Solver shuffleSolver([](const PizzaProblem &problem) {
    using Pizza = std::tuple<PizzaIndex_t, PizzaSliceCount_t>;

    PizzaIndex_t index{0};
    std::vector<Pizza> pizze;
    std::transform(
        problem.pizze.begin(),
        problem.pizze.end(),
        std::back_inserter(pizze),
        [&index](auto slice_count) {
            return Pizza({index++, slice_count});
        }
    );

    unsigned int retry = 1000;
    const auto shouldContinue = [&retry]() {return --retry > 0;};
    std::random_device rd;
    std::mt19937 g(rd());
    PizzaSolution bestSolution;
    do {
        std::shuffle(pizze.begin(), pizze.end(), std::mt19937(rd()));

        PizzaSolution solution;
        for (auto it = pizze.cbegin(); it != pizze.cend(); ++it) {
            const auto [pizza_index, pizza_slice_count] = *it;
            if (solution.sliceCount + pizza_slice_count <= problem.maxSliceCount) {
                solution.sliceCount += pizza_slice_count;
                solution.pizze.push_back(pizza_index);
            }
        }

        if (solution > bestSolution) {
            bestSolution = solution;
        }
    } while (bestSolution.sliceCount != problem.maxSliceCount && shouldContinue());

    std::sort(bestSolution.pizze.begin(), bestSolution.pizze.end());

    return bestSolution;
});