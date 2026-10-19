#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>


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


int main(){

    std::cout << generate_feedback("creed", "crane") << '\n';


    return 0;
}