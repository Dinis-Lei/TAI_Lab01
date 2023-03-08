#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>

using namespace std;


static void get_alphabet(string filename, unordered_set<char>& alphabet);

static double estimate_probability(int hits, int misses, double alpha);

int main(int argc, char** argv) {

    int c;
    int k = 4;
    double alpha = 0.1;

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
    unordered_set<char> :: iterator itr;
    int j = 0;
    for (itr = alphabet.begin(); itr != alphabet.end(); itr++){
        cout << j++ << " " << (*itr) << endl;
    }

    // Create a text string, which is used to output the text file
    char byte = 0;

    string sequence = "";
    string full_seq = "";


    // Create and open a text file
    // cout << k << "\n";
    ifstream input_file(filename);

    char next_symbol = '\0';
    int i =0;
    struct Pattern {
        int hits = 0;
        int misses = 0;
        string seq = "";
        vector<int> positions;
    };
    map<string, Pattern> dict;

    // Use a while loop together with the getline() function to read the file line by line
    while (input_file.get(byte)) {

        // Check if predicted is correct
        if (next_symbol == byte) {
            dict[sequence].hits++;
        }
        else if(next_symbol != '\0') {
            dict[sequence].misses++;
        }

        // Output the text from the file
        if (sequence.length() == k) {
            sequence.erase(0, 1);
        }

        sequence += byte;
        full_seq += byte;


        if (sequence.length()==k){

            // Check if sequence has been seen
            if (dict.count(sequence)) {
                map<char, int> counts;
                
                // Count the different occurences of each char
                
                for (int i: dict.find(sequence)->second.positions) {
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
            }
            else {
                // No Prediction made
                next_symbol = '\0';
            }

            if (dict.count(sequence) == 0){
                Pattern new_seq;
                new_seq.seq = sequence;
                dict.insert( make_pair(sequence, new_seq) );
            }
            dict[sequence].positions.push_back(i+1);
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

    map<string, Pattern>::iterator it;
    for(it = dict.begin(); it != dict.end(); it++) {
        int hits = it->second.hits;
        int misses = it->second.misses;
        double ps = estimate_probability(hits, misses, alpha);
        cout << it->first << " : " << hits << " " << misses << " " << ps << " " << -log2(ps) << endl;
    }

    // Close the file
    input_file.close(); 

}

static double estimate_probability(int hits, int misses, double alpha) {
    return (hits + alpha)/(hits+misses+2*alpha);
}

static void get_alphabet(string filename, unordered_set<char> &alphabet) {
    char byte = 0;

    ifstream input_file(filename);
    if (!input_file.is_open())
    {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return;
    }

    while (input_file.get(byte))
    {
        alphabet.insert(byte);
    }
    input_file.close();
}