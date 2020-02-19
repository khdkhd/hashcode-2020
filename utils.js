const fs = require('fs').promises;

function partition(items, spacing = 16) {
    const output = [];
    for (let i = 0; i < items.length; i += spacing) {
        output[output.length] = items.slice(i, i + spacing);
    }
    return output;
}

function randomInt(max) {
    return Math.floor(Math.random()*Math.floor(max))
}

// Fisher–Yates shuffle algorithm
function shuffle(list = []) {
    for (let i = list.length - 1; i > 0; i--) {
        const j = randomInt(i + 1); [list[i], list[j]] = [list[j], list[i]]
    }
    return list
}

function stringify({ maxCapacity, populationCount, solution }) {
    return `${maxCapacity} ${populationCount} [\n`
        + partition(solution.items).map(partition => partition.join(',\t')).join(',\n')
        + '\n]\n';
}

function dump({ maxCapacity, populationCount, solution }, filePath) {
    fs.writeFileSync(filePath, stringify({ maxCapacity, populationCount, solution }));
}

module.exports = {
    partition,
    shuffle,
    dump
};