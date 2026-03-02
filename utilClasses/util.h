#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

struct util
{
    // file util
    static std::string cleanFileName(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (pos == std::string::npos) ? path : path.substr(pos + 1);
    }
    static std::string cleanFilePath(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (pos == std::string::npos) ? "" : path.substr(0, pos + 1);
    }
    static bool fileExists(const std::string& filePath)
    {
        std::ifstream file(filePath);
        return file.good();
    }
    static std::string readFile(const std::string& filePath) 
    {
        std::ifstream file(filePath);
        std::ostringstream ss;
        ss << file.rdbuf();  // read the whole file buffer into stream
        return ss.str();
    }
    static bool writeFile(const std::string& filePath, const std::string& contents, bool additive = false)
    {
        // stuff to make folder exists if they don't
        std::filesystem::path p(filePath);
        std::filesystem::create_directories(p.parent_path());

        // actual code
        std::ofstream file;
        if (additive) { file.open(filePath, std::ios::out | std::ios::app); }
        else { file.open(filePath, std::ios::out | std::ios::trunc); }
        if (!file.is_open()) { return false; }
        file << contents;
        file.close();
        return true;
    }

    // string manipulation
    static std::vector<std::string> split(const std::string& s, const std::string& delimiter, bool convertToUnix = true) // always keep "convertToUnix = true" unless there is very a good reason to not convert
    {
        std::string working = s;

        if (convertToUnix) {
            // Replace all "\r\n" with "\n"
            size_t pos = 0;
            while ((pos = working.find("\r\n", pos)) != std::string::npos) {
                working.replace(pos, 2, "\n");
                pos += 1; // move past the \n
            }
        }

        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end;

        while ((end = working.find(delimiter, start)) != std::string::npos) {
            tokens.push_back(working.substr(start, end - start));
            start = end + delimiter.length();
        }

        tokens.push_back(working.substr(start)); // last piece
        return tokens;
    }
    static void sanitizeString(std::string& s, const std::initializer_list<std::string>& delimiters)
    {
        for (const std::string& k : delimiters)
        {
            size_t pos = 0;
            while ((pos = s.find(k, pos)) != std::string::npos)
            {
                s.erase(pos, k.size());
            }
        }
    }
    static void removeComments(std::string& s, const std::string& delimiter)
    {
        size_t pos = s.find(delimiter);
        if (pos != std::string::npos)
        {
            s.erase(pos);   // removes delimiter AND everything after
        }
    }
    // I don't think this is used anymore
    // it worked for an older version of text gen
    /*
    static std::vector<std::string> splitWrap(const std::string& s, unsigned int length)
    {
        std::vector<std::string> r;
        std::vector<std::string> ss = split(s, "\n");
        std::reverse(ss.begin(), ss.end());
        while (!ss.empty())
        {
            std::string sss = ss.back(); ss.pop_back(); // 2 statement in one line because every other languages has pop_back as not void
            if (!sss.empty() && sss[0] == ' '){sss = sss.substr(1);}
            if(sss.size() <= length){r.push_back(sss);}
            else
            {
                std::string pre = sss.substr(0, length);
                int n = pre.find_last_of(" ");
                std::string sr;
                if (n == std::string::npos) {
                    n = length;
                    sr = sss.substr(0, n);
                    ss.push_back(sss.substr(n));
                }
                else {
                    sr = sss.substr(0, n);
                    std::string rem = sss.substr(n + 1);
                    rem.erase(0, rem.find_first_not_of(" "));
                    if (!rem.empty()) ss.push_back(rem);
                }
                if (!sr.empty()) {r.push_back(sr);}
            }
        }
        return r;
    }
    //*/

    // array helpers
    static int get1DIndex(unsigned int size1, unsigned int size2, unsigned int index1, unsigned int index2) // these are simple enough to where you should not use them
    {
        return index2 * size1 + index1;
    }
    static int get1DIndexWrap(unsigned int size1, unsigned int size2, unsigned int index1, unsigned int index2) // these are simple enough to where you should not use them
    {
        index1 = index1 % size1;
        index2 = index2 % size2;
        return index2 * size1 + index1;
    }
    // generics
    template<typename T>
    static constexpr T compareMin(T a, T b){return (a < b) ? a : b;}
    template<typename T>
    static constexpr T compareMax(T a, T b){return (a < b) ? b : a;}

    // structs/classes
    template<class A, class B>
    struct BiMap {
        std::unordered_map<A, B> aB;
        std::unordered_map<B, A> bA;
        const B& operator[](const A& a) const 
        {
            //auto it = aB.find(a);
            //if (it == aB.end()) throw std::runtime_error("Key not found in BiMap");
            //return it->second;
            return aB.at(a);
        }
        const A& operator[](const B& b) const 
        {
            //auto it = bA.find(b);
            //if (it == bA.end()) throw std::runtime_error("Key not found in BiMap");
            //return it->second;
            return bA.at(b);
        }
        void insert(const A& a, const B& b)
        {
            //if (aB.contains(a) || bA.contains(b)) { return }
            aB[a] = b;
            bA[b] = a;
        }
        /*
        void insert(const B& b, const A& a)
        {
            //if (aB.contains(a) || bA.contains(b)) { return }
            aB[a] = b;
            bA[b] = a;
        }
        //*/
        void erase(const A& a)
        {
            
            auto it = aB.find(a);
            if (it == aB.end()) return;
            const B& b = it->second;
            bA.erase(b);
            aB.erase(it);
        }
        void erase(const B& b)
        {
            auto it = bA.find(b);
            if (it == bA.end()) return;
            const A& a = it->second;
            aB.erase(a);
            bA.erase(it);
        }
        bool contains(const A& a) const
        {
            return aB.contains(a);
        }
        bool contains(const B& b) const
        {
            return bA.contains(b);
        }
    };
};

