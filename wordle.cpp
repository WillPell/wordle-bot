#include "wordle.h"
#include <iostream>
#include <fstream>


bool is_valid_word(const std::string& word){

    for(char letter : word){
        if(letter < 'a' || letter > 'z'){
            return false;
        }
    }

    if(word.size() != 5){
        return false;
    }

    return true;
}

bool is_valid_feedback(const std::string& feedback){
    for(char letter : feedback){
        if(letter != 'g' && letter != 'y' && letter != 'b'){
            return false;
        }
    }

    if(feedback.size() != 5){
        return false;
    }

    return true;
}


bool load_words(const std::string& filename, std::vector<std::string>& words){
    std::ifstream input(filename);
    std::string word;

    if(!input){
        return false;
    }

    while(std::getline(input, word)){
        if(is_valid_word(word)){
            words.push_back(word);
        }
    }

    return !input.bad();

}


std::string generate_feedback(const std::string& guess, const std::string& answer){
    std::string output(5, 'b'); // init grey
    std::array<int, 26> remaining{}; 

    for(std::size_t i = 0; i < 5; i++){ // range being hardcoded is fine bc is_valid_func will ensure 5 letter word
        if(guess[i] == answer[i]){
            output[i] = 'g';
        }
        else{
            remaining[answer[i] - 'a'] += 1;
        }

    }

    for(std::size_t i = 0; i < 5; i++){
        if(output[i] == 'b'){
                if(remaining[guess[i] - 'a'] > 0){
                    output[i] = 'y';
                    remaining[guess[i] - 'a']--;
            }

        }
    }

    return output;
}

std::vector<std::string> filter_candidates(const std::vector<std::string>& candidates, const std::string& guess, const std::string& observed){
    std::vector<std::string> result;

    for(const std::string& candidate : candidates){
        std::string predicted = generate_feedback(guess, candidate);
            if(predicted == observed){
                result.push_back(candidate);
            }
    }
    return result;

}


std::array<int, 26> count_letters(const std::vector<std::string>& candidates){
    std::array<int, 26> freq{0};

    for(const std::string& word : candidates){
        std::array<bool, 26> seen{};
        for(char letter : word){
            const int index = letter - 'a';
            if(!seen[index]){
                seen[index] = true;
                freq[index] += 1;
            }
        }
    }

    return freq;
}


int score_guess(const std::string& guess, const std::array<int, 26>& frequencies){
    int score = 0;
    std::array<bool, 26> seen{};

    for(const char letter : guess){
        int index = letter - 'a';
        if(!seen[index]){
            score += frequencies[index];
            seen[index] = true;
        }
    }


    return score;
}


std::string suggest(const std::vector<std::string>& allowed, const std::vector<std::string>& candidates){
    std::array<int, 26> candidates_freq = count_letters(candidates);
    std::string bestGuess = allowed.front();
    int score = 0;
    int highest = score_guess(bestGuess, candidates_freq);
    
    for(const std::string& guess : allowed){
        score = score_guess(guess, candidates_freq);


        if(score > highest){
            highest = score;
            bestGuess = guess;
        }
    }
    return bestGuess;
}



