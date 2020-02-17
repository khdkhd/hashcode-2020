const fs = require('fs');

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

function partitionItems(items, spacing = 16) {
    const output = [];
    for (let i = 0; i < items.length; i += spacing) {
        output[output.length] = items.slice(i, i + spacing);
    }
    return output;
}

function dump({ maxCapacity, populationCount, solution }, filename) {
    const output = `${maxCapacity} ${populationCount} [\n`
        + partitionItems(solution.items)
            .map(partition => partition.join(',\t'))
            .join(',\n')
        + '\n]\n';
    fs.writeFileSync(filename, output);
}

function optimisticSolver({ maxCapacity, populationCount, weights }, filename) {
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
    if (filename) {
        dump({ maxCapacity, populationCount, solution }, filename);
    }
    return solution.score;
}

module.exports = optimisticSolver;

