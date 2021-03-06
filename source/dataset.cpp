/**
 * Author: Declan Atkins
 * 
 * Contains the source code for the dataset class
 */

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include "dataset.h"
#include "document.h"


Dataset::Dataset(std::string filename, dataset_type_t type=TEXT_DATASET, int n_grams=1){

    switch(type){
        case BINARY_MODEL:{
            break;
        }
        case ZIPFILE:{
            break;
        }
        case CSV:{
            std::wifstream csv_file;
            csv_file.open(filename);
            if (csv_file.is_open()){
                // parse first line which has column (ie word) names
                std::wstring header;
                csv_file >> header;
                std::vector<std::wstring> words;
                int pos;
                int i = 0;
                while((pos = header.find(L",")) != std::wstring::npos){
                    std::wstring word = header.substr(0, pos);
                    header.erase(0, pos+1);
                    if (i > 0){
                        words.push_back(word);
                    }
                    i++;
                }
                for (std::wstring line; csv_file >> line; i++){
                    i = 0;
                    std::string filename;
                    std::map<std::wstring, double> tf_idf;
                    while((pos = line.find(L",")) != std::wstring::npos){
                        std::wstring word = line.substr(0, pos);
                        line.erase(0, pos+1);
                        if (i > 0){
                            double value = std::stod(word, nullptr);
                            if (value > 0){
                                tf_idf[words[i]] = value;
                            }
                        }
                        else{
                            filename.assign(word.begin(), word.end());
                        }
                        i++;
                    }
                    Document d(filename, tf_idf);
                    std::cout << "Created Document: " << filename << std::endl;
                    documents.push_back(d);
                }  
            }
            else{
                throw dataset_exception("Could not open the csv file");
            }
            break;
        }
        default:{ // TEXT_DATASET
            if (check_directory(filename)){
                std::vector<std::string> files = list_directory(filename);
                for(auto it = files.cbegin(); it != files.cend(); it++){
                    documents.push_back(Document(*it, n_grams));
                    std::cout << "Created Document: " + *it << std::endl;
                }
                std::cout << "Finished Creating Documents" << std::endl;
                compute_idf();
                std::cout << "Finished computing idf" << std::endl;
                std::cout << idf.size();
                compute_tf_idf_for_documents();
                std::cout << "Finished computing tf-idf" << std::endl;
            }
            else{
                throw dataset_exception("When using TEXT_DATASET filename must refer to a directory");
            }
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


void Dataset::to_csv(std::string filename){
    std::wofstream out_file;
    if (!str_ends_with(filename, ".csv")){
        filename += ".csv";
    }
    out_file.open(filename);
    if (out_file.is_open()){
        std::map<std::wstring, std::vector<double>> columns;
        std::vector<std::string> filenames;
        for (auto it = idf.begin(); it != idf.end(); it++){
            columns[it->first] = std::vector<double>();
        }
        for (auto it = documents.begin(); it != documents.end(); it++){
            filenames.push_back(it->filename);
            for(auto m_it = columns.begin(); m_it != columns.end(); m_it++){
                if (it->tf_idf.count(m_it->first)){
                    m_it->second.push_back(it->tf_idf[m_it->first]);
                }
                else{
                    m_it->second.push_back(0);
                }
            }
        }
        std::cout << "N-Rows: " << filenames.size() << std::endl;
        std::cout << "N-Columns: " << idf.size() << std::endl;
        out_file << L"filenames";
        for (auto it = idf.begin(); it != idf.end(); it++){
            out_file << L',' + it->first;
        }
        out_file << std::endl;
        for (int i = 0; i < filenames.size(); i++){
            std::cout << "Outputting row: " << i << std::endl;
            std::wstring w_filename;
            w_filename.assign(filenames[i].begin(), filenames[i].end());
            out_file << w_filename;
            for (auto it = columns.begin(); it != columns.end(); it++){
                out_file << L',' << it->second[i];
            }
            out_file << std::endl;
        }
        out_file.close();
    }
    else{
        throw dataset_exception("Could not open csv file");
    }
}


int main(){

    Dataset d("out.csv", CSV);
    Document doc = d.documents[5];
    for (auto it = d.documents.begin(); it != d.documents.end(); it++){
        std::cout << doc.compute_distance(*it) << std::endl;
    }
}