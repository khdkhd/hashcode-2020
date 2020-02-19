function createNode(level, weight) {
    return {
        level,
        weight
    }
}

function initializeSolution(weight) {
    return {
        score: weight,
        items: weight === 0 ? [] : [weight]
    }
}

function evaluate({ maxCapacity, populationCount, weights }, { level, weight }) {
    const solution = initializeSolution(weight);
    for (let i = populationCount - (level + 1); i >= 0; --i) {
        const w = weights[i];
        if (solution.score + w <= maxCapacity) {
            solution.score += w;
            solution.items.push(w);
        }
    }
    return solution;
}

function maxSolution(a, b) {
    return a.score >= b.score ? a : b;
}

function optimisticSolver({ maxCapacity, populationCount, weights }) {
    let solution = initializeSolution(0);

    for (let i = 1; i < populationCount; ++i) {
        const leftNode = createNode(i, weights[populationCount - i]);
        const rightNode = createNode(i, 0);
        const leftSolution = evaluate({ maxCapacity, populationCount, weights }, leftNode);
        const rightSolution = evaluate({ maxCapacity, populationCount, weights }, rightNode);
        const localMaximum = maxSolution(leftSolution, rightSolution);
        if (localMaximum.score > solution.score) {
            solution = localMaximum;
        }
    }
    return solution;
}

module.exports = optimisticSolver;

