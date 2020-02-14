function greedySolver({ maxCapacity, populationCount, weights }) {
    let score = 0;
    for (let i = populationCount - 1; i >= 0 && score !== maxCapacity; --i) {
        const w = weights[i];
        if (score + w <= maxCapacity) {
            score += w
        }
    }
    return score
}

module.exports = greedySolver;