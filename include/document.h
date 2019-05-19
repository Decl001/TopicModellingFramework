#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <exception>
#include <fstream>
#include <set>
#include <map>
#include <vector>

#define STOP_WORDS_FILE "data/english_stopwords.txt"
#define WSTR_DOUBLE std::pair<std::wstring, double>


struct document_exception : std::runtime_error{
    document_exception(const std::string& message) : std::runtime_error(message){}
};

struct tf_idf_compare{
    bool operator() (const WSTR_DOUBLE elem1, const WSTR_DOUBLE elem2){
        return elem1.second > elem2.second;
    }
};

class Document{
private:
    int n_gram_count;
    std::vector<std::wstring> words;
    void load_words();
    void stem_words();
    void remove_stopwords();
    void compute_term_frequency();
public:
    std::string filename;
    std::map<std::wstring, double> tf;
    std::map<std::wstring, double> tf_idf;
    //std::set<WSTR_DOUBLE, tf_idf_compare> sorted_tf_idf; 
    Document(std::string, int);
    Document(std::string, std::map<std::wstring, double>);
    std::map<std::wstring, double> compute_tf_idf(std::map<std::wstring, double>);
    std::set<WSTR_DOUBLE, tf_idf_compare> sort_tf_idf();
};
#endif