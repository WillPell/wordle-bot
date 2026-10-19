#include <iostream>
#include <fstream>
#include <string>
#include <vector>


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

    for(std::size_t i = 0; i < 5; i++){ // range being hardcoded is fine bc is_valid_func will ensure 5 letter word
        if(guess[i] == answer[i]){
            output[i] = 'g';
        }
    }

    return output;
}


int main(){

    std::cout << generate_feedback("crane", "crave") << '\n';


    return 0;
}