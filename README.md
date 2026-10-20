# Wordle Bot

**A terminal Wordle assistant written in C++20, built from scratch to learn modern C++.**

Enter your guesses and Wordle's feedback to narrow the possible answers and get a frequency-based suggestion for your next move. The solver handles repeated letters, preserves previous constraints, and can suggest words outside the remaining answer set.

**C++20 · Standard library only · Terminal interface**

## Quick start

Requires a C++20-capable compiler. Developed with GCC 13.2.0 from MSYS2 on Windows, using VS Code and PowerShell.

From the repository root:

```powershell
g++ -std=c++20 -Wall -Wextra -Wpedantic main.cpp wordle.cpp -o wordlebot.exe
.\wordlebot.exe
```

Run from the repository root so the program can find the `data/` directory. Both `.cpp` files must be passed to the compiler; the header supplies declarations, while `wordle.cpp` supplies their definitions.

## Using the assistant

1. Play the suggested word in Wordle, or choose your own guess.
2. Enter the word you actually played.
3. Enter five feedback characters, from left to right:

| Character | Colour | Meaning |
| :---: | --- | --- |
| `g` | Green | Correct letter and position |
| `y` | Yellow | Correct letter, different position |
| `b` | Grey | No unused occurrence of that letter in the answer |

```text
Remaining answers: 2315
Frequency suggestion oater
Enter guess (5 lowercase letters): oater
Enter feedback (g/y/b): bbyyy

Remaining answers: 24
Frequency suggestion trice
```

Guesses must contain exactly five lowercase English letters. Feedback must contain exactly five characters drawn from `g`, `y`, and `b`.

If feedback leaves no matching answers, the assistant reports the problem and preserves the previous candidate set so you can retry. All-green feedback for a remaining answer ends the session. End-of-input also exits the program.

## How it works

### Reproducing Wordle feedback

The solver compares a guess against a candidate answer in two passes:

1. Mark greens and count the answer letters left unmatched.
2. Mark yellows only when an unused matching letter remains, consuming one occurrence each time.

Reserving greens first prevents duplicate letters from receiving too many yellow marks. For example, `allee` against `apple` produces `gybbg`.

### Filtering possible answers

For each candidate, generate the feedback that the submitted guess would produce. Keep the candidate only if that feedback exactly matches the user's observation.

Each round filters the surviving candidates from the previous round, retaining the constraints from the whole game.

### Choosing a guess

For every letter, count how many remaining answers contain it. Each answer contributes at most once per letter. A guess's score is the sum of these counts across its distinct letters:

```text
score(guess) = sum of candidate frequencies for each distinct letter in guess
```

The highest-scoring available word is suggested. Ties keep the first word in list order. Accepted guesses are removed from the suggestion pool; when only one candidate remains, that answer is suggested directly.

The two word pools serve different purposes:

| Pool | Role |
| --- | --- |
| Remaining answers | Used to calculate frequencies and track possible solutions |
| Available guesses | Words considered for suggestions, including words outside the remaining answers |

## Example result

The assistant solved a live puzzle with answer **WREST** in four guesses. The automated single-game runner reproduces the same sequence with the bundled lists:

| Turn | Guess | Feedback | Answers remaining |
| :---: | --- | :---: | ---: |
| 1 | `oater` | `bbyyy` | 24 |
| 2 | `trice` | `ygbby` | 2 |
| 3 | `erupt` | `ygbbg` | 1 |
| 4 | `wrest` | `ggggg` | Solved |

This is a worked example, not an aggregate performance result. Success rate and average guesses across the complete answer set have not yet been measured.

## Run the single-game benchmark

```powershell
g++ -std=c++20 -Wall -Wextra -Wpedantic benchmark.cpp wordle.cpp -o benchmark.exe
.\benchmark.exe
```

The runner currently uses the fixed answer `wrest`, generates its own feedback, and stops after success or six guesses. It shares the assistant's solver functions. `benchmark.cpp` and `main.cpp` are separate entry points, so build them as separate executables.

## Project structure

```text
main.cpp             Interactive terminal assistant
wordle.h             Shared function declarations
wordle.cpp           Loading, validation, feedback, filtering, and scoring
benchmark.cpp        Automated single-game runner
data/
  nyt-answers.txt     Initial possible-answer list
  nyt-guesses.txt     Additional guess-only words
  LICENSE            Word-list source's MIT licence
word.txt             Small development word list
```

## Word-list source

The bundled lists come from [Josh Stephenson's Wordle-Solver](https://github.com/joshstephenson/Wordle-Solver):

- [`nyt-answers.txt`](https://github.com/joshstephenson/Wordle-Solver/blob/main/nyt-answers.txt): 2,315 possible answers.
- [`nyt-guesses.txt`](https://github.com/joshstephenson/Wordle-Solver/blob/main/nyt-guesses.txt): 10,638 additional guesses in the local copy.

The program combines both files into an initial pool of 12,953 guess entries, while only the answer file supplies possible solutions. The source repository is published under the MIT licence; its copyright and licence notice are retained in [data/LICENSE](data/LICENSE).

These are fixed lists used for this project, not a guaranteed reflection of the current NYT answer and guess lists.

## Current limitations

- Letter frequency is a heuristic. It does not explicitly measure feedback partitions or expected information gain, and it does not guarantee a solution within six guesses.
- Suggestions can use obscure words from the allowed-guess list. Tie-breaking depends on file order.
- Guess input is checked for format, but is not checked for membership in the allowed-guess list.
- The interactive assistant trusts the user's feedback and has no independent access to the live answer. An empty result can indicate incorrect feedback or an answer missing from the dataset.
- The interactive assistant has no six-guess cap; the automated runner does.
- There is currently no persistent automated test suite. Development checks have covered validation, duplicate-letter feedback, filtering, frequency counts, scoring, and tie-breaking.

## Next steps

- Benchmark the frequency strategy across the complete answer set, reporting success rate, average guesses among successes, and failures separately.
- Add an entropy strategy and compare both approaches on the same dataset.
- Keep repeatable regression tests alongside the solver.

## Learning goals

This project is an exercise in moving from C and Python to modern C++: standard-library containers, references and `const`, automatic resource management, separate compilation, and algorithm evaluation. It uses no external dependencies or manual memory allocation.
