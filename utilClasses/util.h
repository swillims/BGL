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
    static void removeComments(std::string& s, const std::string& delimiter)
    {
        size_t pos = s.find(delimiter);
        if (pos != std::string::npos)
        {
            s.erase(pos);   // removes delimiter AND everything after
        }
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

    static std::string translateToHumanReadable(const std::string& original)
    {
        std::string returnString;

        for (char c : original)
        {
            if (c >= 32 && c <= 126)
            {
                returnString += c;
            }
            else if (c == 9)
            {
                returnString += "[TAB]";
            }
            else if (c == 10)
            {
                returnString += "[NEW_LINE]";
            }
            else if (c == 13)
            {
                returnString += "[WINR]";
            }
            else
            {
                returnString += "[ missing :( ]";
            }
        }
        return returnString;
    }

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
            return aB.at(a);
        }
        const A& operator[](const B& b) const 
        {
            return bA.at(b);
        }
        void insert(const A& a, const B& b)
        {
            aB[a] = b;
            bA[b] = a;
        }

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

