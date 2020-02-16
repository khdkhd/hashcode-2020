#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <istream>
#include <ostream>
#include <random>

class Item
{
        unsigned int id;
        unsigned int weight;
        unsigned int value;

public:
        void set(unsigned short id, unsigned int weight, unsigned int value)
        {
                this->id = id;
                this->weight = weight;
                this->value = value;
        }

        unsigned int getIndex() const
        {
                return id;
        }

        unsigned int getWeight() const
        {
                return weight;
        }

        unsigned int getValue() const
        {
                return value;
        }
};

std::ostream &
operator<<(std::ostream &out, const Item &item)
{
        // out << item.getIndex() << "(" << item.getWeight() << ")";
        out << "+" << item.getWeight();
        return out;
}

struct Solution
{
        unsigned int value;
        std::vector<Item> items;
};

std::ostream &
operator<<(std::ostream &out, const Solution &solution)
{
        out << solution.value << std::endl;
        std::copy(
            solution.items.begin(),
            solution.items.end(),
            std::ostream_iterator<Item>(out, ""));
        return out;
}

Solution solve(const std::vector<Item> &items, const unsigned int maxWeight)
{
        const auto count = items.size();

        std::cerr << "Allocating " << (sizeof(Solution) * 2 * (maxWeight + 1)) / (1024 * 1024) << "Mb..." << std::endl;
        auto **solutions = new Solution *[2];
        for (auto i = 0; i < 2; i++)
        {
                solutions[i] = new Solution[maxWeight + 1]();
        }

        unsigned int currentItemIndex = 0;

        std::cerr << "Computing..." << std::endl;
        for (auto currentItemIndex = 1; currentItemIndex <= count; currentItemIndex++)
        {
                const auto currentItem = items[currentItemIndex - 1];
                const auto currentItemSize = currentItem.getWeight();
                const auto currentItemValue = currentItem.getValue();
                for (auto currentMaxWeights = 0; currentMaxWeights <= maxWeight; currentMaxWeights++)
                {
                        auto &currentlyComputedSolution = solutions[currentItemIndex % 2][currentMaxWeights];
                        if (currentItemSize > currentMaxWeights)
                        {
                                currentlyComputedSolution.value = solutions[(currentItemIndex - 1) % 2][currentMaxWeights].value;
                                currentlyComputedSolution.items = solutions[(currentItemIndex - 1) % 2][currentMaxWeights].items;
                        }
                        else
                        {
                                auto &solutionWith = solutions[(currentItemIndex - 1) % 2][currentMaxWeights - currentItemSize];
                                auto &solutionWithout = solutions[(currentItemIndex - 1) % 2][currentMaxWeights];

                                if (solutionWith.value + currentItemValue > solutionWithout.value)
                                {
                                        currentlyComputedSolution.value = solutionWith.value + currentItemSize;
                                        currentlyComputedSolution.items = solutionWith.items;
                                        currentlyComputedSolution.items.push_back(currentItem);
                                }
                                else
                                {
                                        currentlyComputedSolution.value = solutionWithout.value;
                                        currentlyComputedSolution.items = solutionWithout.items;
                                }
                        }

                        std::cerr << currentlyComputedSolution.value << ",";
                }
                std::cerr << std::endl;
        }

        auto solution = solutions[count % 2][maxWeight];

        for (auto i = 0; i < 2; i++)
        {
                delete[] solutions[i];
        }
        delete[] solutions;

        return solution;
}

bool compareItems(const Item i1, const Item i2)
{
        return (i1.getValue() < i2.getValue());
}

Solution solveLarge(std::vector<Item> &items, const unsigned int maxWeight, const unsigned int batchSize = 8)
{
        const auto itemCount = items.size();
        std::random_shuffle(items.begin(), items.end());

        Solution solution;
        for (auto i = 0; i < itemCount; i += batchSize)
        {
                const auto end = (i + batchSize) > (itemCount) ? itemCount : i + batchSize;

                std::cerr << "From " << i << " to " << end << std::endl;

                // Get random items
                std::cerr << "New items:" << std::endl;
                std::vector<Item> subproblem(&items[i], &items[end]);

                // Add previous solution
                std::cerr << "With previous solution:" << std::endl;
                copy(solution.items.begin(), solution.items.end(), back_inserter(subproblem));

                // std::sort(subproblem.begin(), subproblem.end(), compareItems);

                solution = solve(subproblem, maxWeight);
                std::cerr << "Intermediate solution" << std::endl
                          << solution << std::endl
                          << std::endl;
        }
        return solution;
}

int main()
{
        unsigned int maxWeight;
        unsigned int pizzaTypeCount;

        std::cin >> maxWeight >> pizzaTypeCount;
        std::cerr << maxWeight << std::endl;
        std::cerr << pizzaTypeCount << std::endl;
        std::vector<unsigned int> pizzas;
        for (auto i = 0; i < pizzaTypeCount; i++)
        {
                unsigned int currentSize;
                std::cin >> currentSize;
                pizzas.push_back(currentSize);
        }
        unsigned int index{0};
        std::vector<Item> items;
        std::transform(
            pizzas.begin(),
            pizzas.end(),
            std::back_inserter(items),
            [&index](auto slice_count) {
                    Item item;
                    item.set(index++, slice_count, slice_count);
                    return item;
            });

        const auto solution = solveLarge(items, maxWeight);

        std::cerr << "Overall value: " << solution.value << std::endl;

        std::cout << solution.items.size() << std::endl;
        for (const auto item : solution.items)
        {
                std::cout << item.getIndex() << " ";
        }
}
