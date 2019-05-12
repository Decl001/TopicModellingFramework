/**
 * Author: Declan Atkins
 * Last Modified: 12/05/19
 * 
 * Header file for the dataset class
 * 
 * Also contains functions for cross platform file loading
 */

#ifndef DATASET_H
#define DATASET_H

#include <vector>

struct dataset_exception : std::runtime_error{
    dataset_exception(const std::string& message) : std::runtime_error(message){}
};


#if __cplusplus > 201402L

    #include <filesystem>

    std::vector<std::string> CPP2017_list_directory(std::string directory_name){
        std::vector<std::string> files;
        for (auto& file_or_dir : fs::recursive_directory_iterator(directory_name)){
            if (file_or_dir.is_regular_file()){
                files.push_back(file_or_dir.path());
            }
        }

        return files;
    }

    #define list_directory CPP2017_list_directory


#elif defined(_WIN32)

    #include <Windows.h>
    #include <iostream>
    #include <string>

    std::string join_path(std::string root, std::string sub_path){
        std::string path;
        if (root.back() == '/' || root.back() == '\\'){
            path = root + sub_path;
        }
        else{
            path = root + '\\' + sub_path;
        }
        return path;
    }

    std::vector<std::string> WINDOWS_list_directory(std::string directory_name){
        
        std::vector<std::string> files;
        WIN32_FIND_DATA find_data;
        HANDLE find_handle = INVALID_HANDLE_VALUE;
        DWORD dw_error = 0;

        std::string directory_name_mod = directory_name + "\\*";
        LPCSTR dir_name = directory_name_mod.c_str();
        find_handle = FindFirstFile(dir_name, &find_data);
        
        if (find_handle == INVALID_HANDLE_VALUE){
            throw dataset_exception("Could not open directory");
        }
        do{
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (strcmp((char*) find_data.cFileName, ".") != 0 \
                                && strcmp((char*) find_data.cFileName, "..")){
                    std::string sub_dir = join_path(directory_name, (char*) find_data.cFileName);
                    std::vector<std::string> sub_files = WINDOWS_list_directory(sub_dir);
                    files.reserve(files.size() + distance(sub_files.begin(), sub_files.end()));
                    files.insert(files.end(), sub_files.begin(), sub_files.end());
                }
            }
            else
            {
                std::string the_file = join_path(directory_name, (char*) find_data.cFileName);
                files.push_back(the_file);
            }
        }
        while (FindNextFile(find_handle, &find_data) != 0); 
        dw_error = GetLastError();
        if (dw_error != ERROR_NO_MORE_FILES) 
        {
            throw dataset_exception("Error listing files in directory");
        }
        
        delete[] dir_name;
        return files;
    }

    #define list_directory WINDOWS_list_directory


#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))

    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>

    std::vector<std::string> list_dir(const std::string dir){
        
        DIR *dp;
        struct dirent *dirp;
        std::vector<std::string> files;

        if ((dp = opendir(dir.c_str())) == NULL){
            std::cout << "Error opening dir." << std::endl;
        }

        while ((dirp = readdir(dp)) != NULL){
            std::string entry = dirp->d_name;
            if ( entry == "." or entry == ".." )
            {
                continue;
            }

            files.push_back(entry);
        }

        closedir(dp);
        return files;
    }
    
    std::vector<std::string> UNIX_list_directory(std::string directory_name){
        std::vector<std::string> files;
        std::vector<std::string> files_or_dirs = list_dir(directory_name);
        struct stat sb;
        

        for (auto it = files_or_dirs.begin(); it != files_or_dirs.end() ; it++){
            std::string entry = *it;
            std::string full_path = directory_name + "/" + entry;

            if (lstat(full_path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)){
                std::vector<std::string> sub_files = UNIX_list_directory(full_path);
                files.reserve(files.size() + distance(sub_files.begin(), sub_files.end()));
                files.insert(files.end(), sub_files.begin(), sub_files.end());

            } 
            else {
                files.push_back(full_path);
            }
        }

        return files;
    }

    #define list_directory UNIX_list_directory

#endif // OS CHECK

#endif // DATASET_H