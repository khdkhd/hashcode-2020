const fs = require('fs');

const charset = 'utf-8';
const eol = '\n';
const tab = ' ';

const inputs = [
    'inputs/a_example.in',
    'inputs/b_small.in',
    'inputs/c_medium.in',
    'inputs/d_quite_big.in',
    'inputs/e_also_big.in'
];

function chunkToNumbers(line) {
    return line.split(tab).map(Number)
}

function parse(file) {
    const input = fs.readFileSync(file, charset);
    const lines = input.split(eol);
    const [maxCapacity, populationCount] = chunkToNumbers(lines.shift());
    const weights = chunkToNumbers(lines.shift());
    return {
        maxCapacity, populationCount, weights, file
    }
}

function partition(items, spacing = 16) {
    const output = [];
    for (let i = 0; i < items.length; i += spacing) {
        output[output.length] = items.slice(i, i + spacing);
    }
    return output;
}

function randomInt(max) {
    return Math.floor(Math.random() * Math.floor(max))
}

// Fisher–Yates shuffle algorithm
function shuffle(list = []) {
    for (let i = list.length - 1; i > 0; i--) {
        const j = randomInt(i + 1);
        [list[i], list[j]] = [list[j], list[i]]
    }
    return list
}

function dump(items, filePath) {
    fs.writeFileSync(filePath, `${ items.length }\n${ items.reverse().join(' ') }\n`);
}

function createNode(level, weight) {
    return {
        level,
        weight
    }
}

function initializeSolution(weight, index) {
    return {
        score: weight,
        items: weight === 0 ? [] : [index]
    }
}

function evaluate({ maxCapacity, populationCount, weights }, { level, weight }) {
    const solution = initializeSolution(weight, populationCount - level);
    for (let i = populationCount - (level + 1); i >= 0; --i) {
        const w = weights[i];
        if (solution.score + w <= maxCapacity) {
            solution.score += w;
            solution.items.push(i);
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

function solve({ maxCapacity = 0, populationCount = 0, weights = [] }) {
    return optimisticSolver({ maxCapacity, populationCount, weights })
}

function computeGap({ maxCapacity, solution }) {
    return maxCapacity - solution.score;
}

function shuffleInstance(instance, partitionSize) {
    instance.weights = partition([...instance.weights], partitionSize).flatMap(shuffle)
}

function refine(instances = []) {
    if (instances.length === 0) {
        return [];
    }

    for (const instance of instances) {
        const score = instance.solution.score;

        if (computeGap(instance) === 0) {
            continue;
        }

        for (let i = 2; i < Math.floor(instance.weights.length / 2); i += 2) {
            shuffleInstance(instance, i);
            const solution = solve(instance);
            if (solution.score > score) {
                instance.solution = solution;
            }
        }
    }
}

function run() {
    const instances = inputs.map(parse);
    for (const instance of instances) {
        instance.solution = solve(instance);
    }

    const startTime = new Date().getTime();
    const executionTimeInMillis = 1000 * 60 * 30;

    while(new Date().getTime() - startTime < executionTimeInMillis) {
        refine(instances);
    }

    for (const instance of instances) {
        console.log(`score=${instance.solution.score}, gap=${computeGap(instance)}`);
        dump(instance.solution.items, `${instance.file}.out`)
    }

}

run();