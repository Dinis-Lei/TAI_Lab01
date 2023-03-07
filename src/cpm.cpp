#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>

using namespace std;




void get_alphabet(string filename, unordered_set<char>& alphabet) {
    char byte = 0;


    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return;
    }

    while (input_file.get(byte)) {
        alphabet.insert(byte);
    }
}







int main(int argc, char** argv) {

    int c;
    int k = 3;

    while ((c = getopt(argc, argv, "k:")) != -1)
    {
        switch (c)
        {
            case 'k':
            {
                try {
                    k = stoi(optarg);
                }
                catch (exception &err) {
                    std::cout << "Invalid K argument" << std::endl;
                    return 1;
                }
                std::cout << "K = " << k << std::endl;
                break;
            }
            case '?':
            {
                std::cout << "Got unknown option." << std::endl; 
                break;
            }
            default:
            {
                std::cout << "Got unknown parse returns: " << c << std::endl; 
            }
        }
    }
    unordered_set<char> alphabet;
    string filename = argv[optind];

    get_alphabet(filename, alphabet);
    map<char, vector<int>> hit_miss;
    unordered_set<char> :: iterator itr;
    int j = 0;
    for (itr = alphabet.begin(); itr != alphabet.end(); itr++){
        cout << j++ << " " << (*itr) << endl;
        // TODO: Revise this
        hit_miss[*itr].push_back(0);
        hit_miss[*itr].push_back(0);
    }


    map<string, vector<int> > dict;

    // Create a text string, which is used to output the text file
    char byte = 0;

    string sequence = "";
    string full_seq = "";


    // Create and open a text file
    // cout << k << "\n";
    ifstream input_file(filename);

    char next_symbol;
    int r = rand() % alphabet.size();
    vector<char> alpha;
    alpha.insert(alpha.end(), alphabet.begin(), alphabet.end());
    next_symbol = alpha[r];

    int i =0;
    int rand_pred = 0;
    int acc_pred = 0;
    // Use a while loop together with the getline() function to read the file line by line
    while (input_file.get(byte)) {

        // Check if predicted is correct
        if (next_symbol == byte) {
            hit_miss[next_symbol][0] += 1;
        }
        else if(next_symbol != '\0') {
            hit_miss[next_symbol][1] += 1;
        }

        // Output the text from the file
        sequence += byte;
        full_seq += byte;


        if (sequence.length()==k){
            // cout << i+k << "\n";
            // cout << sequence << "\n";
            //cout << "Full Seq: " << full_seq << endl;
            if (dict.count(sequence)) {
                map<char, int> counts;
                //cout << "SEQ: " << sequence << endl;
                for (int i: dict.find(sequence)->second) {
                    char test = full_seq[i];
                    //cout << i << " " << full_seq.substr(i-k, k) << test << endl;
                    if (!counts.count(test)) {
                        counts[test] = 0;
                    }
                    counts[test]++;
                    break;
                }
                // Predict next Symbol
                int max_int = 0;
                char max_char;
                
                for (const auto& pair : counts) {
                    //cout <<"Pair: " << pair.first << " " << pair.second << endl;
                    if (pair.second > max_int) {
                        max_int = pair.second;
                        max_char = pair.first;
                    }
                }
                //cout << "MAX: " << max_char << endl;
                next_symbol = max_char;
                acc_pred++;


            }
            else {
                //cout << sequence << " not found\n";
                // Predict next Symbol
                //r = rand() % alphabet.size();
                //next_symbol = alpha[r];
                //rand_pred++;
                next_symbol = '\0';
            }

            if (dict.count(sequence) == 0){
                dict.insert( make_pair(sequence, vector<int>()) );
            }
            dict[sequence].push_back(i+1);
            sequence.erase(0, 1);

        }

        i++;
        // // Traverse the string
        // for (int i = 0; i < myText.length()-k+1; i++) {
    
        //     // Print current character
        //     string sequence = myText.substr(i,k);
        //     // cout << sequence << "\n";
        //     if (dict.count(sequence) == 0){
        //         dict.insert( make_pair(sequence, vector<int>(i)) );
        //     }
        //     dict[sequence].push_back(i+k);
        //     // cout << i+k << "\n";

        // }




    }
    // cout << "Begin File: " << endl;
    // for(map<string, vector<int> >::const_iterator it = dict.begin();
    // it != dict.end(); ++it)
    // {
    //     std::cout << it->first << " : " ;

    //     for (int index: it->second){
    //         if (index != 0)
    //             std::cout  << index << " " ;

    //      }

    //     std::cout << endl;
    // }

    cout << "R: " << rand_pred << " A: " << acc_pred << endl;
    map<char, vector<int>>::iterator it;
    for(it = hit_miss.begin(); it != hit_miss.end(); it++) {
        int hits = it->second[0];
        int misses = it->second[1];
        float ps = (float) hits/ (float) (hits+misses);
        cout << it->first << " : " << hits << " " << misses << " " << ps << " " << -log2(ps) << endl;
    }



    // Close the file
    input_file.close(); 

}