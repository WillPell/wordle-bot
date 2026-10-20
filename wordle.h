#pragma once // something to do with repeated inclusion?

#include <string>
#include <vector>
#include <array>

bool is_valid_word(const std::string& word);

bool is_valid_feedback(const std::string& feedback);

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