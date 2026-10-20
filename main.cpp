#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <cassert> // testing
#include "wordle.h" // header file


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
