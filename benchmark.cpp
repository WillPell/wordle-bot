#include "wordle.h"

#include <algorithm>
#include <iostream>

int main()
{
    const std::string answer = "wrest";
    std::vector<std::string> candidates;

    if (!load_words("data/nyt-answers.txt", candidates) || candidates.empty()) {
        std::cerr << "Could not load a nonempty answer list\n";
        return 1;
    }

    if (std::find(candidates.begin(), candidates.end(), answer) == candidates.end()) {
        std::cerr << "Test answer is missing from the answer list\n";
        return 1;
    }

    std::vector<std::string> available = candidates;
    if (!load_words("data/nyt-guesses.txt", available)) {
        std::cerr << "Could not load data/nyt-guesses.txt\n";
        return 1;
    }

    std::cout << "Frequency strategy, answer: " << answer << '\n';

    for (int turn = 1; turn <= 6; ++turn) {
        if (candidates.empty() || available.empty()) {
            std::cerr << "Unexpected empty candidate or guess pool\n";
            return 1;
        }

        std::string guess;
        if (candidates.size() == 1) {
            guess = candidates.front();
        } else {
            guess = suggest(available, candidates);
        }

        const std::string feedback = generate_feedback(guess, answer);
        std::cout << turn << ": " << guess << "  " << feedback << '\n';

        if (feedback == "ggggg") {
            std::cout << "Solved in " << turn << " guesses\n";
            return 0;
        }

        candidates = filter_candidates(candidates, guess, feedback);
        std::erase(available, guess);
        std::cout << "Remaining answers: " << candidates.size() << '\n';
    }

    std::cout << "Failed to solve within 6 guesses\n";
    return 0;
}
