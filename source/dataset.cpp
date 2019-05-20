/**
 * Author: Declan Atkins
 * Last Modified: 19/05/19
 * 
 * Contains the source code for the dataset class
 */

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include "dataset.h"
#include "document.h"


Dataset::Dataset(std::string filename, dataset_type_t type=TEXT_DATASET, int n_grams=1){

    switch(type){
        case BINARY_MODEL:
            break;
        case ZIPFILE:
            break;
        default: // TEXT_DATASET
            if (check_directory(filename)){
                std::vector<std::string> files = list_directory(filename);
                for(auto it = files.cbegin(); it != files.cend(); it++){
                    documents.push_back(Document(*it, n_grams));
                    std::cout << "Created Document: " + *it << std::endl;
                }
                std::cout << "Finished Creating Documents" << std::endl;
                compute_idf();
                std::cout << "Finished computing idf" << std::endl;
                compute_tf_idf_for_documents();
                std::cout << "Finished computing tf-idf" << std::endl;
            }
            else{
                throw dataset_exception("When using TEXT_DATASET filename must refer to a directory");
            }
    }
}


void Dataset::compute_idf(){
    double n_articles = documents.size();
    // First calculate raw frequency
    for (auto d_it = documents.cbegin(); d_it != documents.cend(); d_it++){   
        for (auto m_it = d_it->tf.cbegin(); m_it != d_it->tf.cend(); m_it++){
            std::pair<std::wstring, double> insert_pair(m_it->first, 1);
            std::pair<std::map<std::wstring, double>::iterator, bool> result;
            result = idf.insert(insert_pair);
            if (!result.second){
                idf[m_it->first]++;
            }
        }
    }
    // Now compute idf
    for (auto it = idf.begin(); it != idf.end(); it++){
        it->second = std::log(n_articles / it->second);
    }
}


void Dataset::compute_tf_idf_for_documents(){
    for (auto it = documents.begin(); it != documents.end(); it++){
        it->compute_tf_idf(idf);
        std::cout << "Finished computing tf-idf for document: " + it->filename << std::endl;
    }
}


void Dataset::output_keywords(int n_keywords=10){
    for(auto d_it = documents.begin(); d_it != documents.end(); d_it++){
        std::multimap<double, std::wstring> sorted_tf_idf = d_it->sort_tf_idf();
        int pos = 0;
        std::cout << "----------" << d_it->filename << "----------" << std::endl;
        for (auto it = sorted_tf_idf.rbegin(); it != sorted_tf_idf.rend(); it++){
            if (pos >= n_keywords) {break;}
            std::wcout << it->second << L"   ->   " << it->first << std::endl;
            pos++;
        }
    }
}


int main(){

    Dataset d("data/datasets/ETNCOEHR/articles", TEXT_DATASET, 4);
    d.output_keywords();
}