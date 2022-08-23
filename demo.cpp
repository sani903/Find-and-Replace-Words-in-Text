#include <bits/stdc++.h>
#include "TrieFindandReplace.cpp"

vector<tuple<string,int>> readA(unordered_set<char> non_word_boundaries, unordered_set<char> whiteSpaceChars){
    string filename = "text/a1.txt";
    int index_counter = 0;
    vector<char> bytes;
    char byte = 0;
    ifstream input_file(filename);
    string copy;
    vector<tuple<string,int>> after;
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"<< filename << "'" << endl;
        return vector<tuple<string,int>>();
    }
    while (input_file.get(byte)) {
        bytes.push_back(byte);
    }
    for (const auto &i : bytes) {
        if(non_word_boundaries.find(i)!= non_word_boundaries.end()){
            copy=copy+(char)tolower(i);
        }
        else if(whiteSpaceChars.find(i)!= whiteSpaceChars.end()){
            if(copy.size()!=0){
                int indx = index_counter-copy.size();
                tuple<string,int> ind = make_tuple(copy,indx);
                after.push_back(ind);
                copy.clear();
            }
        }
        else{
            if(((i+1)<bytes.size())&&(whiteSpaceChars.find(bytes[i+1])!=whiteSpaceChars.end())){
                    copy=copy+(char)tolower(bytes[i]);
                    index_counter++;
                    continue;
                }
            if(copy.size()!=0){
                int indx = index_counter-copy.size();
                tuple<string,int> ind = make_tuple(copy,indx);
                after.push_back(ind);
                copy.clear();
            }
            after.push_back(make_tuple("",index_counter));
        }
        index_counter++;
    }
    input_file.close();
    return after;
}

int main(void){
    Trie T;
    fstream file;
    string word, t, q, filename;
    string copy;
    unordered_set<char> non_word_boundaries;
    unordered_set<char> whiteSpaceChars;
    // filename of the file
    char _chars[] = {'7', '0', '2', '9', '8', '3', '4', '6', '1', '5', 'v', 'Q', 'r', 'g', 'K', 'U', 'n', 'f', 's', 'G', 'E', 'M', 'W', 'a', 't', 'i', 'H', 'J', 'V', 'O', 'l', 'A', 'd', 'u', 'T', 'p', 'P', 'F', 'B', 'Z', 'q', 'o', 'D', 'c', 'y', 'X', 'Y', 'h', 'x', 'm', 'w', 'S', 'I', 'C', 'k', 'e', 'j', 'L', 'R', 'z', 'N', 'b', '_'};
    for(const auto &s: _chars)
    {
        non_word_boundaries.insert(s);
    }

    whiteSpaceChars.insert('\t');
    whiteSpaceChars.insert('\n');
    whiteSpaceChars.insert('\a');
    whiteSpaceChars.insert(' ');
    vector<tuple<string,int>> after;
    vector<tuple<string,int>> after1;
    // opening file
    auto start1 = high_resolution_clock::now();
    for(int q=1;q<192;q++){
        filename = "text/p-";
        string add = to_string(q);
        size_t n = 3;
        int precision = n - min(n, add.size());
        add.insert(0, precision, '0');
        filename = filename + add + ".txt";
        // file.open(filename.c_str());
        int index_counter = 0;
        vector<char> bytes;
        char byte = 0;
        ifstream input_file(filename);
        if (!input_file.is_open()) {
            cerr << "Could not open the file - '"<< filename << "'" << endl;
            return EXIT_FAILURE;
        }
        T.setCurrentFile(filename);
        while (input_file.get(byte)) {
            bytes.push_back(byte);
        }
        for (const auto &i : bytes) {
            if(non_word_boundaries.find(i)!= non_word_boundaries.end()){
                copy=copy+(char)tolower(i);
            }
            else if(whiteSpaceChars.find(i)!= whiteSpaceChars.end()){
                if(copy.size()!=0){
                    int indx = index_counter-copy.size();
                    tuple<string,int> ind = make_tuple(copy,indx);
                    after.push_back(ind);
                    copy.clear();
                }
            }
            else{
                if(((i+1)<bytes.size())&&(whiteSpaceChars.find(bytes[i+1])!=whiteSpaceChars.end())){
                    copy=copy+(char)tolower(bytes[i]);
                    index_counter++;
                    continue;
                }
                if(copy.size()!=0){
                    int indx = index_counter-copy.size();
                    tuple<string,int> ind = make_tuple(copy,indx);
                    after.push_back(ind);
                    copy.clear();
                }
                after.push_back(make_tuple("",index_counter));
            }
            index_counter++;
        }
        T.init_build(after);
        if(q==3){
            after1 = after;
        }
        after.clear();
        input_file.close();
    }
    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(stop1 - start1);
    cout << "Time taken by function: "<< duration1.count() << " microseconds" << endl;
    T.setCurrentFile("text/p-003.txt");
    T.before = after1;
    after = readA(non_word_boundaries,whiteSpaceChars);
    auto start2 = high_resolution_clock::now();
    T.updateTrie(after);
    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    cout << "Time taken by function: "<< duration2.count() << " microseconds" << endl;
    cout << "Press a key" << endl;
    cout << "1: Search with prefix" <<endl;
    cout << "2: Search with entire word" <<endl;
    cout << "3: Remove a prefix" <<endl;
    cout << "4: Remove a word" <<endl;
    cout << "5: Replace a prefix" <<endl;
    cout << "6: Replace a word" <<endl;
    cout << "7: Replace multiple words" <<endl;
    cout << "9: Exit" <<endl;
    while(true){
        int n;
        cin>> n;
        string str;
        string fil;
        int pos;
        int w;
        string str1;
        time_point<std::chrono::_V2::system_clock, std::chrono::duration<long long int, std::ratio<1, 1000000000> > > start;
        time_point<std::chrono::_V2::system_clock, std::chrono::duration<long long int, std::ratio<1, 1000000000> > > stop;
        duration<long long int, std::ratio<1, 1000000>> duration;
        vector<tuple<string,int,int>> index;
        vector<string> multi;
        string str2;
        vector<tuple<string,int,int>> b;
        switch(n){
            case 1:
                cout <<"Enter the prefix: ";
                cin >> str;
                start = high_resolution_clock::now();
                index=T.findWord(str);
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                cout <<"The indices in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
                break;
            case 2:
                cout <<"Enter the word: ";
                cin >> str;
                start = high_resolution_clock::now();
                index=T.findWord_entire(str);
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                cout <<"The indices in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
                break;
            case 3:
                cout <<"Enter the prefix: ";
                cin >> str;
                index=T.findWord(str);
                cout <<"The indices in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                start = high_resolution_clock::now();
                T.remove(str,index);
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                index=T.findWord(str);
                cout <<"The indices remaining in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout<<endl;
                cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
                break;
            case 4:
                cout <<"Enter the word: ";
                cin >> str;
                index=T.findWord_entire(str);
                cout <<"The indices in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                start = high_resolution_clock::now();
                T.remove_entire(str,index);
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                index=T.findWord_entire(str);
                cout <<"The indices remaining in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout<<endl;
                cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
                break;
            case 5:
                cout <<"Enter the prefix: ";
                cin >> str;
                cout <<"Enter the replacement: ";
                cin >> str1;
                index=T.findWord(str);
                cout <<"The indices in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                start = high_resolution_clock::now();
                T.replace_single(str,str1);
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                index=T.findWord(str1);
                cout <<"The indices remaining in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
                break;
            case 6:
                cout <<"Enter the word: ";
                cin >> str;
                cout <<"Enter the replacement: ";
                cin >> str1;
                index=T.findWord_entire(str);
                cout <<"The indices in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                 start = high_resolution_clock::now();
                T.replace_single_entire(str,str1);
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                index=T.findWord_entire(str1);
                cout <<"The indices remaining in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
                break;
            case 7:
                str = "second edition";
                str1 = "udaan project demo";
                str2.clear();
                for(int i = 0; i<str.size();i++){
                    if(str[i]!=' '){
                        str2=str2+str[i];
                    }
                    else{
                        multi.push_back(str2);
                        str2.clear();
                    }
                }
                multi.push_back(str2);
                start = high_resolution_clock::now();
                index=T.multi_index(multi)[0];
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                cout <<"The indices in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                start = high_resolution_clock::now();
                T.replace(str,str1);
                stop = high_resolution_clock::now();
                duration = duration_cast<microseconds>(stop - start);
                multi.clear();
                // for(int p = 0; p< T.insertions.size();p++){
                //     cout<<"("<<get<0>(T.insertions[p])<<","<<get<1>(T.insertions[p])<<","<<get<2>(T.insertions[p])<<","<<get<3>(T.insertions[p])<<") ";
                // }
                cout << endl;
                str2.clear();
                for(int i = 0; i<str1.size();i++){
                    if(str1[i]!=' '){
                        str2=str2+str1[i];
                    }
                    else{
                        multi.push_back(str2);
                        str2.clear();
                    }
                }
                multi.push_back(str2);
                str2.clear();
                index=T.multi_index(multi)[0];
                cout << "The indices remaining in the format (file index) are: ";
                for(w =0;w<index.size();w++){
                    fil = get<0>(index[w]);
                    pos = get<1>(index[w]);
                    cout <<"("<<fil<<" "<<pos<<"), "; 
                }
                cout <<endl;
                cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
                break;
            case 8:
                return 0;
                break;
            default:
                cout << "Enter a valid option"<<endl;
                break;
        }
    }    



}