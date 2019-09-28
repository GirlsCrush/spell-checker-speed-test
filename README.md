# Spell Checker

Implementations of several variants of SpellChecker class (Using different ways of storing information). THe best perfomance is provided by Trie version.

Provided functionality:
 * ability to select underlined container: std::vector, std::set, std::unordered_set, your own hash table and trie implementations. 
 Selection is based on ContainerType value (constructor parameter)
 * Your fastest implementations should correspond to ContainerType::Fastest enum member. 
 It can be one of listed above variants, or you can implement something different.
 * implement string_hash function for your hash table
 * load contents of dictionary to memory
 * detect if word is valid (i.e. determine if word should be skipped during spell checking or checked against dictionary)
 * check if word exist in dictionary
 * add word to dictionary-in-memory (that we do not want to be reported as misspelled, e.g. google, variadic)

Notes:
 * dictionary is assumed to be a file containing a list of lowercase words, one per line;
 * each word will contain only lowercase alphabetical characters and possibly apostrophes;
 * from top to bottom, the dictionary file is sorted lexicographically, with only one word per line (each of which ends with \n);
 * no word is longer than 45 characters;
 * no word appears more than once
 * Due to server load different runs of tests can show different timings for the same code, so feel free to re-run tests if the last run was unexpectedly slow.

Word is considered invalid if:
 * exceeds maximum length for a word
 * has non alpha characters or begins with apostrophe

The list of files which are allowed to be modified:
* project/spell_check.cpp

