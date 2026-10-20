#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <cassert> // testing


bool is_valid_word(const std::string& word){

    for(char letter : word){
        if(letter < 'a' || letter > 'z'){
            std::cout << "Invalid characters\n";
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



int main(){
    // variable decleration
    std::string guess, feedback;
    std::vector<std::string> candidates;
    std::vector<std::string> result;
    std::string word;
    

    // filling candidates
    std::ifstream input("word.txt");

    if(!input){
        std::cerr << "File opening error";
        return 1;
    }

    while(std::getline(input, word)){
        if(is_valid_word(word)){
            candidates.push_back(word);
        }
    }

    if(candidates.empty()){
        std::cerr << "File filling error";
        return 1;
    }



    // main loop
    while(true){
        // suggestions
        std::cout << suggest(candidates, candidates);
        std::cout << '\n';


        // gathering inputs
        std::cout << "Enter word: "; 
        if(!std::getline(std::cin, guess)){
            return 0;
        }

        if(!is_valid_word(guess)){
            std::cerr << "Non-valid word\n";
            continue;
        }

        
        std::cout << "Enter feedback: "; 
        if(!std::getline(std::cin, feedback)){
            return 0;
        }

        if(!is_valid_feedback(feedback)){
            std::cerr << "Non-valid feedback\n";
            continue;
        }


        // begin filtering
        result = filter_candidates(candidates, guess, feedback);

        if(result.empty()){
            std::cout << "No current matches\n";
            continue;
        }
        else if(feedback == "ggggg"){
            std:: cout << "Solved";
            return 0;
        }
        else{
            candidates = result;
        }

        // print remainders
        std::cout << "\n========== REMAINING ==========\n";
        for(std::string& word : candidates){
            std::cout << word;
            std::cout << "\n";
        }
        
        std::cout << "\n";


    }
  

    return 0;
}
