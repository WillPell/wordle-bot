#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <cassert> // testing
#include <algorithm>

#include "wordle.h" // header file


int main(){
    // variable decleration
    std::string guess, feedback;
    std::vector<std::string> candidates;
    std::vector<std::string> result;
    std::string word;
    std::string choice;
    

    // load answers
    if (!load_words("data/nyt-answers.txt", candidates)) {
        std::cerr << "Could not load data/nyt-answers.txt\n";
        return 1;
    }

    if(candidates.empty()){
        std::cerr << "File filling error\n";
        return 1;
    }

    // copy vector
    std::vector<std::string> available = candidates;

    // load guesses
    if (!load_words("data/nyt-guesses.txt", available)) {
        std::cerr << "Could not load data/nyt-guess.txt\n";
        return 1;
    }

    // selection
    while(1){
        std::cout << "Select mode: frequency (f) or entropy (e): ";
        if(!std::getline(std::cin, choice)){
            std::cout << "\nError in choice input\n";
            return 1;
        }

        if(choice != "f" && choice != "e"){
            std::cout << "\nInvalid selection, try again\n";
        }
        else{
            break; 
        }
    }


    // main loop
    while(true){
        std::cout << "\nRemaining answers: " << candidates.size() << '\n';
        if(choice == "f"){
            std::cout << "Frequency suggestion: "; // no \n so it continues on
        }
        else if(choice == "e"){
            std::cout << "Entropy suggestion: ";
        }


        // suggestions
        if(candidates.size() == 1){
            std::cout << candidates[0];
        }
        else if(choice == "f"){
            std::cout << suggest(available, candidates);
        }
        else if(choice == "e"){
            std::cout << suggest_entropy(available, candidates);
        }
        else{
            std::cout << "Error in suggestion choice";
            return 1;
        }
        std::cout << '\n';


        // gathering inputs
        std::cout << "Enter guess (5 lowercase letters): "; 
        if(!std::getline(std::cin, guess)){
            return 0;
        }

        if(!is_valid_word(guess)){
            std::cerr << "Invalid word, ensure that it's 5 lowercase letters and a-z\n";
            continue;
        }

        
        std::cout << "Enter feedback (g/y/b): "; 
        if(!std::getline(std::cin, feedback)){
            return 0;
        }

        if(!is_valid_feedback(feedback)){
            std::cerr << "Invalid feedback, ensure that it's 5 characters consisting of g, y or b\n";
            continue;
        }



        // begin filtering
        result = filter_candidates(candidates, guess, feedback);

        if(result.empty()){
            std::cout << "No current matches, ensure you have entered words and feedback correctly\nIf entered correctly, word is likely missing form word list\n";
            continue;
        }
        else if(feedback == "ggggg"){
            std:: cout << "Solved\n";
            return 0;
        }
        else{
            candidates = result;
            std::erase(available, guess);
        }


    }
  

    return 0;
}
