#pragma once // something to do with repeated inclusion?

#include <string>
#include <vector>
#include <array>
#include <map>

bool is_valid_word(const std::string& word);

bool is_valid_feedback(const std::string& feedback);

bool load_words(const std::string& filename, std::vector<std::string>& words);

std::string generate_feedback(
    const std::string& guess,
    const std::string& answer);

std::vector<std::string> filter_candidates(
    const std::vector<std::string>& candidates,
    const std::string& guess,
    const std::string& observed);

std::array<int, 26> count_letters(
    const std::vector<std::string>& candidates);

int score_guess(
    const std::string& guess,
    const std::array<int, 26>& frequencies);

std::string suggest(
    const std::vector<std::string>& allowed,
    const std::vector<std::string>& candidates);

std::map<std::string, int> count_feedback_patterns(
    const std::string& guess,
    const std::vector<std::string>& candidates);

double entropy_score(
    const std::string& guess,
    const std::vector<std::string>& candidates);

std::string suggest_entropy(
    const std::vector<std::string>& allowed,
    const std::vector<std::string>& candidates);