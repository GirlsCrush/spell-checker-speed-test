#include "spell_checker.h"
#include <vector>
#include <set>
#include <memory>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <functional>

const unsigned int MAX_HASH = 501037;

unsigned int string_hash(const std::string &word, unsigned int max) {
    std::hash<std::string> hash_str;
	return hash_str(word) % MAX_HASH;
}

inline int getIndex(char c) {
    return (c - 'a' >= 0 ? c - 'a' : 26);
}
class SpellChecker_Vector : public SpellChecker_Impl
{
public:
    void load(const std::string &dictionary){ 
        std::ifstream file(dictionary); 
        if (file.fail())
            throw SpellChecker_InvalidDictFile();
        std::string tmp;
        while (file >> tmp)
            dict.push_back(tmp);
        }
    bool check(const std::string &word) const{ 
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        for (std::string s : dict)
            if (s == wordLower)
                return true;
        return false; 
    }
    void add(const std::string &word){
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        if (!check(wordLower))
            dict.push_back(wordLower);
    }
    size_t size(void) const{ return dict.size(); }
private:
    std::vector<std::string> dict;
};

class SpellChecker_Set : public SpellChecker_Impl
{
public:
    void load(const std::string &dictionary){ 
        std::ifstream file(dictionary); 
        if (file.fail())
            throw SpellChecker_InvalidDictFile();
        std::string tmp;
        while (file >> tmp)
            dict.insert(tmp);
        }
    bool check(const std::string &word) const{ 
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        return dict.find(wordLower) != dict.end(); 
        }
    void add(const std::string &word){
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        dict.insert(wordLower);
    }
    size_t size(void) const{ return dict.size(); }
private:
    std::set<std::string> dict;
};

class SpellChecker_UnorderedSet : public SpellChecker_Impl
{
public:
    void load(const std::string &dictionary){ 
        std::ifstream file(dictionary); 
        if (file.fail())
            throw SpellChecker_InvalidDictFile();
        std::string tmp;
        while (file >> tmp)
            dict.insert(tmp);
        }
    bool check(const std::string &word) const{ 
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        return dict.find(wordLower) != dict.end(); 
        }
    void add(const std::string &word){
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        dict.insert(wordLower);
    }
    size_t size(void) const{ return dict.size(); }
private:
    std::unordered_set<std::string> dict;
};

class HashNode {
public:
    HashNode() = default;
    HashNode(const std::string &str) : _str(str) {}
    std::shared_ptr<HashNode> next() {
        return _next;
    }
    void setNext(const std::string &str) {
        _next = std::shared_ptr<HashNode>(new HashNode(str));
    }
    void setString(const std::string &str) {
        _str = str;
    }
    bool isEmpty() {
        return _str != "";
    }
    std::string str() {
        return _str;
    }
    bool compare(const std::string &str) {
        return _str == str;
    }
private:
    std::string _str;
    std::shared_ptr<HashNode> _next;
};

class SpellChecker_CustomHashTable : public SpellChecker_Impl
{
private:
    std::vector<std::shared_ptr<HashNode>> dict;
    size_t _size = 0;
public:
    SpellChecker_CustomHashTable() : dict(MAX_HASH) {}
    
    void push(const std::string & str) {
        unsigned code = string_hash(str, MAX_HASH);
        std::shared_ptr<HashNode> ptr = dict[code];
        if (ptr){
            while(ptr->next())
                ptr = ptr->next();
            ptr->setNext(str);
        }
        else {
            dict[code] = std::shared_ptr<HashNode>(new HashNode(str));
        }
        ++_size;
    }
    
    void load(const std::string &dictionary) {
        std::ifstream file(dictionary); 
        if (file.fail())
            throw SpellChecker_InvalidDictFile();
        std::string tmp;
        while (file >> tmp)
            push(tmp);
    }

    bool check(const std::string &word) const {
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        std::shared_ptr<HashNode> ptr = dict[string_hash(wordLower, MAX_HASH)];
        while (ptr) {
            if (ptr->compare(wordLower))
                return true;
            ptr = ptr->next();
        }
        return false;
    }
    void add(const std::string &word) {
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        push(wordLower);
    }
    size_t size(void) const{ return _size; }
};  

class TrieNode {
public:
    TrieNode() = default;
	bool end = false;
	TrieNode * next[27] = {nullptr, nullptr, nullptr, nullptr, nullptr, 
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    // ~TrieNode() {
    //     for (int i = 0; i < 27; ++i)
    //         delete next[i];
    // }
};

inline void tolowerPtr(char *p) 
{
    switch(*p)
    {
    case 'A':*p='a'; return;
    case 'B':*p='b'; return;
    case 'C':*p='c'; return;
    case 'D':*p='d'; return;
    case 'E':*p='e'; return;
    case 'F':*p='f'; return;
    case 'G':*p='g'; return;
    case 'H':*p='h'; return;
    case 'I':*p='i'; return;
    case 'J':*p='j'; return;
    case 'K':*p='k'; return;
    case 'L':*p='l'; return;
    case 'M':*p='m'; return;
    case 'N':*p='n'; return;
    case 'O':*p='o'; return;
    case 'P':*p='p'; return;
    case 'Q':*p='q'; return;
    case 'R':*p='r'; return;
    case 'S':*p='s'; return;
    case 'T':*p='t'; return;
    case 'U':*p='u'; return;
    case 'V':*p='v'; return;
    case 'W':*p='w'; return;
    case 'X':*p='x'; return;
    case 'Y':*p='y'; return;
    case 'Z':*p='z'; return;
    };
}

inline void tolowerStr(std::string& s)
{
    char* c=const_cast<char*>(s.c_str());
size_t l = s.size();
  for(char* c2=c;c2<c+l;c2++)tolowerPtr(c2); 
};

class SpellChecker_Trie : public SpellChecker_Impl
{
private:
    inline void push(const std::string &word) {
        TrieNode * tmp = root;
        std::string::const_iterator it;
        for (it = word.begin(); it != word.end(); ++it) {
			unsigned short index = getIndex(*it);
            if (!tmp->next[index]){
                break;
            }
			tmp = tmp->next[index];
        }
        // if (it == word.end()) {
        //     ++_size;
        //     return;
        // }
		if (it != word.end())
			tmp = tmp->next[getIndex(*it)] = (new TrieNode);
        else if (!tmp->end) {
            tmp->end = true;
            ++_size;
            return;
        } 
        else {
            return;
        }

        while (++it != word.end()) {
            tmp = tmp->next[getIndex(*it)] = (new TrieNode);
        }
		tmp->end = true;
        ++_size;
    }
public:
    void load(const std::string &dictionary){
        std::ifstream file(dictionary); 
        if (file.fail())
            throw SpellChecker_InvalidDictFile();
        std::string tmp;
        while (file >> tmp)
            push(tmp);
    }
    bool check(const std::string &word) const {
        std::string wordLower(word);
        //std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
        // for (int i = 0; i < wordLower.size(); ++i)
        //     if (wordLower[i] !islower(wordLower[i]))
        //         wordLower[i] = tolower(wordLower[i]);
        // tolowerStr(wordLower);
        //for ( auto it = wordLower.begin(); it != wordLower.end(); ++it )
        //    *it |= 0x20;
        
        TrieNode * tmp = root;
        for (char c : word) {
            tmp = tmp->next[getIndex(c | 0x20)];
            if (!tmp){
                return false;
            }
        }
        return tmp->end;
    }
    void add(const std::string &word){ 
        std::string wordLower(word);
        std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
            push(wordLower); 
        }
    size_t size(void) const{ return _size; }
    SpellChecker_Trie() : root(new TrieNode()) {}
    //~SpellChecker_Trie() { delete root; }
private:
    TrieNode * root;
    size_t _size = 0;
};

SpellChecker::SpellChecker(const enum ContainerType type)
{
    switch (type)
    {
    case ContainerType::Vector:
        impl_ = std::make_unique<SpellChecker_Set>();
        break;
    case ContainerType::Set:
        impl_ = std::make_unique<SpellChecker_Set>();
        break;
    case ContainerType::Unordered_Set:
        impl_ = std::make_unique<SpellChecker_UnorderedSet>();
        break;
    case ContainerType::CustomHashTable:
        impl_ = std::make_unique<SpellChecker_CustomHashTable>();
        break;                
    case ContainerType::Trie:
        impl_ = std::make_unique<SpellChecker_Trie>();
        break;        
    case ContainerType::Fastest:
        impl_ = std::make_unique<SpellChecker_Trie>();
        break;        
    }
}

// Loads dictionary into memory. Throws exception if any issues
void SpellChecker::load(const std::string &dictionary){
    impl_->load(dictionary);
}

// returns true if word is in dictionary else false
bool SpellChecker::check(const std::string &word) const{
    return impl_->check(word);
}

// adds word to dictionary in-memory
void SpellChecker::add(const std::string &word){
    impl_->add(word);
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
size_t SpellChecker::size(void) const{
    return impl_->size();
}

// is string recognized as word to check for spelling or should be skipped
bool SpellChecker::is_valid(const std::string &word){
    if (word.size() > 45 || word.size() == 0) 
        return false;
    if (!isalpha(word.front()))
        return false;
    for (int i = 1; i < word.size(); ++i)
        if (!isalpha(word[i]) && word[i] != '\'')
            return false;
    return true;
}