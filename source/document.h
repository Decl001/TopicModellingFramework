#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <fstream>
#include <map>
#include "python_include.h"

class Document{
private:
    char** words;
    char* filename;
    std::map<char*, int> tf;
    void remove_stopwords(char**);
    std::map<char*, int> compute_term_frequency(const char**);
public:
    Document(char* document_filename);
    ~Document();

};
#endif