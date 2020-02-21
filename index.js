const fs = require('fs');

const charset = 'utf-8';
const eol = "\n";
const tab = " ";

const inputs = [
    "a_example.txt",
    "b_read_on.txt",
    "c_incunabula.txt",
    "d_tough_choices.txt",
    "e_so_many_books.txt",
    "f_libraries_of_the_world.txt"
];

function chunkToNumbers(line) {
    return line.split(tab).map(Number)
}

function parse(filename) {
    const data = fs.readFileSync(filename, charset).split(eol);
    const [totalBooksCount, librariesCount, availableDays] = chunkToNumbers(data.shift());
    const booksScores = chunkToNumbers(data.shift());
    const libraries = [];
    for (let i = 0; i < data.length; i += 2) {
        const id = i / 2;
        const [booksCount, signUpDuration, booksShippedPerDay] = chunkToNumbers(data.shift());
        const bookIds = chunkToNumbers(data.shift()).sort((a, b) => booksScores[b] - booksScores[a]);
        const maxScore = bookIds.reduce((a, i) => a + booksScores[i], 0);
        libraries.push({
            id,
            booksCount,
            maxScore,
            signUpDuration,
            booksShippedPerDay,
            bookIds,
            booksScores,
            availableDays
        })
    }
    return {
        filename,
        totalBooksCount,
        booksScores,
        librariesCount,
        availableDays,
        libraries
    }
}

function chooseBook(bookIds, knownBooks) {
    do {
        const bookId = bookIds.pop();
        if (!knownBooks.includes(bookId)) {
            return bookId;
        }
    } while (bookIds.length >= 0);

    return -1;
}

function createIterativeEvaluator() {
    const knownBooks = [];

    return ({ signUpDuration, booksShippedPerDay, bookIds, booksScores, availableDays }) => {
        let score = 0;
        for (let i = availableDays - signUpDuration, j = bookIds.length - 1; i >= 0 && j >= 0; --i, --j) {
            const bookId = bookIds[j];
            if (!knownBooks.includes(bookIds)) {
                score += booksScores[bookId];
                knownBooks.push(bookId);
            }
        }
        return score;
    };
}


// function formulaEvaluator({ signUpDuration, booksShippedPerDay, maxScore }) {
//     return 1 - signUpDuration / (maxScore * booksShippedPerDay);
// }

function compareLibraries(a, b) {
    const evaluate = createIterativeEvaluator();
    return evaluate(b) - evaluate(a);
}

function initializeSubSolution(library) {
    return {
        id: library.id,
        booksCount: 0,
        bookIds: []
    }
}

function initializeSolution(filename) {
    return { filename, subSolutions: [] };
}

function solve({ totalBooksCount, booksScores, librariesCount, availableDays, libraries, filename }) {
    const solution = initializeSolution(filename), knownBooks = [];

    for (let i = availableDays; i >= 0; --i) {
        for (const library of libraries) {
            --library.signUpDuration;

            if (library.signUpDuration > 0) {
                continue;
            }

            if (library.bookIds.length === 0) {
                continue;
            }

            let subSolution;

            if (library.signUpDuration === 0) {
                subSolution = initializeSubSolution(library);
                solution.subSolutions.push(subSolution);
            } else {
                subSolution = solution.subSolutions.find(s => s.id === library.id);
            }

            for (let book = library.booksShippedPerDay; book > 0; --book) {
                const bookId = chooseBook(library.bookIds, knownBooks);
                if (bookId > 0) {
                    subSolution.bookIds.push(bookId);
                    subSolution.booksCount = subSolution.bookIds.length;
                    knownBooks.push(bookId);
                }
            }
        }
    }
    solution.subSolutions = solution.subSolutions.filter(library => library.booksCount > 0);
    return solution;
}

function dump({ subSolutions, filename }) {
    let output = `${ subSolutions.length }\n`;
    for (const library of subSolutions) {
        output += `${ library.id } ${ library.booksCount }\n`;
        output += `${ library.bookIds.join(' ') }\n`
    }
    fs.writeFileSync(`${ filename }.out`, output);
}

function run() {
    for (const input of inputs) {
        const instance = parse(input);
        instance.libraries.sort(compareLibraries);
        dump(solve(instance));
    }
}


run();