#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <exception>
#include <fstream>
#include <map>
#include <vector>

#define STOP_WORDS_FILE "data/english_stopwords.txt"

struct document_exception : std::runtime_error{
    document_exception(const std::string& message) : std::runtime_error(message){}
};

class Document{
private:
    int n_gram_count;
    std::vector<std::wstring> words;
    std::string filename;
    void load_words();
    void stem_words();
    void remove_stopwords();
    void compute_term_frequency();
public:
    std::map<std::wstring, double> tf;
    std::map<std::wstring, double> tf_idf;
    Document(std::string, int);
    Document(std::string, std::map<std::wstring, double>);
    std::map<std::wstring, double> compute_tf_idf(std::map<std::wstring, double>);
};
#endif