const fs = require("fs");
const { leanDynamicSolver } = require("./max");

const charset = "utf-8";
const eol = "\n";
const tab = " ";

const inputs = [
  "inputs/a_example.in",
  "inputs/b_small.in",
  "inputs/c_medium.in",
  "inputs/d_quite_big.in",
  "inputs/e_also_big.in"
].filter((value, index) => index < 5);

function chunkToLines(input) {
  const lines = input.split(eol);
  lines.pop();
  return lines;
}

function chunkToNumbers(line) {
  return line.split(tab).map(Number);
}

function parse(file) {
  const input = fs.readFileSync(file, charset);
  const lines = chunkToLines(input);
  const [maxCapacity, populationCount] = chunkToNumbers(lines.shift());
  const weights = chunkToNumbers(lines.shift());
  return {
    maxCapacity,
    populationCount,
    weights
  };
}

function solve({ maxCapacity = 0, populationCount = 0, weights = [] }) {
  return leanDynamicSolver(maxCapacity, weights);
}

function run() {
  for (const input of inputs) {
    const solution = solve(parse(input));

    console.log(solution.length);
    console.log(solution.join(" "));
  }
}

run();
