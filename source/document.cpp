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
        for(std::string word; the_document >> word;){
            words.push_back(word);
        }
    }
    else{
        throw document_exception("Could not load the file");
    }
}

void Document::remove_stopwords(){
}

int main(){
    Document d("test_data/test_document.txt");
}