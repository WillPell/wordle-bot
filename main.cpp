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



int main(){
    std::ifstream input("word.txt");
    std::string word;
    std::vector<std::string> words; 

    if(!input){
        std::cerr << "Error in input file";
        return 1;
    }

    while(std::getline(input, word)){
        if(is_valid_word(word)){
            words.push_back(word);
        }
    }


    std::cout << words.size() << '\n';

    return 0;
}