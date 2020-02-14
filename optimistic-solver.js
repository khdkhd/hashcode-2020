function createNode(level, weight = 0) {
    return {
        level,
        weight
    }
}

function evaluate({ maxCapacity, populationCount, weights }, { level, weight }) {
    let score = weight;
    for (let i = populationCount - (level + 1); i >= 0; --i) {
        const w = weights[i];
        if (score + w <= maxCapacity) {
            score += w
        }
    }
    return score;
}

function optimisticSolver({ maxCapacity, populationCount, weights }) {
    let score = 0;

    for (let i = 1; i < populationCount; ++i) {
        const left = createNode(i, weights[populationCount - i]);
        const right = createNode(i, 0);
        const leftScore = evaluate({ maxCapacity, populationCount, weights }, left);
        const rightScore = evaluate({ maxCapacity, populationCount, weights }, right);
        const localMaximum = Math.max(leftScore, rightScore);
        if (localMaximum > score) {
            score = localMaximum;
        }
    }

    return score;

}

module.exports = optimisticSolver;

