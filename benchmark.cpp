#include "wordle.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

// Each strategy has its own cache. With fixed lists and deterministic choices,
// the same feedback history implies the same candidates and available guesses.
using SuggestionCache = std::map<std::vector<std::string>, std::string>;

// Returns guesses on success, or 0 after six unsuccessful guesses.
int play_game(const std::string& answer,
              const std::vector<std::string>& answers,
              const std::vector<std::string>& allowed_guesses,
              bool use_entropy,
              SuggestionCache& cache)
{
    std::vector<std::string> candidates = answers;
    std::vector<std::string> available = allowed_guesses;
    std::vector<std::string> history;

    for (int turn = 1; turn <= 6; ++turn) {
        assert(!candidates.empty() && !available.empty());

        std::string guess;
        if (candidates.size() == 1) {
            guess = candidates.front();
        } else {
            auto cached = cache.find(history);
            if (cached == cache.end()) {
                guess = use_entropy ? suggest_entropy(available, candidates)
                                    : suggest(available, candidates);
                cache.emplace(history, guess);
            } else {
                guess = cached->second;
            }
        }

        const std::string feedback = generate_feedback(guess, answer);
        if (feedback == "ggggg") {
            return turn;
        }

        candidates = filter_candidates(candidates, guess, feedback);
        std::erase(available, guess);
        history.push_back(guess + feedback);
        assert(std::find(candidates.begin(), candidates.end(), answer) != candidates.end());
    }

    return 0;
}

bool benchmark(const std::vector<std::string>& answers,
               const std::vector<std::string>& allowed_guesses,
               bool use_entropy)
{
    const std::string strategy = use_entropy ? "entropy" : "frequency";
    const std::string filename = "results/" + strategy + ".csv";
    std::ofstream results(filename);
    if (!results) {
        std::cerr << "Could not open " << filename << '\n';
        return false;
    }
    results << "answer,solved,guesses\n";

    SuggestionCache cache;
    std::array<int, 7> distribution{}; // 0: failures; 1-6: successful guess counts.
    std::vector<std::string> failures;
    int total_successful_guesses = 0;
    const auto start = std::chrono::steady_clock::now();
    std::cout << "Running " << strategy << "..." << std::endl;

    for (const std::string& answer : answers) {
        const int guesses = play_game(answer, answers, allowed_guesses, use_entropy, cache);
        ++distribution[guesses];
        total_successful_guesses += guesses;
        if (guesses == 0) {
            failures.push_back(answer);
        }
        results << answer << ',' << (guesses != 0 ? "true" : "false") << ',';
        if (guesses != 0) {
            results << guesses;
        }
        results << '\n'; // A failure has no successful guess count.
    }

    const double seconds = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();
    results.close();
    if (!results) {
        std::cerr << "Could not finish writing " << filename << '\n';
        return false;
    }

    const auto successes = answers.size() - failures.size();
    std::cout << std::fixed << std::setprecision(3)
              << "Strategy: " << strategy << ", six-guess limit\n"
              << "Answers: " << answers.size() << '\n'
              << "Initial guess entries: " << allowed_guesses.size() << '\n'
              << "Solved: " << successes << '\n'
              << "Failed: " << failures.size() << '\n'
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
    std::cout << "Elapsed seconds (shared histories cached): " << seconds << '\n'
              << "Distinct cached suggestions: " << cache.size() << '\n'
              << "Per-answer results: " << filename << '\n'
              << "Failed answers:";
    for (const std::string& answer : failures) {
        std::cout << ' ' << answer;
    }
    std::cout << "\n\n";
    return true;
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

    // Compare shared-cache and fresh-cache outcomes on a small answer set.
    const std::vector<std::string> small_answers{"crane", "crave", "crate", "slate"};
    for (bool use_entropy : {false, true}) {
        SuggestionCache shared_cache;
        for (const std::string& answer : small_answers) {
            SuggestionCache fresh_cache;
            const int fresh_result = play_game(answer, small_answers, small_answers,
                                               use_entropy, fresh_cache);
            const int shared_result = play_game(answer, small_answers, small_answers,
                                                use_entropy, shared_cache);
            assert(shared_result == fresh_result);
            assert(shared_result > 0 && shared_result <= 6);
        }
        SuggestionCache singleton_cache;
        assert(play_game("apple", {"apple"}, {"apple"}, use_entropy, singleton_cache) == 1);
    }
    SuggestionCache live_example_cache;
    assert(play_game("wrest", answers, allowed_guesses, false, live_example_cache) == 4);

    std::error_code directory_error;
    std::filesystem::create_directories("results", directory_error);
    if (directory_error) {
        std::cerr << "Could not create results directory: " << directory_error.message() << '\n';
        return 1;
    }
    if (!benchmark(answers, allowed_guesses, false)
        || !benchmark(answers, allowed_guesses, true)) {
        return 1;
    }
}