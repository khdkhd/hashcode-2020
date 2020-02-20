#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <istream>
#include <ostream>
#include <random>
#include <map>
#include <bitset>

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

                        // std::cerr << currentlyComputedSolution.value << ",";
                }
                // std::cerr << std::endl;
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

Solution solveLarger(std::vector<Item> &items, const unsigned int maxWeight, const unsigned short bucketSize = 100, unsigned int maxIterations = 50000)
{
        const auto itemCount = items.size();
        const auto maxItemWeight = items[itemCount - 1].getWeight();
        const auto maxBucket = maxItemWeight / bucketSize;

        std::cerr << "Building index with " << maxBucket << " buckets" << std::endl;
        std::map<unsigned short, std::vector<Item>> index;
        for (const auto &item : items)
        {
                const unsigned short bucket = item.getWeight() / bucketSize;

                for (auto i = bucket; i <= maxBucket; i++)
                {
                        index[i].push_back(item);
                }
        }

        Solution solution = solveGreedy(items, maxWeight);
        Solution bestSolution = solution;

        std::bitset<10000> presentItems;
        for (const auto &item : solution.items)
        {
                presentItems[item.getIndex()] = true;
        }

        unsigned short interationCount{0};
        while (solution.weight < maxWeight && interationCount < maxIterations)
        {
                interationCount++;
                std::cerr << "Iteration [bestValue=" << bestSolution.value << ";value=" << solution.value << ";remaining=" << (maxWeight - solution.weight) << ";count=" << interationCount << "]" << std::endl;

                auto countToRemove = 1 + rand() % 10;
                auto removedWeight = 0;
                // std::cerr << "Removing " << countToRemove << " items" << std::endl;

                while (solution.items.size() > 0 && countToRemove > 0)
                {
                        const auto chosen = rand() % solution.items.size();
                        Item removed = solution.items[chosen];

                        remove_at<Item>(solution.items, chosen);
                        solution.value -= removed.getValue();
                        solution.weight -= removed.getWeight();
                        presentItems[removed.getIndex()] = false;
                        removedWeight += removed.getWeight();

                        countToRemove--;
                }
                // std::cerr << "Removed " << removedWeight << " kg, now " << solution.weight << "kg/" << maxWeight << "kg => " << (maxWeight - solution.weight) << "kg to find" << std::endl;

                while (true)
                {
                        const auto remaining = maxWeight - solution.weight;
                        // std::cerr << (maxWeight - solution.weight) << ";" << remaining << std::endl;
                        const auto bucketId = remaining / bucketSize;
                        const auto bucketToSearchInto = bucketId > maxBucket ? maxBucket : bucketId;

                        const auto &bucket = index[bucketToSearchInto];
                        std::vector<Item> candidates;
                        std::copy_if(bucket.begin(), bucket.end(), std::back_inserter(candidates), [remaining, presentItems](Item i) { return !presentItems[i.getIndex()] && i.getWeight() <= remaining; });
                        std::random_shuffle(candidates.begin(), candidates.end());
                        if (candidates.size() == 0)
                        {
                                // std::cerr << "No candidates in " << bucketToSearchInto << "/" << maxBucket << ";" << remaining << "/" << maxItemWeight << std::endl;
                                break;
                        }

                        // std::cerr << "Item found: " << item << std::endl;
                        const auto &item = candidates[0];
                        solution.items.push_back(item);
                        solution.value += item.getValue();
                        solution.weight += item.getWeight();
                        presentItems[item.getIndex()] = true;
                }

                if (solution.value > bestSolution.value)
                {
                        bestSolution = solution;
                }
        }
        return bestSolution;
}

Solution solveLarger2(std::vector<Item> &items, const unsigned int maxWeight, const unsigned short bucketSize = 100, unsigned int maxIterations = 50000)
{
        Solution solution = solveGreedy(items, maxWeight);
        Solution bestSolution = solution;

        std::bitset<10000> presentItems;
        for (const auto &item : solution.items)
        {
                presentItems[item.getIndex()] = true;
        }

        unsigned short interationCount{0};
        while (solution.weight < maxWeight && interationCount < maxIterations)
        {
                interationCount++;
                std::cerr << "Iteration [bestValue=" << bestSolution.value << ";value=" << solution.value << ";remaining=" << (maxWeight - solution.weight) << ";count=" << interationCount << "]" << std::endl;

                auto countToRemove = 1 + rand() % 10;
                // std::cerr << "Removing " << countToRemove << " items" << std::endl;

                while (solution.items.size() > 0 && countToRemove > 0)
                {
                        const auto chosen = rand() % solution.items.size();
                        Item removed = solution.items[chosen];

                        remove_at<Item>(solution.items, chosen);
                        solution.value -= removed.getValue();
                        solution.weight -= removed.getWeight();
                        presentItems[removed.getIndex()] = false;

                        countToRemove--;
                }

                const auto remaining = maxWeight - solution.weight;
                std::vector<Item> candidates;
                std::copy_if(items.begin(), items.end(), std::back_inserter(candidates), [remaining, presentItems](Item i) {
                        if (rand() % 10 <= 1)
                        {
                                return false;
                        }
                        return !presentItems[i.getIndex()] && i.getWeight() <= remaining;
                });

                const auto intermediateSolution = solve(candidates, remaining);
                for (const auto &item : intermediateSolution.items)
                {
                        presentItems[item.getIndex()] = true;
                        solution.value += item.getValue();
                        solution.weight += item.getWeight();
                        solution.items.push_back(item);
                }

                if (solution.value > bestSolution.value)
                {
                        bestSolution = solution;
                }

                if (bestSolution.weight == maxWeight)
                {
                        break;
                }
        }
        return bestSolution;
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

        const auto solution = solveLarger2(items, maxWeight);

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
