const greedySolver = (maxSlices, pizzaSizes) => {
  const alreadyOrderedPizzas = new Array(pizzaSizes.length);
  const orderedPizzasIndices = [];

  const showState = remaining => {
    const order = orderedPizzasIndices.map(
      order => `${order}(${pizzaSizes[order]})`
    );
    console.warn(`Remaining(${remaining}) with ${order}`);
  };

  const showEnd = () => {
    const orderedSlices = orderedPizzasIndices
      .map(index => pizzaSizes[index])
      .reduce((size1, size2) => size1 + size2, 0);
    console.warn(`Filled with ${orderedSlices} slices`);
  };

  const pickBestPizza = maxSlices => {
    for (let i = pizzaSizes.length - 1; i >= 0; i--) {
      if (pizzaSizes[i] <= maxSlices && !alreadyOrderedPizzas[i]) {
        return i;
      }
    }
    return -1;
  };

  const order = maxSlices => {
    showState(maxSlices);
    const bestPizza = pickBestPizza(maxSlices);
    if (bestPizza === -1) {
      showEnd(maxSlices);
      return;
    }

    const bestPizzaSize = pizzaSizes[bestPizza];
    if (bestPizzaSize > maxSlices) {
      showEnd(maxSlices);
      return;
    }

    alreadyOrderedPizzas[bestPizza] = true;
    orderedPizzasIndices.push(bestPizza);
    order(maxSlices - bestPizzaSize);
  };

  const solve = () => {
    order(maxSlices);
    return orderedPizzasIndices;
  };

  return solve();
};

const recuriveSolver = (maxSlices, pizzaSizes) => {
  const pickPizzaOrNot = (maxSlices, pizzaIndex = 0) => {
    const indent = "    ".repeat(pizzaIndex);
    if (pizzaIndex >= pizzaSizes.length) {
      return {
        sliceCount: 0,
        pizzas: []
      };
    }

    console.warn(indent, `Solving ${maxSlices} slices from ${pizzaIndex}`);

    const currentPizzaSize = pizzaSizes[pizzaIndex];
    if (currentPizzaSize > maxSlices) {
      return {
        sliceCount: 0,
        pizzas: []
      };
    }

    const solutionWith = pickPizzaOrNot(
      maxSlices - currentPizzaSize,
      pizzaIndex + 1
    );
    const solutionWithout = pickPizzaOrNot(maxSlices, pizzaIndex + 1);

    console.warn(
      indent,
      `Solution with: ${pizzaIndex}: ${solutionWith.sliceCount}`
    );
    console.warn(
      indent,
      `Solution without: ${pizzaIndex}: ${solutionWithout.sliceCount}`
    );

    let solution;
    if (
      solutionWith.sliceCount + currentPizzaSize >
      solutionWithout.sliceCount
    ) {
      console.warn(indent, `--> ${pizzaIndex} should be taken`);
      solution = {
        sliceCount: solutionWith.sliceCount + currentPizzaSize,
        pizzas: [pizzaIndex, ...solutionWith.pizzas]
      };
    } else {
      console.warn(indent, `--> ${pizzaIndex} should NOT be taken`);
      solution = {
        sliceCount: solutionWithout.sliceCount,
        pizzas: [...solutionWithout.pizzas]
      };
    }
    const json = JSON.stringify(solution);
    console.warn(
      indent,
      `Solution for ${maxSlices} starting at ${pizzaIndex}: ${json}`
    );
    return solution;
  };

  const solve = () => {
    console.warn(pizzaSizes);
    const solution = pickPizzaOrNot(maxSlices);
    return solution.pizzas;
  };

  return solve();
};

const dynamicSolver = (maxSlices, pizzaSizes) => {
  const pizzaTypeCount = pizzaSizes.length;
  const noSolution = {
    sliceCount: 0,
    pizzas: []
  };

  const solutions = new Array(maxSlices + 1);
  for (let i = 0; i <= maxSlices; i++) {
    solutions[i] = new Array(pizzaTypeCount + 1);
    solutions[i][0] = noSolution;
  }

  for (
    let currentPizzaIndex = 1;
    currentPizzaIndex <= pizzaTypeCount;
    currentPizzaIndex++
  ) {
    for (
      let currentMaxSlices = 0;
      currentMaxSlices <= maxSlices;
      currentMaxSlices++
    ) {
      const currentPizzaSize = pizzaSizes[currentPizzaIndex - 1];
      if (currentPizzaSize > currentMaxSlices) {
        solutions[currentMaxSlices][currentPizzaIndex] =
          solutions[currentMaxSlices][currentPizzaIndex - 1];
      } else {
        const solutionWith =
          solutions[currentMaxSlices - currentPizzaSize][currentPizzaIndex - 1];
        const solutionWithout =
          solutions[currentMaxSlices][currentPizzaIndex - 1];

        if (
          solutionWith.sliceCount + currentPizzaSize >
          solutionWithout.sliceCount
        ) {
          solution = {
            sliceCount: solutionWith.sliceCount + currentPizzaSize,
            pizzas: [currentPizzaIndex, ...solutionWith.pizzas]
          };
        } else {
          solution = {
            sliceCount: solutionWithout.sliceCount,
            pizzas: [...solutionWithout.pizzas]
          };
        }
        solutions[currentMaxSlices][currentPizzaIndex] = solution;
      }
    }
  }

  console.warn("Solution table:");
  for (let i = 0; i < solutions.length; i++) {
    console.warn(solutions[i].map(solution => solution.sliceCount).join(" "));
  }

  return solutions[maxSlices][pizzaTypeCount].pizzas;
};

const leanDynamicSolver = (maxSlices, pizzaSizes) => {
  const pizzaTypeCount = pizzaSizes.length;

  const index = i => {
    const id = i;
    return id;
  };

  const solutions = new Array(pizzaTypeCount + 1);

  solutions[0] = new Array(maxSlices + 1);
  for (let i = 0; i < solutions[0].length; i++) {
    solutions[0][i] = {
      sliceCount: 0,
      pizzas: []
    };
  }

  for (
    let currentPizzaIndex = 1;
    currentPizzaIndex <= pizzaTypeCount;
    currentPizzaIndex++
  ) {
    if (currentPizzaIndex > 2) {
      console.warn(`Freeing ${currentPizzaIndex - 2}`);
      for (let i = 0; i < solutions[currentPizzaIndex - 2].length; i++) {
        solutions[currentPizzaIndex - 2][i] = null;
      }
      solutions[currentPizzaIndex - 2] = null;
      try {
        if (global.gc) {
          global.gc();
        }
      } catch (e) {
        console.log("`node --expose-gc index.js`");
        process.exit();
      }
    }
    solutions[currentPizzaIndex] = new Array(maxSlices + 1);
    for (
      let currentMaxSlices = 0;
      currentMaxSlices <= maxSlices;
      currentMaxSlices++
    ) {
      const currentPizzaSize = pizzaSizes[currentPizzaIndex - 1];
      if (currentPizzaSize > currentMaxSlices) {
        solutions[index(currentPizzaIndex)][currentMaxSlices] =
          solutions[index(currentPizzaIndex - 1)][currentMaxSlices];
      } else {
        const solutionWith =
          solutions[index(currentPizzaIndex - 1)][
            currentMaxSlices - currentPizzaSize
          ];
        const solutionWithout =
          solutions[index(currentPizzaIndex - 1)][currentMaxSlices];

        if (
          solutionWith.sliceCount + currentPizzaSize >
          solutionWithout.sliceCount
        ) {
          solution = {
            sliceCount: solutionWith.sliceCount + currentPizzaSize,
            pizzas: [currentPizzaIndex, ...solutionWith.pizzas]
          };
        } else {
          solution = {
            sliceCount: solutionWithout.sliceCount,
            pizzas: [...solutionWithout.pizzas]
          };
        }
        solutions[index(currentPizzaIndex)][currentMaxSlices] = solution;
      }
    }
  }

  return solutions[index(pizzaTypeCount)][maxSlices].pizzas;
};

module.exports = {
  greedySolver,
  recuriveSolver,
  dynamicSolver,
  leanDynamicSolver
};
