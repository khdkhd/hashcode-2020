#include <pizza.hpp>

#include <fstream>
#include <iostream>

void usage() {
    std::cerr << "Usage: hashcode-2020-pizza <SOLVER>" << std::endl;
    std::exit(1);
}

int
main(int argc, char const **argv) {
    PizzaProblem problem;

    if (argc < 2) {
        usage();
    }

    const auto solve = createSolver(argv[1]);

    if (argc < 3) {
        std::cin >> problem;
    } else if (auto input = std::ifstream(argv[2])) {
        input >> problem;
    } else {
        usage();
    }

    std::cout << solve(problem) << std::endl;

    return 0;
}
