/**
 * Author: Declan Atkins
 * Last Modified: 11/05/19
 * 
 * Contains the source code for the document class
 */

#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "document.h"
#include "english_stem.h"

/**
 * Constructor for a document object
 * 
 * @param f_name -> The name of the file to create the document from
 * @param n_gram_c -> The number of n-grams to use (Up to this number of n-grams 
 *                    will be used)
 */ 
Document::Document(std::string f_name, int n_gram_c=1){
    filename = f_name;
    n_gram_count = n_gram_c;
    load_words();
    stem_words();
    remove_stopwords();
    compute_term_frequency();
}


/**
 * Loads the words from the file into a vector
 * Uses std::wstring to load unicode files
 * 
 * Throws a document_exception if the file cannot be loaded
 */ 
void Document::load_words(){
    std::wifstream the_document;
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


/**
 * Performs stemming on the words loaded from the file
 * 
 * Uses the Oleander stemming library:
 * https://github.com/OleanderSoftware/OleanderStemmingLibrary
 */
void Document::stem_words(){
    stemming::english_stem<> english_stemmer;   
    for (auto it = words.begin(); it != words.end(); it++){
        english_stemmer(*it);
    } 
}


/**
 * Removes stopwords from the vector
 * Stop words are loaded from the file specified in
 * the document.h header file
 */
void Document::remove_stopwords(){
    std::vector<std::wstring> stop_words_removed;
    std::vector<std::wstring> stop_words;
    std::wifstream stop_words_file;
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


/**
 * Computes term frequency from the words vector
 * stores the frequency in a map with the term as the
 * key and the frequency as the value
 * 
 * When computing n-gram tf, the key will be:
 * "the_first_word->the_second_word->etc"
 */
void Document::compute_term_frequency(){
    for(int n_grams = 0; n_grams < n_gram_count; n_grams++){
        for(int i = 0; i < words.size(); i++){
            if (i >= n_grams){
                std::wstring term = words[i - n_grams];
                for(int j = i - n_grams + 1; j <= i; j++){
                    term += L"->" + words[j];
                }
            
                std::pair<std::wstring, int> insert_pair(term, 1);
                std::pair<std::map<std::wstring, int>::iterator, bool> result;
                result = tf.insert(insert_pair);
                if (result.second == false){
                    tf[term]++;
                }
            }
        }
    }
}
