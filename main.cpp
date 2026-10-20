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





int main(){
    // variable decleration
    std::string guess, feedback;
    std::vector<std::string> candidates{"crane", "brain", "crave", "slave", "crate", "creed", "cruel"};
    std::vector<std::string> result;
    
    // main loop
    while(true){
        // gathering inputs
        std::cout << "Enter word: "; 
        if(!std::getline(std::cin, guess)){
            return 0;
        }

        
        std::cout << "Enter feedback: "; 
        if(!std::getline(std::cin, feedback)){
            return 0;
        }


        if(!is_valid_word(guess)){
            std::cerr << "Non-valid word\n";
            continue;
        }

        if(!is_valid_feedback(feedback)){
            std::cerr << "Non-valid feedback\n";
            continue;
        }


        // begin filtering
        result = filter_candidates(candidates, guess, feedback);

        if(result.empty()){
            std::cout << "Database exhausted, try again\n";
            continue;
        }
        else if(feedback == "ggggg" && result[0] == guess){
            std:: cout << "Solved";
            return 0;
        }
        else{
            candidates = result;
        }

        std::cout << "\n========== REMAINING ==========\n";
        for(std::string& word : candidates){
            std::cout << word;
            std::cout << "\n";
        }
        
        std::cout << "\n";


    }
  

    return 0;
}