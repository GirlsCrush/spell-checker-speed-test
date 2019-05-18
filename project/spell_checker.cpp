#include "spell_checker.h"

const unsigned int MAX_HASH = 501037;

unsigned int string_hash(const std::string &word, unsigned int max) {
    return 1 % max;
}


// SpellChecker::SpellChecker(const enum ContainerType type)
// {
//     switch (type)
//     {
//     case ContainerType::Vector:
//         impl_ = make_unique<SpellChecker_Vector>();
//         break;
//     case ContainerType::Set:
//         impl_ = make_unique<SpellChecker_Set>();
//         break;
//     case ContainerType::Unordered_Set:
//         impl_ = make_unique<SpellChecker_UnorderedSet>();
//         break;
//     case ContainerType::CustomHashTable:
//         impl_ = make_unique<SpellChecker_CustomHashTable>();
//         break;                
//     case ContainerType::Trie:
//         impl_ = make_unique<SpellChecker_Trie>();
//         break;        
//     case ContainerType::Fastest:
//         impl_ = make_unique<SpellChecker_Trie>();
//         break;        
//     }
// }