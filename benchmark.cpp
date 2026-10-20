#include "wordle.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

// Returns the number of guesses on success, or 0 after six unsuccessful guesses.
int play_game(const std::string& answer,
              const std::vector<std::string>& answers,
              const std::vector<std::string>& allowed_guesses)
{
    std::vector<std::string> candidates = answers;
    std::vector<std::string> available = allowed_guesses;

    for (int turn = 1; turn <= 6; ++turn) {
        assert(!candidates.empty() && !available.empty());

        const std::string guess = candidates.size() == 1
            ? candidates.front() : suggest(available, candidates);
        const std::string feedback = generate_feedback(guess, answer);

        if (feedback == "ggggg") {
            return turn;
        }

        candidates = filter_candidates(candidates, guess, feedback);
        std::erase(available, guess);
        // The known answer must survive every round of generated feedback.
        assert(std::find(candidates.begin(), candidates.end(), answer) != candidates.end());
    }

    return 0;
}

int main()
{
    std::vector<std::string> answers;
    if (!load_words("data/nyt-answers.txt", answers) || answers.empty()) {
        std::cerr << "Could not load a nonempty answer list\n";
        return 1;
    }

    std::vector<std::string> allowed_guesses = answers;
    if (!load_words("data/nyt-guesses.txt", allowed_guesses)) {
        std::cerr << "Could not load data/nyt-guesses.txt\n";
        return 1;
    }

    // Check immediate success and the previously verified live-game example.
    assert(play_game("apple", {"apple"}, {"apple"}) == 1);
    assert(play_game("wrest", answers, allowed_guesses) == 4);

    std::ofstream results("benchmark-results.csv");
    if (!results) {
        std::cerr << "Could not open benchmark-results.csv\n";
        return 1;
    }
    results << "answer,solved,guesses\n";

    std::array<int, 7> distribution{}; // Index 0 counts failures; 1-6 count successes.
    int total_successful_guesses = 0;
    const auto start = std::chrono::steady_clock::now();

    for (const std::string& answer : answers) {
        const int guesses = play_game(answer, answers, allowed_guesses);
        ++distribution[guesses];
        total_successful_guesses += guesses;
        results << answer << ',' << (guesses != 0 ? "true" : "false") << ',';
        if (guesses != 0) {
            results << guesses;
        }
        results << '\n'; // Failed games leave guesses blank rather than implying a solution.
    }

    const double seconds = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();
    results.close();
    if (!results) {
        std::cerr << "Could not finish writing benchmark-results.csv\n";
        return 1;
    }

    const int failures = distribution[0];
    const auto successes = answers.size() - failures;
    std::cout << std::fixed << std::setprecision(3)
              << "Strategy: letter frequency, six-guess limit\n"
              << "Answers: " << answers.size() << '\n'
              << "Initial guess entries: " << allowed_guesses.size() << '\n'
              << "Solved: " << successes << '\n'
              << "Failed: " << failures << '\n'
              << "Success rate: " << 100.0 * successes / answers.size() << "%\n";
    if (successes != 0) {
        std::cout << "Average guesses (successful games only): "
                  << static_cast<double>(total_successful_guesses) / successes << '\n';
    } else {
        std::cout << "Average guesses (successful games only): N/A\n";
    }
    std::cout << "Failures are excluded from the average; each used all six guesses.\n";
    for (int turn = 1; turn <= 6; ++turn) {
        std::cout << "Solved in " << turn << ": " << distribution[turn] << '\n';
    }
    std::cout << "Elapsed benchmark seconds: " << seconds << '\n'
              << "Per-answer results: benchmark-results.csv\n";
}