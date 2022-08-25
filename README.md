# Find-and-Replace-Words-in-Text
Trie-based algorithm with all features of regex search

## Features
1. Prefix search and multi-word search queries are supported in the trie.
2. Replace feature provided for prefixes and the remaining trie after the replaced words is merged with the trie of
the new word.
3. Replace feature also works for multiple words and the trie accounts for the changes in indices of words without updating the entire trie again.
4. Can be integrated with a change-tracking tool which returns vector of words with their indices in latest traversal. Upon calling the update function, 
changes are detected and accordingly trie is updated to account for manual changes made without using in-built trie functions and then be deployed in a 
formal setting.
5. To improve efficiency, indices are stored in trie only at time of insertion of word. Change in indices of words is calculated instead.

## How It Works
1. Indices of words at time of insertion stored for every character of the word. The last character of a word also stores the index separately to denote
it is the end of a word.
2. Two vectors of tuples, insertions and deletions, store the index as well as the word that was inserted or deleted, respectively. The tuple also
contains a timestamp which keeps track of the relative time of the operations performed.
3. A vector stores the words with their indices of the file that is currently open. After every few seconds, the file can be traversed and the new
vector containing the words and indices can be compared with the stored vector to detect manual changes to the file.

## The Functions
0. void init_build(vector<tuple<string,int>>): Builds initial trie and requires traversal over each file. It takes as argument a vector of words and their
indices in a file and repeats the process for all files. It detects the current file using the file set using 'setCurrentFile' function.
1. void setSearchType(int): Can be called to make future searches and word replacements local(limited to current file), or global(spanning
all files). 0 as an argument denotes global operations, and 1 denotes local operations.
2. void setCurrentFile(string): Takes the current file name as the argument and can be called to update which file is being worked on.
3. void updateTrie(vector<tuple<string, int>>): Can be called to detect changes in the text and update the trie accordingly. It takes as 
input a vector of tuples of words in the file with their indices. It uses 2 functions, trackChangesDel, and makeChanges to accomplish this task.
4. void insertTrie: Overloaded function to deal with the various kinds of insertions required to support the features offered.
5. vector<tuple<string,int,int>> findWord(string): Returns instances of the string including prefixes as a vector of tuples.
6. vector<tuple<string,int,int>> findWord_entire(string): Returns instances of entire words as a vector of tuples.
7. vector<tuple<string,int,int>> new_index(vector<tuple<string,int,int>>): Takes a vector of tuples that is stored in the trie and since the indices 
stored are those at the time of the insertion of the words, it calculates the current indices by iterating over the insertion and deletion vectors. It
is used in the implementation of findWord functions.
8. void remove_one/remove/remove_entire: Deal with various kinds of removal of words in trie required to support the features offered.
9. void replace(string search, string replace): Takes as input the string to be replaced and the new string. It deals with various cases of search and replace queries 
such as different number of words in each, multi-word search and replace, single word search and replace.
10. vector<vector<tuple<string,int,int>>> multi_index(vector<string>): Takes as argument the vector of words in search query and returns instances where
the words in search query are found in the text consecutively. Used to deal with multi-word search queries.
11. void replace_single(string search, string replace): Replace function for prefix search and replace queries. It updates the trie by deleting
the original prefix and inserting the new prefix, but also merges the remaining trie of the replaced prefix with that of the new prefix. This is 
implemented using the 'merge' function.
12. void replace_single_entire(string search, string replace): Replace function for search and replace queries of entire single words. Faster than
'replace' function which can also handle multi-word replace cases.
13. Other functions are present to help with implementation of functions mentioned above, but are not included
