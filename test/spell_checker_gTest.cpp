// Copyright 2013, GlobalLogic Inc.
// All rights reserved.
//

#include "spell_checker.h"
#include "gtest/gtest.h"
#include <fstream>
#include <list>
#include <chrono>
#include <unordered_set>
#include <iomanip>

const char *large_dict_file = "../dictionaries/large";
const unsigned large_dict_words_count = 143091;
const unsigned  speed_test_iterations = 5;

const char *list_valid[] = {
    "denormalise",
    "revocation",
    "rewards",
    "roadworks",
    "absolutely",
    "obsession",
    "obsolete",
    "logic",
    "won't"};

const char *list_misspelled[] = {
    "GlobalLogic",
    "udemy",
    "kazka",
    "showtime",
};

const char *list2add[] = {
    "google",
    "variadic",
};

struct TestWords
{
    const char *pattern;
    bool expected;
};

// maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
const size_t Max_Length = 45;

TestWords tw[] = {
    {"word", true},
    {"IBSN9998", false}, // has non alpha characters
    {"$100", false},     // has special symbols
    // valid the longest word
    {"Pneumonoultramicroscopicsilicovolcanoconiosis", true},
    // exceeds max length
    {"Pneumonoultramicroscopicsilicovolcanoconiosisa", false},
    {"Beau--ootiful", false}, // has '-'
    {"e--e--evening", false},
    {"http://www.gutenberg.org/1/11/", false}, // has non alpha
    {"11.zip", false},                         // has digits
    {"1.E.3.", false},
    {"Gutenberg-tm", false},
    {"'cause", false}, // apostrophe at first position is not allowed
    {"won't", true},   // apostrophe in other than first position is fine
};

// performance check
struct TextData
{
    const char *text;
    unsigned missed;
    unsigned valid;
    unsigned total;
};

TextData data[] = {
    {"../texts/alice.txt", 73, 23203, 29459},
    {"../texts/dracula.txt", 1540, 138207, 163798},
    {"../texts/sherlock.txt", 539, 88248, 106840}    
};

TextData speedTestData[] = {
    {"../texts/holmes.txt", 10185, 970026, 1137708},
    {"../texts/tolstoy.txt", 8240, 477745, 564514},
    {"../texts/dracula.txt", 1540, 138207, 163798}
};


TEST(SpellChecker, when_empty_size_return_0)
{
    SpellChecker obj(ContainerType::Vector);
    EXPECT_EQ(obj.size(), 0);
}

void test_load(ContainerType type)
{
    SpellChecker obj(type);
    obj.load(large_dict_file);
    EXPECT_EQ(obj.size(), large_dict_words_count);
}

TEST(SpellChecker, load_Vector)
{
    test_load(ContainerType::Vector);
}

TEST(SpellChecker, load_Set)
{
    test_load(ContainerType::Set);
}

TEST(SpellChecker, load_Unordered_Set)
{
    test_load(ContainerType::Unordered_Set);
}

TEST(SpellChecker, load_CustomHashTable)
{
    test_load(ContainerType::CustomHashTable);
}

TEST(SpellChecker, load_Trie)
{
    test_load(ContainerType::Trie);
}

TEST(SpellChecker, invalid_dict_throws_exc)
{
    SpellChecker obj(ContainerType::Vector);

    EXPECT_THROW(obj.load("invalid.txt"), SpellChecker_InvalidDictFile);
}

void test_valid_and_misspelled(ContainerType type)
{
    SpellChecker obj(type);
    obj.load(large_dict_file);
    EXPECT_EQ(obj.size(), large_dict_words_count);

    for (auto i : list_valid)
    {
        EXPECT_EQ(obj.check(i), true);
    }
    for (auto i : list_misspelled)
    {
        EXPECT_EQ(obj.check(i), false);
    }
}

void test_add_and_check(ContainerType type)
{
    SpellChecker obj(type);
    EXPECT_EQ(obj.size(), 0);
    obj.load(large_dict_file);
    size_t dictSize = obj.size();
    EXPECT_EQ(dictSize, large_dict_words_count);

    for (auto i : list2add)
    {
        EXPECT_EQ(SpellChecker::is_valid(i), true);
        EXPECT_EQ(obj.check(i), false);
        obj.add(i);
        EXPECT_EQ(obj.size(), ++dictSize);
        EXPECT_EQ(obj.check(i), true);
        obj.add(i);                      // duplicates not stored in dict, second add ignored
        EXPECT_EQ(obj.size(), dictSize); // size not changed
    }
    // re-check valid and misspelled that nothing broken after add
    for (auto i : list_valid)
    {
        EXPECT_EQ(obj.check(i), true);
    }
    for (auto i : list_misspelled)
    {
        EXPECT_EQ(obj.check(i), false);
    }
}

TEST(SpellChecker, test_add_and_check_Vector)
{
    test_add_and_check(ContainerType::Vector);
}
TEST(SpellChecker, test_add_and_check_Set)
{
    test_add_and_check(ContainerType::Set);
}
TEST(SpellChecker, test_add_and_check_UnorderedSet)
{
    test_add_and_check(ContainerType::Unordered_Set);
}

TEST(SpellChecker, test_add_and_check_CustomHashTable)
{
    test_add_and_check(ContainerType::Unordered_Set);
}

TEST(SpellChecker, test_add_and_check_Trie)
{
    test_add_and_check(ContainerType::Trie);
}

TEST(SpellChecker, when_valid_word_is_valid_returns_true)
{
    for (auto i : list_valid)
    {
        EXPECT_EQ(SpellChecker::is_valid(i), true);
    }
    for (auto i : list_misspelled)
    {
        EXPECT_EQ(SpellChecker::is_valid(i), true);
    }
}

TEST(SpellChecker, is_valid)
{
    for (auto i : tw)
    {
        EXPECT_EQ(SpellChecker::is_valid(i.pattern), i.expected);
    }
}

void checkHashFunction(unsigned &collisions, unsigned &max) {
    std::ifstream infile;
    infile.open(large_dict_file);
    std::string line;
    std::list<std::string> words;
    
    while (infile >> line) {
        words.push_back(line);
    }

    max = 0;
    collisions = 0;
    std::unordered_set<unsigned> dict;
    std::unordered_set<std::string> wordDict;
    for(auto word : words) {
        unsigned h = string_hash(word, MAX_HASH);

        if(wordDict.find(word) == wordDict.end()) {
            wordDict.insert(word);
        
            if(dict.find(h) != dict.end()) {
                collisions++;
            }
            else {
                dict.insert(h);
            }
        }

        if(h > max) {
            max = h;
        }
    }    
}

TEST(string_hash, hash_function_metrics) {
    unsigned collisions;
    unsigned max;
    checkHashFunction(collisions, max);
    
    EXPECT_LT(collisions, large_dict_words_count/6);
    EXPECT_LT(max, large_dict_words_count * 4);
}

double get_reference_time() {
    std::vector<std::string> dict;

    std::ifstream dictfile;
    dictfile.open(large_dict_file);
    if (dictfile.fail()) {
        throw SpellChecker_InvalidDictFile();
    }
    std::string line;
    dictfile >> line;
    while (!dictfile.eof())
    {
        dict.push_back(line);
        dictfile >> line;
    }
    
    double timeSpent = 0;

    int i = 0;
    for (auto test : speedTestData) {
        i++;
        std::ifstream infile;
        infile.open(test.text);
        std::string line;
        std::list<std::string> words;
        
        while (infile >> line) {            
            size_t n = line.length();
            bool bad = false;
            if(n > Max_Length)
                bad = true;

            for (size_t i = 0; !bad && i < n; ++i)
            {
                if (!(isalpha(line[i]) || (line[i] == '\'' && i > 0)))
                    bad = true;
            }
        
            if(!bad) words.push_back(line);
        }

        for(int k = 0; k < speed_test_iterations; k++)
        {
            unsigned misspelled = 0;

            auto start = std::chrono::high_resolution_clock::now();
            for(auto word : words) {
                std::string copy_lower(word);
                std::transform(word.begin(), word.end(), copy_lower.begin(), ::tolower);

                if(!(std::binary_search(dict.begin(), dict.end(), copy_lower))) {
                    misspelled++;
                }
            }        
            timeSpent += (std::chrono::high_resolution_clock::now() - start).count();

            if(misspelled != test.missed) {
                printf("Misspelled for test %d mismatch: %u\n", i, misspelled);
                throw "Get reference time failed!";
            }
        }
    }

    return timeSpent;
}

double measure_performance(ContainerType type) {
    SpellChecker obj(type);
    obj.load(large_dict_file);

    double timeSpent = 0;

    for (auto test : speedTestData) {
        std::ifstream infile;
        infile.open(test.text);
        std::string line;
        std::list<std::string> words;
        
        while (infile >> line) {
            if (SpellChecker::is_valid(line)) {
                words.push_back(line);
            }            
        }

        for(int k = 0; k < speed_test_iterations; k++)
        {
            unsigned misspelled = 0;

            auto start = std::chrono::high_resolution_clock::now();
            for(auto word : words) {
                if(!obj.check(word)){
                    misspelled++;
                }            
            }        
            timeSpent += (std::chrono::high_resolution_clock::now() - start).count();

            if(misspelled != test.missed) {
                throw "measure_performance failed!";
            }
        }
    }

    return timeSpent;
}

double referenceTime = get_reference_time();

void test_relative_speed(ContainerType type, double factor) {
    auto time = measure_performance(type);
    EXPECT_LT(time, referenceTime*factor);
    std::cout << "You time is " << (int)(time/referenceTime * 100) << "% of reference time" << std::endl;
}

void test_performance(ContainerType type)
{
    SpellChecker obj(type);
    obj.load(large_dict_file);
    ASSERT_EQ(obj.size(), large_dict_words_count);

    // allow only alphabetical characters and apostrophes not at beginning
    // ignore words with numbers (like MS Word can)
    // ignore alphabetical strings too long to be words
    for (auto test : data)
    {
        std::ifstream infile;
        infile.open(test.text);
        std::string line;
        unsigned misspelled = 0;
        unsigned words_in_text = 0;
        unsigned total = 0;
        ASSERT_EQ(infile.good(), true);
        while (infile >> line)
        {
            total++;
            if (!SpellChecker::is_valid(line))
            {
                continue;
            }
            words_in_text++;
            if (!obj.check(line))
            {
                misspelled++;
            }
        }
        EXPECT_EQ(misspelled, test.missed);
        EXPECT_EQ(words_in_text, test.valid);
        EXPECT_EQ(total, test.total);
    }
}

TEST(SpellChecker, performance_check_vector)
{
    test_performance(ContainerType::Vector);
}
TEST(SpellChecker, performance_check_set)
{
    test_performance(ContainerType::Set);
}
TEST(SpellChecker, performance_check_unordered_set)
{
    test_performance(ContainerType::Unordered_Set);
}

TEST(SpellChecker, performance_check_custom_hash_table)
{
    test_performance(ContainerType::CustomHashTable);
}

TEST(SpellChecker, performance_check_trie)
{
    test_performance(ContainerType::Trie);
}

TEST(SpellChecker, check_speed_acceptable)
{
    auto time = measure_performance(ContainerType::Fastest);
    
    double grades[] = {2.0, 1.0, 0.9}; //, 0.75, 0.5, 0.2, 0.1, 0.05};

    for(double q : grades) {
        EXPECT_LT(time, referenceTime*q) << 
        std::endl << std::setprecision(2) << "Failed to reach " << q << " of reference time limit!" << std::endl <<
        "You time is " << (int)(time/referenceTime * 100) << "% of reference time" << std::endl;
    }
}

int main(int argc, char **argv)
{
    printf("Running main() from Coder_gTest.cpp\n");

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
