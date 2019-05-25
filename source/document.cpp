/**
 * Author: Declan Atkins
 * Last Modified: 22/05/19
 * 
 * Contains the source code for the document class
 */

#include <algorithm>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "document.h"
#include "stemming/english_stem.h"


bool is_punct_extended(wchar_t ch){
    return iswpunct(ch) || ch == L'“' || ch == L'’' || ch == L'�' || ch == L' ';
}

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
    extra_removal_conditions();
    compute_term_frequency();
    filter_terms();
}

/**
 * Constructor for a document object for loading from model
 * 
 * @param f_name -> The name of the file the object refers to
 * @param _tf_idf -> The tf-idf values of the document
 */ 
Document::Document(std::string f_name, std::map<std::wstring, double> _tf_idf){
    filename = f_name;
    tf_idf = _tf_idf;
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
            std::wstring word_no_punc = L"";
            for (wchar_t c : word){
                if(!is_punct_extended(c)){
                    word_no_punc.push_back(towlower(c));
                }
            }
            if (word_no_punc.length()){
                words.push_back(word_no_punc);
            }
        }
    }
    else{
        throw document_exception("Could not load the file: " + filename);
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
 * the document.h header file. Throws a document exception
 * if this file can't be loaded
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


void Document::extra_removal_conditions(){
    std::vector<std::wstring> modified_words;
    for (auto it = words.begin(); it != words.end(); it++){
        int n_alpha = 0;
        int n_not_alpha = 0;
        for(wchar_t c : *it){
            if (iswalpha(c)){
                n_alpha++;
            }
            else{
                n_not_alpha++;
            }
        }
        if (n_alpha >= n_not_alpha){
            modified_words.push_back(*it);
        }
    }
    words = modified_words;
}


/**
 * Computes term frequency from the words vector
 * stores the frequency in a map with the term as the
 * key and the frequency as the value
 * 
 * When computing n-gram tf, the key will be:
 * "the_first_word->the_second_word->etc"
 * 
 * Formula for tf:
 * ln( (x / max_x) + 1)
 */
void Document::compute_term_frequency(){
    
    // compute raw frequency
    for(int n_grams = 0; n_grams < n_gram_count; n_grams++){
        for(int i = 0; i < words.size(); i++){
            if (i >= n_grams){
                std::wstring term = words[i - n_grams];
                for(int j = i - n_grams + 1; j <= i; j++){
                    term += L"->" + words[j];
                }
            
                std::pair<std::wstring, double> insert_pair(term, 1.0);
                std::pair<std::map<std::wstring, double>::iterator, bool> result;
                result = tf.insert(insert_pair);
                if (result.second == false){
                    tf[term]++;
                }
            }
        }
    }
    // compute max frequency
    double max_freq = 0;
    for (auto it = tf.cbegin(); it != tf.cend(); it++){
        if (it->second > max_freq){
            max_freq = it->second;
        }
    }
    // compute relative tf
    for (auto it = tf.cbegin(); it != tf.cend(); it++){
        double raw_freq = it->second;
        double relative_freq = std::log((raw_freq / max_freq) + 1);
        tf[it->first] = relative_freq;
    }
}


/**
 * Computes tf_idf of the document if it has not already been calculated
 * 
 * @param idf -> The map of inverse document frequency of the dataset
 * 
 * @return tf_idf -> the map of tf_idf for this document
 */ 
std::map<std::wstring, double> Document::compute_tf_idf(std::map<std::wstring, double> idf){
    
    if (tf_idf.empty()){
        for (auto it = tf.cbegin(); it != tf.cend(); it++){
            double tf_idf_value = it->second * idf[it->first];
            std::pair<std::wstring, double> insert_pair(it->first, tf_idf_value);
            tf_idf.insert(insert_pair);
        }
        filter_tf_idf();
    }
    return tf_idf;
}

std::multimap<double, std::wstring> Document::sort_tf_idf(){
    if (sorted_tf_idf.empty()){
        for (auto it = tf_idf.begin(); it != tf_idf.end(); it++){
            std::pair<double, std::wstring> insert_pair(it->second, it->first);
            sorted_tf_idf.insert(insert_pair);
        }
    }
    return sorted_tf_idf;
}


void Document::filter_terms(){
    std::map<std::wstring, double> filtered_tf;
    std::cout << "N-vals before: " << tf.size() << std::endl;
    double sum_values = 0;
    double average_value;
    for (auto it = tf.cbegin(); it != tf.cend(); it++){
        sum_values += it->second;
    }
    average_value = sum_values / tf.size();
    for (auto it = tf.cbegin(); it != tf.cend(); it++){
        if (it->second >= average_value){
            filtered_tf[it->first] = it->second;
        }
    }
    tf = filtered_tf;
    std::cout << "N-vals after: " << tf.size() << std::endl;
}


void Document::filter_tf_idf(){
    std::map<std::wstring, double> filtered_tf_idf;
    std::cout << "N-vals before: " << tf_idf.size() << std::endl;
    double sum_values = 0;
    double average_value;
    for (auto it = tf_idf.cbegin(); it != tf_idf.cend(); it++){
        sum_values += it->second;
    }
    average_value = sum_values / tf_idf.size();
    for (auto it = tf_idf.cbegin(); it != tf_idf.cend(); it++){
        if (it->second >= (0.8 * average_value)){
            filtered_tf_idf[it->first] = it->second;
        }
    }
    tf_idf = filtered_tf_idf;
    std::cout << "N-vals after: " << tf_idf.size() << std::endl;
}
