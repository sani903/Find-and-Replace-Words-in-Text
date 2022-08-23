#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstring>  
#include <fstream>
#include <cctype>
#include <set>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;

class TrieNode{
    public:
        // used to map a node of the trie to its children 
        unordered_map<char, TrieNode *> children;
        // stores the indices at which the prefix appears in text
        unordered_map<char, vector<tuple<string, int, int>>> indices;
        // stores the indices at which the prefix covered in trie is a word
        unordered_map<char, vector<tuple<string, int, int>>> EOW_indices;
};

class Trie{
    public:
        // root node of Trie
        TrieNode *root;
        // stores the (timestamp, file number, index) of insertions
        unordered_map<string, vector<tuple<string, int, int,string>>> insertions;
        // stores the (timestamp, file number, index) of deletions
        unordered_map<string, vector<tuple<string, int, int,string>>> deletions;
        // stores the text in the last update
        vector<tuple<string, int>> before;
        int local = 0;
        int lookahead = 15;
        int checkahead = 15;
        int confirm = 3;
        string currentFile;
        // used to keep track of order of operations
        int timestamp = 0;
        Trie(){
            root = new TrieNode();
        }
        void init_build(vector<tuple<string,int>> after){
            for(int i = 0;i<after.size();i++){
                    string word = get<0>(after[i]);
                    int pos = get<1>(after[i]);
                    if (word == ""){
                        continue;
                    }
                    insertTrie(word, make_tuple(currentFile,pos,timestamp));
                }
        }
        void setSearchType(int check){
            local = check;
        }
        void setCurrentFile(string file){
            before.clear();
            currentFile = file;
        }
        tuple<int,int,int> trackChangesDel(vector<tuple<string, int>> after, int it1, int it2,int net_change){
            int check = 0;
            for(int i = 0;i<checkahead;i++){
                for(int k = 1;k<=lookahead;k++){
                    if((it1+k)>before.size()){
                        return tuple<int,int,int>();
                    }
                    if((it2+i)>before.size()){
                        return tuple<int,int,int>();
                    }
                    string b_word = get<0>(before[it1+k]);
                    string a_word = get<0>(after[it2+i]);
                    if(a_word==b_word){
                        for(int j = 1;j<confirm;j++){
                            string b_word1 = get<0>(before[it1+k+j]);
                            string a_word1 = get<0>(after[it2+j+i]);
                            if(a_word1!=b_word1){
                                check = 1;
                                break;
                            }
                        }
                        if(check==1){
                            check = 0;
                            break;
                        }
                        else{
                            net_change = makeChanges(after,k,i,it1,it2,net_change);
                            return make_tuple(net_change,(it1+k),(it2+i));
                        }
                    }
                }
            }
            return tuple<int,int,int>();
        }
        int makeChanges(vector<tuple<string, int>> after, int k, int l, int start_bef, int start_af,int net_change){
            timestamp++;
            for(int m = 0;m<k;m++){
                string w = get<0>(before[start_bef+m]);
                int p = get<1>(before[start_bef+m]);
                remove_one(w,make_tuple(currentFile,(p+net_change),timestamp));
                net_change-=(w.size()+1);
            }
            timestamp++;
            for(int m = 0;m<l;m++){
                string w = get<0>(after[start_af+m]);
                int p = get<1>(after[start_af+m]);
                insertTrie(w,make_tuple(currentFile,p,timestamp));
                net_change+=(w.size()+1);
            }
            return net_change;
        }
        void updateTrie(vector<tuple<string, int>> after){
            int it1 = 0;
            int it2 = 0;
            int net_change = 0;
            int breakout = 0;
            int track_bef;
            int track_af;
            int help = 0;
            while((it1<before.size())&&(it2<after.size())){
                if(help==1){
                    return;
                }
                string b_word = get<0>(before[it1]);
                string a_word = get<0>(after[it2]);
                int b_ind = get<1>(before[it1])+net_change;
                int a_ind = get<1>(after[it2]);
                
                if((a_word==b_word)&&(b_ind==a_ind)){
                    it1++;
                    it2++;
                    continue;
                }
                else{
                    track_af = it2;
                    track_bef = it1;
                    int help = 0;
                    tuple<int,int,int> updates = trackChangesDel(after, it1,it2,net_change);
                    if (updates==tuple<int,int,int>()){
                        while(breakout==0){
                            it2+=50;
                            if(after.size()<it2){
                                it2 = after.size()-3;
                                breakout = 1;
                            }
                            string a_word = get<0>(after[it2]);
                            while((it1!=it2)&&(it1<before.size())){
                                int check = 0;
                                string b_word = get<0>(before[it1]);
                                if(a_word==b_word){
                                    for(int j = 1;j<confirm;j++){
                                        string b_word1 = get<0>(before[it1+j]);
                                        string a_word1 = get<0>(after[it2+j]);
                                        if(a_word1!=b_word1){
                                            check = 1;
                                            break;
                                        }
                                    }
                                    if(check==1){
                                        check = 0;
                                        it1++;
                                        continue;
                                    }
                                    else{
                                        int k = it1-track_bef;
                                        int l = it2-track_af;
                                        net_change=makeChanges(after,k,l,track_bef,track_af,net_change);
                                        breakout = 2;
                                        break;
                                    }
                                }
                                it1++;
                            }
                        }
                        if(breakout!=2){
                            makeChanges(after,(before.size()-track_bef),(after.size()-track_af),track_bef,track_af,net_change);
                            before=after;
                            return;
                        }
                        
                    }
                    else{
                        it1 = get<1>(updates);
                        it2 = get<2>(updates);
                        net_change = get<0>(updates);
                        cout<<it1<<endl;
                        cout<<net_change<<endl;
                        help = 1;
                        return;
                        continue;
                    }
                }
            }
            while(it1<before.size()){
                int lim = before.size()-it1;
                timestamp++;
                for(int m = 0;m<lim;m++){
                    string w = get<0>(before[it1+m]);
                    int p = get<1>(before[it1+m]);
                    remove_one(w,make_tuple(currentFile,(p+net_change),timestamp));
                    net_change-=(w.size()+1);
                }
            }
            while(it2<after.size()){
                int lim = after.size()-it2;
                timestamp++;
                for(int m = 0;m<lim;m++){
                    string w = get<0>(after[it2+m]);
                    int p = get<1>(after[it2+m]);
                    insertTrie(w,make_tuple(currentFile,p,timestamp));
                    net_change+=(w.size()+1);
                }
            }
            before=after;
            return;
        }
       

        //  leads to insertion of a word at the position specified by index, used to build initial Trie
        void insertTrie(string str, tuple<string,int,int> index)
        {
            TrieNode *current = root;
            for (int i = 0; i < str.size(); ++i){
                char ch = str[i];
                if (current->children.find(ch) != current->children.end())
                {
                    if(i==(str.size()-1)){
                        current->EOW_indices[ch].push_back(index);
                    }
                    current->indices[ch].push_back(index);
                    current = current->children[ch];
                }
                else
                {
                    vector<tuple<string,int,int>> positions;
                    positions.push_back(index);
                    if(i==(str.size()-1)){
                        current->EOW_indices.insert(make_pair(ch,positions));
                    }
                    current->children.insert(make_pair(ch, new TrieNode()));
                    current->indices.insert(make_pair(ch, positions));
                    current = current->children[ch];
                }
            }
            if(timestamp == 0){
                return;
            }
            get<2>(index) = timestamp;
            string fil = get<0>(index);
            if(insertions.find(fil)!=insertions.end()){
                insertions[fil].push_back(make_tuple(get<0>(index),get<1>(index),get<2>(index),str));
            }
            else{
                vector<tuple<string,int,int,string>> v;
                v.push_back(make_tuple(get<0>(index),get<1>(index),get<2>(index),str));
                insertions.insert(make_pair(fil,v));

            }
        }
        // leads to insertion of replace string for when prefix search and replace is used
        void insertTrie(string str, vector<tuple<string,int,int>> index, TrieNode *remaining)
        {
            TrieNode *current = root;
            for (int i = 0; i < str.size(); ++i){
                char ch = str[i];
                if (current->children.find(ch) != current->children.end())
                {
                    for(int k = 0; k < index.size(); k++){
                        current->indices[ch].push_back(index[k]);
                    }
                    if(i == (str.size()-1)){
                        for(int k = 0; k < index.size(); k++){
                            current->EOW_indices[ch].push_back(index[k]);
                        }
                        break;
                    }
                    current = current->children[ch];
                }
                else
                {
                    vector<tuple<string,int,int>> positions;
                    for(int k = 0; k < index.size(); k++){
                        positions.push_back(index[k]);
                    }
                    current->children.insert(make_pair(ch, new TrieNode()));
                    current->indices.insert(make_pair(ch, positions));
                    if(i == (str.size()-1)){
                        current->EOW_indices.insert(make_pair(ch, positions));
                        break;
                    }
                    current = current->children[ch];
                }
            }
            merge(current, remaining);
            for (int k = 0; k < index.size(); k++){
                get<2>(index[k]) = timestamp;
                string fil = get<0>(index[k]);
                if(insertions.find(fil)!=insertions.end()){
                    insertions[fil].push_back(make_tuple(get<0>(index[k]),get<1>(index[k]),get<2>(index[k]),str));
                }
                else{
                    vector<tuple<string,int,int,string>> v;
                    v.push_back(make_tuple(get<0>(index[k]),get<1>(index[k]),get<2>(index[k]),str));
                    insertions.insert(make_pair(fil,v));

                }
            }
            return;
        }
        //  for replacing the entire word and not prefix
        void insertTrie(string str, vector<tuple<string,int,int>> index)
        {
            TrieNode *current = root;
            for (int i = 0; i < str.size(); ++i){
                char ch = str[i];
                if (current->children.find(ch) != current->children.end())
                {
                    for(int k = 0; k < index.size(); k++){
                        current->indices[ch].push_back(index[k]);
                    }
                    if(i == (str.size()-1)){
                        for(int k = 0; k < index.size(); k++){
                            current->EOW_indices[ch].push_back(index[k]);
                        }
                        break;
                    }
                    current = current->children[ch];
                }
                else
                {
                    vector<tuple<string,int,int>> positions;
                    for(int k = 0; k < index.size(); k++){
                        positions.push_back(index[k]);
                    }
                    current->children.insert(make_pair(ch, new TrieNode()));
                    current->indices.insert(make_pair(ch, positions));
                    if(i == (str.size()-1)){
                        current->EOW_indices.insert(make_pair(ch, positions));
                        break;
                    }
                    current = current->children[ch];
                }
            }
            for(int k = 0; k < index.size(); k++){
                get<2>(index[k]) = timestamp;
                string fil = get<0>(index[k]);
                if(insertions.find(fil)!=insertions.end()){
                    insertions[fil].push_back(make_tuple(get<0>(index[k]),get<1>(index[k]),get<2>(index[k]),str));
                }
                else{
                    vector<tuple<string,int,int,string>> v;
                    v.push_back(make_tuple(get<0>(index[k]),get<1>(index[k]),get<2>(index[k]),str));
                    insertions.insert(make_pair(fil,v));

                }
            }
        }

        // Used for replacing string during prefix find and replace in insertTrie. 
        // The remaining trie after the prefix to be replaced is merged with the trie of the replace string after its insertion using recursion.
        void merge(TrieNode* current, TrieNode* remaining){
            for (auto i : remaining->children){
                char ch = i.first;
                if (current->children.find(ch) != current->children.end())
                {
                    for(int i = 0; i < remaining->indices[ch].size(); i++){
                        if((get<0>(remaining->indices[ch][i])!=currentFile)&&(local==1)){
                            continue;
                        }
                        current->indices[ch].push_back(remaining->indices[ch][i]);
                    }
                    for(int i = 0; i < remaining->EOW_indices[ch].size(); i++){
                        if((get<0>(remaining->EOW_indices[ch][i])!=currentFile)&&(local==1)){
                            continue;
                        }
                        current->EOW_indices[ch].push_back(remaining->EOW_indices[ch][i]);
                    }
                    merge(current->children[ch], remaining->children[ch]);
                }
                else
                {
                    vector<tuple<string,int,int>> positions;
                    for(int i = 0; i < remaining->indices[ch].size(); i++){
                        if((get<0>(remaining->indices[ch][i])!=currentFile)&&(local==1)){
                            continue;
                        }
                        positions.push_back(remaining->indices[ch][i]);
                    }
                    vector<tuple<string,int,int>> pos;
                    for(int i = 0; i < remaining->EOW_indices[ch].size(); i++){
                        if((get<0>(remaining->EOW_indices[ch][i])!=currentFile)&&(local==1)){
                            continue;
                        }
                        pos.push_back(remaining->EOW_indices[ch][i]);
                    }
                    current->children.insert(make_pair(ch, remaining->children[ch]));
                    current->indices.insert(make_pair(ch, positions));
                    current->EOW_indices.insert(make_pair(ch, pos));
                }
            }
            return;
        }
        //  finds instances of the string including prefixes.
        vector<tuple<string,int,int>> findWord(string str){
            TrieNode *current = root;
            for (int i = 0; i < str.size(); ++i){
                char ch = str[i];
                if (current->children.find(ch) != current->children.end())
                {
                    if(i == (str.size()-1)){
                        break;
                    }
                    current = current->children[ch];
                }
                else{
                    return vector<tuple<string,int,int>>();
                }
            }
            return current->indices.empty() ? vector<tuple<string,int,int>>() : new_index(current->indices[str[str.size()-1]]);
            
        }

        // finds instances of entire words
        vector<tuple<string,int,int>> findWord_entire(string str){
            TrieNode *current = root;
            for (int i = 0; i < str.size(); ++i){
                char ch = str[i];
                if (current->children.find(ch) != current->children.end())
                {
                    if(i == (str.size()-1)){
                        break;
                    }
                    current = current->children[ch];
                }
                else{
                    return vector<tuple<string,int,int>>();
                }
            }
            return current->indices.empty() ? vector<tuple<string,int,int>>() : new_index(current->EOW_indices[str[str.size()-1]]);
            
        }

        // calculates the updated indices for findWord functions that are stored in the trie at 
        // time of insertion by iterating over insertion and deletion vectors to find new index
        vector<tuple<string,int,int>> new_index(vector<tuple<string,int,int>> index){
            for(int i = 0;i<index.size();i++){
                string fil = get<0>(index[i]);
                if((fil!=currentFile)&&(local==1)){
                    continue;
                }
                set<tuple<string,int,int,string,int>> all;
                if(deletions.find(fil)!=deletions.end()){
                    for(int p = 0;p<deletions[fil].size();p++){
                        all.insert(make_tuple(fil,get<1>(deletions[fil][p]),get<2>(deletions[fil][p]),get<3>(deletions[fil][p]),0));
                    }
                }
                if(insertions.find(fil)!=insertions.end()){
                    for(int p = 0;p<insertions[fil].size();p++){
                        all.insert(make_tuple(fil,get<1>(insertions[fil][p]),get<2>(insertions[fil][p]),get<3>(insertions[fil][p]),1));
                    }
                }
                int init_time = get<2>(index[i]);
                string init_file = get<0>(index[i]);
                int init_pos = get<1>(index[i]);
                int real_pos = init_pos;
                set<tuple<string,int,int,string,int>>::iterator itr;
                for (itr = all.begin();itr != all.end(); itr++)
                {
                    int len = get<3>(*itr).size()+1;
                    int time = get<2>(*itr);
                    int pos = get<1>(*itr);
                    int check = get<4>(*itr);
                    if((time > init_time) && (pos<real_pos)){
                        if(check == 0){
                            real_pos-=len;
                        }
                        else{
                            real_pos+=len;
                        }
                    }
                }
                get<1>(index[i])=real_pos;
                get<2>(index[i])=timestamp;
            }

            // indices are inserted into a set and pushed back into a vector to remove duplicates and get the indices in ascending order
            set<tuple<string,int,int>> ind;
            for(int i =0;i<index.size();i++){
                if((get<0>(index[i])!=currentFile)&&(local==1)){
                    continue;
                }
                ind.insert(index[i]);
            }
            set<tuple<string,int,int>>::iterator itr;
            vector<tuple<string,int,int>> in;
            for (itr = ind.begin();itr != ind.end(); itr++)
            {
                in.push_back(*itr);
            }
            return in;
            
        }
        // calculates new index for single index tuple
        tuple<string,int,int> new_index(tuple<string,int,int> index,int time){
            int init_time = get<2>(index);
            int init_pos = get<1>(index);
            string init_file = get<0>(index);
            if(deletions.find(init_file)==deletions.end()){
                vector<tuple<string,int,int,string>> v;
                deletions.insert(make_pair(init_file,v));
            }
            if(insertions.find(init_file)==insertions.end()){
                vector<tuple<string,int,int,string>> v;
                insertions.insert(make_pair(init_file,v));
            }
            int real_pos = init_pos;
            int it1 = 0;
            int it2 = 0;
            int del_size = deletions[init_file].size();
            int ins_size = insertions[init_file].size();
            while((it1<del_size) && (it2<ins_size)){
                int len_del = get<3>(deletions[init_file][it1]).size()+1;
                int len_ins = get<3>(insertions[init_file][it2]).size()+1;
                int del = get<2>(deletions[init_file][it1]);
                int del_pos = get<1>(deletions[init_file][it1]);
                int ins = get<2>(insertions[init_file][it2]);
                int ins_pos = get<1>(insertions[init_file][it2]);
                if((del<ins)&&(del>init_time)){
                    if(del_pos < real_pos){
                        real_pos-=len_del;
                    }
                    it1++;
                }
                else if((del>ins)&&(ins>init_time)){
                    if(ins_pos < real_pos){
                        real_pos+=len_ins;
                    }
                    it2++;
                }
                else if((del==ins)&&(ins>init_time)){
                    if(ins_pos < real_pos){
                        real_pos+=len_ins;
                    }
                    if(del_pos < real_pos){
                        real_pos-=len_del;
                    }
                    it1++;
                    it2++;
                }
                else{
                    it1++;
                    it2++;
                    continue;
                }
            }
                while(it1<del_size){
                    int len_del = get<3>(deletions[init_file][it1]).size()+1;
                    int del = get<2>(deletions[init_file][it1]);
                    int del_pos = get<1>(deletions[init_file][it1]);
                    if((del>init_time) && (del_pos < real_pos)){
                        real_pos-=len_del;
                    }
                    it1++;
                }
                while(it2<ins_size){
                    int len_ins = get<3>(insertions[init_file][it2]).size()+1;
                    int ins = get<2>(insertions[init_file][it2]);
                    int ins_pos = get<1>(insertions[init_file][it2]);
                    if((ins>init_time) && (ins_pos < real_pos)){
                        real_pos+=len_ins;
                    }
                    it2++;
                }
            
            get<1>(index)=real_pos;
            get<2>(index)=time;
            return index;
        }
        void remove_one(string str, tuple<string,int,int> index){
            TrieNode* current = root;
            for(int i = 0; i<str.size();i++){
                char ch = str[i];
                if (current->children.find(ch)!=current->children.end()){
                    int time = get<2>(index);
                    for(int k = 0; k<current->indices[ch].size();k++){
                        if (new_index(current->indices[ch][k],time)==index){
                            current->indices[ch].erase(current->indices[ch].begin()+k);
                            break;
                        }
                    }
                    if(i == (str.size()-1)){
                        int time = get<2>(index);
                        for(int k = 0; k<current->EOW_indices[ch].size();k++){
                            if (new_index(current->EOW_indices[ch][k],time)==index){
                                current->EOW_indices[ch].erase(current->EOW_indices[ch].begin()+k);
                                break;
                            }
                        }
                    }
                    current = current->children[ch];
                }
                else{
                    return;
                }
            }
            get<2>(index) = timestamp;
            string fil = get<0>(index);
            if(deletions.find(fil)!=deletions.end()){
                deletions[fil].push_back(make_tuple(get<0>(index),get<1>(index),get<2>(index),str));
            }
            else{
                vector<tuple<string,int,int,string>> v;
                v.push_back(make_tuple(get<0>(index),get<1>(index),get<2>(index),str));
                deletions.insert(make_pair(fil,v));

            }
        }
        
        // removes string, which can also be a prefix of word, to be replaced and returns the node to its children which is to be inserted after replace string.
        TrieNode* remove(string str, vector<tuple<string,int,int>> index){
            if(index.empty()==true){
                cout<<"Does not exist";
                // throw error for word not existing
            }
            TrieNode* current = root;
            TrieNode* remaining = new TrieNode();
            for(int i = 0; i<str.size();i++){
                char ch = str[i];
                if (current->children.find(ch)!=current->children.end()){
                    if(local!=1){
                        current->indices.erase(ch);
                    }
                    else{
                        for(int m = 0;m<current->indices[ch].size();m++){
                            if(get<0>(current->indices[ch][m])!=currentFile){
                                continue;
                            }
                            current->indices[ch].erase(current->indices[ch].begin()+m);
                        }
                    }
                    if (i == str.size()-1){
                        remaining = current->children[ch];
                        if(local!=1){
                            current->EOW_indices.erase(ch);
                        }
                        else{
                            for(int m = 0;m<current->EOW_indices[ch].size();m++){
                            if(get<0>(current->EOW_indices[ch][m])!=currentFile){
                                continue;
                            }
                            current->EOW_indices[ch].erase(current->EOW_indices[ch].begin()+m);
                            }
                        }
                        break;
                    }
                    current = current->children[ch];
                }
                else{
                    return remaining;
                }
            }
            for(int k = 0; k < index.size(); k++){
                string fil = get<0>(index[k]);
                if(local==1){
                    if(fil!=currentFile){
                        continue;
                    }
                }
                get<2>(index[k]) = timestamp;
                if(deletions.find(fil)!=deletions.end()){
                    deletions[fil].push_back(make_tuple(get<0>(index[k]),get<1>(index[k]),get<2>(index[k]),str));
                }
                else{
                    vector<tuple<string,int,int,string>> v;
                    v.push_back(make_tuple(get<0>(index[k]),get<1>(index[k]),get<2>(index[k]),str));
                    deletions.insert(make_pair(fil,v));

                }
            }
            return remaining;       
        }

        // removes strings which are entire words to be replaced.
        void remove_entire(string str, vector<tuple<string,int,int>> EOW_index){
            if(EOW_index.empty()==true){
                cout<< "Word not found";
                return;
                // throw error for word not existing
            }
            TrieNode* current = root;
            for(int i = 0; i<str.size();i++){
                char ch = str[i];
                if (current->children.find(ch)!=current->children.end()){
                    for(int j = 0; j<EOW_index.size();j++){
                        int time = get<2>(EOW_index[j]);
                        for(int k = 0; k<current->indices[ch].size();k++){
                            if((get<0>(current->indices[ch][k])!=currentFile)&&(local==1)){
                                continue;
                            }
                            if (new_index(current->indices[ch][k],time)==EOW_index[j]){
                                current->indices[ch].erase(current->indices[ch].begin()+k);
                                // break;
                            }
                        }
                    }
                    if(i == (str.size()-1)){
                        if(local!=1){
                            current->EOW_indices.erase(ch);
                        }
                        else{
                            for(int m = 0;m<current->EOW_indices[ch].size();m++){
                                if(get<0>(current->EOW_indices[ch][m])!=currentFile){
                                    continue;
                                }
                                current->EOW_indices[ch].erase(current->EOW_indices[ch].begin()+m);
                            }
                        }
                    }
                    current = current->children[ch];
                }
                else{
                    return;
                }
            }
            for(int k = 0; k < EOW_index.size(); k++){
                string fil = get<0>(EOW_index[k]);
                if(local==1){
                    if(fil!=currentFile){
                        continue;
                    }
                }
                get<2>(EOW_index[k]) = timestamp;
                if(deletions.find(fil)!=deletions.end()){
                    deletions[fil].push_back(make_tuple(get<0>(EOW_index[k]),get<1>(EOW_index[k]),get<2>(EOW_index[k]),str));
                }
                else{
                    vector<tuple<string,int,int,string>> v;
                    v.push_back(make_tuple(get<0>(EOW_index[k]),get<1>(EOW_index[k]),get<2>(EOW_index[k]),str));
                    deletions.insert(make_pair(fil,v));

                }
            }
        }

        // deals with various cases of search and replace queries such as different number of words in each, multi-word search and replace, single word search and replace
        void replace(string search, string replace){
            vector<string> multi_search;
            vector<string> multi_replace;
            int counter_search = 1;
            int counter_replace = 1;
            for(int i = 0; i<search.size();i++){
                if(search[i]==' '){
                    counter_search++;
                }
            }
            for(int i = 0; i<replace.size();i++){
                if(replace[i]==' '){
                    counter_replace++;
                }
            }
            string str;
            for(int i = 0; i<search.size();i++){
                if(search[i]!=' '){
                    str=str+search[i];
                }
                else{
                    multi_search.push_back(str);
                    str.clear();
                }
            }
            multi_search.push_back(str);
            str.clear();
            for(int i = 0; i<replace.size();i++){
                if(replace[i]!=' '){
                    str=str+replace[i];
                }
                else{
                    multi_replace.push_back(str);
                    str.clear();
                }
            }
            multi_replace.push_back(str);
            str.clear();
            int difference = counter_replace-counter_search;
            vector<vector<tuple<string,int,int>>> all_indices = multi_index(multi_search);
            // if replace string has fewer words than search string
            if(difference<0){
                int j;
                int len_search=0;
                int len_replace=0;
                int change = 0;
                for(j = 0; j<counter_replace;j++){
                    change = len_replace-len_search;
                    for(int m = 0;m<all_indices[j].size();m++){
                        get<1>(all_indices[j][m])+=change;
                    }
                    replace_multi(multi_search[j],multi_replace[j], all_indices[j]);
                    len_search += multi_search[j].size()+1;
                    len_replace += multi_replace[j].size()+1;
                }
                change = 0;
                for(j=counter_replace;j<counter_search;j++){
                    timestamp++;
                    change += (multi_search[j].size()+1);
                    vector<tuple<string,int,int>> index = all_indices[counter_replace-1];
                    string init = get<0>(index[0]);
                    int del = 0;
                    for(int k = 0;k<index.size();k++){
                        if(get<0>(index[k])!=init){
                            init = get<0>(index[k]);
                            del = 0;
                        }
                        get<1>(index[k])-=((del)*(j-counter_search+1)*(change)-(multi_replace[counter_replace-1].size()+1));
                        get<2>(index[k])=timestamp;
                        del++;
                    }
                    remove_entire(multi_search[j], index);
                }
            }
            // if replace string has more words than search string
            else if(difference>0){
                int j;
                int len_search=0;
                int len_replace=0;
                int change = 0;
                for(j = 0; j<counter_search;j++){
                    change = len_replace-len_search;
                    for(int m = 0;m<all_indices[j].size();m++){
                        get<1>(all_indices[j][m])+=change;
                    }
                    replace_multi(multi_search[j],multi_replace[j], all_indices[j]);
                    len_search += multi_search[j].size()+1;
                    len_replace += multi_replace[j].size()+1;
                }
                change = 0;
                for(j=counter_search;j<counter_replace;j++){
                    timestamp++;
                    change += (multi_replace[j].size()+1);
                    vector<tuple<string,int,int>> index = all_indices[counter_search-1];
                    string init = get<0>(index[0]);
                    int ins = 0;
                    for(int k = 0;k<index.size();k++){
                        if(get<0>(index[k])!=init){
                            init = get<0>(index[k]);
                            ins = 0;
                        }
                        get<1>(index[k])+=((multi_replace[counter_search-1].size()+1)+(j-counter_search)*(change)+(ins)*(j-counter_search+1)*(change));
                        get<2>(index[k])=timestamp;
                        ins++;
                    }
                    insertTrie(multi_replace[j], index);
                }
            }
            // if search and replace strings have equal nummber of words
            else {
                int len_search=0;
                int len_replace=0;
                int change = 0;
                for(int j=0; j<counter_search;j++){
                    change = len_replace-len_search;
                    for(int m = 0;m<all_indices[j].size();m++){
                        get<1>(all_indices[j][m])+=change;
                    }
                    replace_multi(multi_search[j],multi_replace[j], all_indices[j]);
                    len_search += multi_search[j].size()+1;
                    len_replace += multi_replace[j].size()+1;
                }
            }
            
        }

        // to calculate indices of multi-word search strings as tries only support single word search at a time
        vector<vector<tuple<string,int,int>>> multi_index(vector<string> multi_search){
            vector<vector<tuple<string,int,int>>> relevant_indices;
            if(multi_search.size()==1){
                vector<tuple<string,int,int>> index = findWord_entire(multi_search[0]);
                relevant_indices.push_back(index);
                return relevant_indices;
            }
            int start_size = findWord_entire(multi_search[0]).size();
            int end_size = findWord_entire(multi_search[multi_search.size()-1]).size();
            vector<vector<tuple<string,int,int>>> indices;
            
            vector<tuple<string,int,int>> init;
            vector<tuple<string,int,int>> temp;
            vector<tuple<string,int,int>> relevant;
            // finds indices for all words of string individually
            for(int q = 0; q<multi_search.size();q++){
                vector<tuple<string,int,int>> index = findWord_entire(multi_search[q]);
                indices.push_back(index);
            }
            // finds two consecutive words which have consecutive indices, starting from either the start of the string 
            // or the end based on which has less number of indices. Then it goes to next word and uses the consecutive indices
            // to find instances of all three being consecutive. Basically sieves out non-consecutive indices of the first word
            if(start_size>end_size){
                relevant = indices[multi_search.size()-1];
                for(int l = (indices.size()-1);l>0;l--){
                    for(int m = 0;m<relevant.size();m++){
                        for(int n = 0; n < indices[l-1].size();n++){
                            if(get<0>(relevant[m])==get<0>(indices[l-1][n])&&(get<1>(relevant[m])==(get<1>(indices[l-1][n])+multi_search[l-1].size()+1))){
                                if(l==(indices.size()-1)){
                                    init.push_back(relevant[m]);
                                }
                                temp.push_back(indices[l-1][n]);
                                break;
                            }
                        }
                    }
                    if(!init.empty()){
                        relevant_indices.push_back(init);
                    }
                    relevant_indices.push_back(temp);
                    relevant.clear();
                    for(int k = 0; k<temp.size();k++){
                        relevant.push_back(temp[k]);
                    }
                    temp.clear();
                    init.clear();
                }
                reverse(relevant_indices.begin(), relevant_indices.end());
            }
            else{
                relevant = indices[0];
                for(int l = 0;l<(indices.size()-1);l++){
                    for(int m = 0;m<relevant.size();m++){
                        for(int n = 0; n < indices[l+1].size();n++){
                            if(get<0>(relevant[m])==get<0>(indices[l+1][n])&&(get<1>(relevant[m])==(get<1>(indices[l+1][n])-multi_search[l].size()-1))){
                                if(l==0){
                                    init.push_back(relevant[m]);
                                }
                                temp.push_back(indices[l+1][n]);
                                break;
                            }
                        }
                    }
                    if(!init.empty()){
                        relevant_indices.push_back(init);
                    }
                    relevant_indices.push_back(temp);
                    relevant.clear();
                    for(int k = 0; k<temp.size();k++){
                        relevant.push_back(temp[k]);
                    }
                    temp.clear();
                    init.clear();
                }
            }
            return relevant_indices;

        }

        // used when replacing multiple words
        void replace_multi(string search, string replace, vector<tuple<string,int,int>> index){
            remove_entire(search, index);
            insertTrie(replace, index);
        }

        // helps replace function for prefix search and replace queries
        void replace_single(string search, string replace){
            vector<tuple<string,int,int>> index = findWord(search);
            TrieNode* remaining = remove(search, index);
            insertTrie(replace, index, remaining);
        }

        // helps replace function for search and replace queries of entire words.
        void replace_single_entire(string search, string replace){
            vector<tuple<string,int,int>> index = findWord_entire(search);
            remove_entire(search, index);
            insertTrie(replace, index);
        }

        // regex to benchmark
        vector<tuple<string,int,int>> regex(string search){
            fstream file;
            string word, t, q, filename;
            string copy;
            unordered_set<char> non_word_boundaries;
            vector<tuple<string,int,int>> indices;
            for(int q=1;q<192;q++){
                filename = "sample-";
                string add = to_string(q);
                size_t n = 3;
                int precision = n - min(n, add.size());
                add.insert(0, precision, '0');
                filename = filename + add + ".txt";
                file.open(filename.c_str());
                int index_counter = 0;
                // extracting words from the file
                while (file >> word)
                {
                    // displaying content
                    // if punctuation in middle of a word, also considers that. Thus, currently only for demo
                    copy.clear();
                    for(int i=0;i<word.size();i++){
                        if(non_word_boundaries.find(word[i])!= non_word_boundaries.end()){
                            copy=copy+(char)tolower(word[i]);
                        }
                    }
                    if(search==word){
                        indices.push_back(make_tuple(filename,index_counter,0));
                    }

                    index_counter++;
                }
                file.close();
            }
            return indices;
        }
};

