const fs = require('fs')
const path = require('path')

const charset = 'utf-8'
const eol = '\n'
const tab = ' '

const inputs = [
    'a_example',
    'b_small',
    'c_medium',
    'd_quite_big',
    'e_also_big'
]

function chunkToLines(input) {
    const lines = input.split(eol)
    lines.pop()
    return lines
}

function chunkToNumbers(line) {
    return line.split(tab).map(Number)
}

function parse(file) {
    const input = fs.readFileSync(path.join('inputs', file + '.in'), charset)
    const lines = chunkToLines(input)
    const [maxCapacity, pizzaCount] = chunkToNumbers(lines.shift())
    const weights = chunkToNumbers(lines.shift())
    return {
        maxCapacity, pizzaCount, weights
    }
}

function randomInt(max) {
    return Math.floor(Math.random()*Math.floor(max))
}

// Fisher–Yates shuffle algorithm
function shuffle(list) {
    for (let i = list.length - 1; i > 0; i--) {
        const j = randomInt(i + 1); [list[i], list[j]] = [list[j], list[i]]
    }
    return list
}

function sliceList(weights) {
    return shuffle(weights.map((sliceCount, index) => [index, sliceCount]))
}

function solve({ maxCapacity = 0, pizzaCount = 0, weights = [] }) {
    let sliceTotalCount = 0
    const pizzas = []

    for (const [index, sliceCount] of sliceList(weights)) {
        if (sliceTotalCount + sliceCount > maxCapacity) {
            break;
        }
        pizzas.push(index)
        sliceTotalCount += sliceCount
    }

    return [sliceTotalCount, pizzas.sort((a, b) => a - b)]
}

let shouldStop = false

function task(data, prevSliceTotalCount = 0) {
    const [sliceTotalCount, pizzas] = solve(data)
    if (sliceTotalCount > prevSliceTotalCount) {
        fs.writeFileSync(
            path.join('outputs', data.instance + '.out'), 
            `${sliceTotalCount}\n${pizzas.length}\n${pizzas.join(' ')}`
        )
    }
    if (sliceTotalCount < data.maxCapacity && !shouldStop) {
        setImmediate(task, data, Math.max(prevSliceTotalCount, sliceTotalCount))
    }
}

function run() {
    process.on('SIGINT', () => {
        console.warn("Caught interrupt signal");
        shouldStop = true
    })

    for (const input of inputs) {
        const data = {
            instance: input,
            ...parse(input)
        }
        task(data, 0)
    }
}

run()