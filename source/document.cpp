#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "document.h"

Document::Document(std::string f_name){
    filename = f_name;
    load_words();
    stem_words();
    remove_stopwords();
    //compute_term_frequency();
}

void Document::load_words(){
    std::ifstream the_document;
    the_document.open(filename);
    if (the_document.is_open()){
        for(std::wstring word; the_document >> word;){
            words.push_back(word);
        }
    }
    else{
        throw document_exception("Could not load the file");
    }
}

void Document::stem_words(){
    
}

void Document::remove_stopwords(){
    std::vector<std::wstring> stop_words_removed;
    std::vector<std::wstring> stop_words;
    std::ifstream stop_words_file;
    stop_words_file.open(STOP_WORDS_FILE);
    if (stop_words_file.is_open()){
        for(std::wstring word; stop_words_file >> word;){
            stop_words.push_back(word);
        }
    }
    else{
        throw document_exception("Could not load the stop words file");
    }

    for(auto it = words.begin(); it != words.end(); it++){
        if (std::find(stop_words.begin(), stop_words.end(), *it) == stop_words.end()){
            stop_words_removed.push_back(*it);
        }
    }
    words = stop_words_removed;
}

int main(){
    Document d("test_data/test_document.txt");
}