#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <istream>
#include <ostream>
#include <random>
#include <map>

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

class Solution
{
public:
        unsigned int value;
        unsigned int weight;
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

bool compareItemsReversed(const Item i1, const Item i2)
{
        return (i1.getValue() > i2.getValue());
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

Solution solveGreedy(const std::vector<Item> &items, const unsigned int maxWeight)
{
        Solution solution;
        solution.weight = 0;
        solution.value = 0;
        for (const auto &item : items)
        {
                auto currentItemWeight = item.getWeight();
                if (currentItemWeight <= (maxWeight - solution.weight))
                {
                        solution.value += item.getValue();
                        solution.weight += item.getWeight();
                        solution.items.push_back(item);
                }
                else
                {
                        break;
                }
        }
        return solution;
}

template <typename T>
T remove_at(std::vector<T> &v, typename std::vector<T>::size_type n)
{
        T ans = std::move_if_noexcept(v[n]);
        v[n] = std::move_if_noexcept(v.back());
        v.pop_back();
        return ans;
}

Solution solveLarger(std::vector<Item> &items, const unsigned int maxWeight, const unsigned short bucketSize = 20, unsigned short maxIterations = 100)
{
        std::map<unsigned short, std::vector<Item>> index;
        for (const auto &item : items)
        {
                const unsigned short bucket = item.getWeight() / bucketSize;
                index[bucket].push_back(item);
        }

        Solution solution = solveGreedy(items, maxWeight);

        unsigned short interationCount{0};
        while (solution.weight < maxWeight && interationCount < maxIterations)
        {
                const unsigned int remaining = maxWeight - solution.weight;
                std::cerr << "Remaining: " << remaining << std::endl;
                std::cerr << solution << std::endl;

                const auto chosen = rand() % solution.items.size();

                std::cerr << "Removing " << chosen << std::endl;

                Item removed = solution.items[chosen];

                remove_at<Item>(solution.items, chosen);
                solution.value -= removed.getValue();
                solution.weight -= removed.getWeight();

                const auto searchedWeight = remaining + removed.getWeight();
                const auto bucketToSearchInto = searchedWeight / bucketSize;

                const auto &bucket = index[bucketToSearchInto];
                std::vector<Item> candidates;
                std::copy_if(bucket.begin(), bucket.end(), std::back_inserter(candidates), [searchedWeight](Item i) { return i.getWeight() <= searchedWeight; });
                if (candidates.size() > 0)
                {
                        std::sort(candidates.begin(), candidates.end(), compareItemsReversed);
                        const auto itemToAdd = candidates[0];
                        solution.items.push_back(itemToAdd);
                        solution.value += itemToAdd.getValue();
                        solution.weight += itemToAdd.getWeight();
                }
                else
                {
                        std::cerr << "No matching bucket" << std::endl;
                }
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

        const auto solution = solveLarger(items, maxWeight);

        unsigned int i{0};
        std::cerr << "Overall value: " << solution.value << std::endl;

        std::cout << solution.items.size() << std::endl;
        for (const auto item : solution.items)
        {
                i += item.getWeight();
                std::cout << item.getIndex() << " ";
        }

        std::cerr << std::endl
                  << i << std::endl;
}
