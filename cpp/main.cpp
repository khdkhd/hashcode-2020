#include <pizza.hpp>

#include <streambuf>
#include <fstream>
#include <iostream>

void
usage() {
    std::cerr << "Usage: hashcode-2020-pizza <SOLVER> INPUT_FILE..." << std::endl;
    std::exit(1);
}

std::tuple<std::shared_ptr<std::istream>, std::shared_ptr<std::ostream>>
openStreams(const std::string &filepath) {
    if (filepath == "") {
        return std::make_tuple(
            std::make_shared<std::istream>(std::cin.rdbuf()),
            std::make_shared<std::ostream>(std::cout.rdbuf())
        );
    }

    const auto first = filepath.begin() + filepath.find_last_of('/') + 1;
    const auto last = filepath.begin() + filepath.find_last_of('.');
    const std::string basename(first, last);

    return std::make_tuple(
        std::make_shared<std::ifstream>(filepath),
        std::make_shared<std::ofstream>(basename + ".out")
    );
}

int
main(int argc, char const **argv) {
    if (argc < 2) {
        usage();
    }

    const auto solve = createSolver(argv[1]);
    std::vector<std::string> input_filepaths;

    if (argc >= 3) {
        std::copy(argv + 2, argv + argc, std::back_inserter(input_filepaths));
    } else {
        input_filepaths.emplace_back("");
    }

    for (const auto filepath: input_filepaths) {
        auto [input, output] = openStreams(filepath);

        PizzaProblem problem;

        *input >> problem;
        *output << solve(problem) << std::endl;
    }
    
    return 0;
}
