#include "spell_checker.h"
#include <vector>
#include <set>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <functional>
#include <thread>

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
	SpellChecker_Vector() : dict(27) {}
	void load(const std::string &dictionary) {
		std::ifstream file(dictionary);
		if (file.fail())
			throw SpellChecker_InvalidDictFile();
		std::string tmp;
		while (file >> tmp)
			dict[getIndex(tmp[0])].push_back(tmp);
	}
	bool check(const std::string &word) const {
		std::string wordLower(word);
		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
		const std::vector<std::string> &v = dict[getIndex(wordLower[0])];
		for (int i = 0; i < v.size(); ++i)
			if (v[i] == wordLower)
				return true;
		return false;
	}
	void add(const std::string &word) {
		std::string wordLower(word);
		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
		if (!check(wordLower))
			dict[getIndex(wordLower[0])].push_back(wordLower);
	}
	size_t size(void) const { 
		size_t res = 0;
		for (int i = 0; i < dict.size(); ++i)
			res += dict[i].size();
		return res; }
private:
	std::vector<std::vector<std::string>> dict;
};

class SpellChecker_Set : public SpellChecker_Impl
{
public:
	void load(const std::string &dictionary) {
		std::ifstream file(dictionary);
		if (file.fail())
			throw SpellChecker_InvalidDictFile();
		std::string tmp;
		while (file >> tmp)
			dict.insert(tmp);
	}
	bool check(const std::string &word) const {
		std::string wordLower(word);
		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
		return dict.find(wordLower) != dict.end();
	}
	void add(const std::string &word) {
		std::string wordLower(word);
		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
		dict.insert(wordLower);
	}
	size_t size(void) const { return dict.size(); }
private:
	std::set<std::string> dict;
};

class SpellChecker_UnorderedSet : public SpellChecker_Impl
{
public:
	void load(const std::string &dictionary) {
		std::ifstream file(dictionary);
		if (file.fail())
			throw SpellChecker_InvalidDictFile();
		std::string tmp;
		while (file >> tmp)
			dict.insert(tmp);
	}
	bool check(const std::string &word) const {
		std::string wordLower(word);
		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
		return dict.find(wordLower) != dict.end();
	}
	void add(const std::string &word) {
		std::string wordLower(word);
		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
		dict.insert(wordLower);
	}
	size_t size(void) const { return dict.size(); }
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
		if (ptr) {
			while (ptr->next())
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
	size_t size(void) const { return _size; }
};

class TrieNode {
public:
	TrieNode() = default;
	bool end = false;
	TrieNode* next[123] = { nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	// ~TrieNode() {
	// 	for (int i = 0; i < 91; ++i)
	// 		if (!next[i])
	// 			delete next[i];
	// }
};


class SpellChecker_Trie : public SpellChecker_Impl
{
private:
	inline void push(const std::string &word) {
		TrieNode * tmp = root;
		std::string::const_iterator it = word.begin();
		for (; it != word.end(); ++it) {
				if (!tmp->next[*it]) {
					break;
				}
				tmp = tmp->next[*it];
		}
		
		if (it == word.end()) {
			if (!tmp->end) {
				tmp->end = true;
				++_size;
			}
			return;
		}

		do {
			tmp = tmp->next[(*it) & ~0x20]  = tmp->next[*it] = (new TrieNode);
		} while (++it != word.end());

		tmp->end = true;
		++_size;
	}
public:
	inline void load(const std::string &dictionary) {
		std::ifstream file(dictionary);
		if (file.fail())
			throw SpellChecker_InvalidDictFile();

		std::string word;
		std::string::const_iterator it;
		TrieNode * tmp;
		while (file >> word) {	//push(tmp);
			
			for (tmp = root, it = word.begin(); it != word.end(); ++it) {
				if (!tmp->next[*it]) {
					break;
				}
				tmp = tmp->next[*it];
			}

			// Not needed here as words in dictionary are sorted

			// if (it == word.end() && !tmp->end) {
			// 	tmp->end = true;
			// 	++_size;
			// 	continue;
			// }
			// else {
			// 	continue;
			// }

			do {
				tmp = tmp->next[(*it) & ~0x20] = tmp->next[*it] = (new TrieNode);	// "& ~0x20" converts a char to upper case 
			} while (++it != word.end());
			tmp->end = true;
			++_size;
		}
	}
	inline bool check(const std::string &word) const {
		TrieNode * tmp = root;
		for (int i = 0; i < word.size(); ++i) {
			if (!(tmp = tmp->next[word[i]])) {
				return false;
			}
		}
		return tmp->end;
	}
	
	inline void add(const std::string &word) {
		std::string wordLower(word);
		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
		push(wordLower);
	}
	inline size_t size(void) const { return _size; }
	SpellChecker_Trie() : root(new TrieNode()) {}
	//~SpellChecker_Trie() { delete root; }
private:
	TrieNode * root;
	size_t _size = 0;
};


// class TrieNode {
// public:
// 	TrieNode() = default;
// 	bool end = false;
// 	std::unordered_map<char, TrieNode *> next;
// };

// class SpellChecker_Trie : public SpellChecker_Impl
// {
// private:
// 	inline void push(const std::string &word) {
// 		TrieNode * tmp = root;
// 		std::string::const_iterator it = word.begin();
// 		unsigned short index;
// 		for (; it != word.end(); ++it) {
// 			index = getIndex(*it);
// 			if (!tmp->next[index]) {
// 				break;
// 			}
// 			tmp = tmp->next[index];
// 		}
// 		// if (it == word.end()) {
// 		//     ++_size;
// 		//     return;
// 		// }
// 		if (it != word.end())
// 			tmp = tmp->next[getIndex(*it)] = (new TrieNode);
// 		else if (!tmp->end) {
// 			tmp->end = true;
// 			++_size;
// 			return;
// 		}
// 		else {
// 			return;
// 		}

// 		while (++it != word.end()) {
// 			tmp = tmp->next.insert(*it);
// 		}
// 		tmp->end = true;
// 		++_size;
// 	}
// public:
// 	inline void load(const std::string &dictionary) {
// 		std::ifstream file(dictionary);
// 		if (file.fail())
// 			throw SpellChecker_InvalidDictFile();

// 		std::string word;
// 		std::string::const_iterator it;
// 		unsigned short index;
// 		TrieNode * tmp;
// 		while (file >> word) {	//push(tmp);
// 			tmp = root;
// 			it = word.begin();
// 			for (; it != word.end(); ++it) {
// 				index = getIndex(*it);
// 				if (!tmp->next[index]) {
// 					break;
// 				}
// 				tmp = tmp->next[index];
// 			}
// 			// if (it == word.end()) {
// 			//     ++_size;
// 			//     return;
// 			// }
// 			if (it != word.end())
// 				tmp = tmp->next[getIndex(*it)] = (new TrieNode);
// 			else if (!tmp->end) {
// 				tmp->end = true;
// 				++_size;
// 				continue;
// 			}
// 			else {
// 				continue;
// 			}

// 			while (++it != word.end()) {
// 				tmp = tmp->next[getIndex(*it)] = (new TrieNode);
// 			}
// 			tmp->end = true;
// 			++_size;
// 		}
// 	}
// 	inline bool check(const std::string &word) const {
// 		//std::string wordLower(word);
// 		//std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
// 		// for (int i = 0; i < wordLower.size(); ++i)
// 		//     if (wordLower[i] !islower(wordLower[i]))
// 		//         wordLower[i] = tolower(wordLower[i]);
// 		// tolowerStr(wordLower);
// 		//for ( auto it = wordLower.begin(); it != wordLower.end(); ++it )
// 		//    *it |= 0x20;

// 		TrieNode * tmp = root;
// 		// for (char c : word) {
// 		// 	tmp = tmp->next[getIndex(c | 0x20)];
// 		// 	if (!tmp) {
// 		// 		return false;
// 		// 	}
// 		// }
// 		for (int i = 0; i < word.size(); ++i) {
// 			tmp = tmp->next[getIndex(word[i] | 0x20)];	// "| 0x20" converts a word to lower case 
// 			if (!tmp) {
// 				return false;
// 			}
// 		}
// 		return tmp->end;
// 	}
// 	inline void add(const std::string &word) {
// 		std::string wordLower(word);
// 		std::transform(word.begin(), word.end(), wordLower.begin(), ::tolower);
// 		push(wordLower);
// 	}
// 	inline size_t size(void) const { return _size; }
// 	SpellChecker_Trie() : root(new TrieNode()) {}
// 	~SpellChecker_Trie() { delete root; }
// private:
// 	TrieNode * root;
// 	size_t _size = 0;
// };

SpellChecker::SpellChecker(const enum ContainerType type)
{
	switch (type)
	{
	case ContainerType::Vector:
		impl_ = std::make_unique<SpellChecker_Vector>();
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
void SpellChecker::load(const std::string &dictionary) {
	impl_->load(dictionary);
}

// returns true if word is in dictionary else false
bool SpellChecker::check(const std::string &word) const {
	return impl_->check(word);
}

// adds word to dictionary in-memory
void SpellChecker::add(const std::string &word) {
	impl_->add(word);
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
size_t SpellChecker::size(void) const {
	return impl_->size();
}

// is string recognized as word to check for spelling or should be skipped
bool SpellChecker::is_valid(const std::string &word) {
	if (word.size() > 45)
		return false;
	if (!isalpha(word[0]))
		return false;
	for (int i = 1; i < word.size(); ++i)
		if (!isalpha(word[i]) && word[i] != '\'')
					return false;
	return true;
}