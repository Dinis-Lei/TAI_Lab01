#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <bits/stdc++.h>
#include <cmath>


using namespace std;
static int get_alphabet(string filename, unordered_set<char> &alphabet);

int main(int argc, char** argv) {

    map<string, map<string, float>> dict;   // sequence -> <symbol, probability>
    int n = 100;                          // number of generated symbols
    int k = 4;                            // length of the sequence
    string filename;
    string sample = "";

    unordered_set<char> alphabet;


    // Process command line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-k") {
            i++;
            if (i >= argc) {
                cerr << "Invalid k argument" << endl;
                return EXIT_FAILURE;
            }
            try {
                k = stoi(argv[i]);
            } catch (exception& e) {
                cerr << "Invalid k argument: " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "-n") {
            i++;
            if (i >= argc) {
                cerr << "Invalid n argument" << endl;
                return EXIT_FAILURE;
            }
            try {
                n = stoi(argv[i]);
            } catch (exception& e) {
                cerr << "Invalid n argument: " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        }else if (arg == "-s") {
            i++;
            if (i >= argc) {
                cerr << "Invalid s argument" << endl;
                return EXIT_FAILURE;
            }
            try {
                sample = argv[i];
            } catch (exception& e) {
                cerr << "Invalid s argument: " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        } else if (filename.empty()) {
            filename = arg;
        } else {
            cerr << "Unknown argument: " << arg << endl;
            return EXIT_FAILURE;
        }
    }

    if (filename.empty()) {
        cerr << "No input file specified" << endl;
        return EXIT_FAILURE;
    }

    // Open the input file
    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Failed to open input file: " << filename << endl;
        return EXIT_FAILURE;
    }



    // Read the input file
    string sequence = "";
    char c;
    while (infile.get(c)) {
        sequence += c;
        alphabet.insert(c);

        if (sequence.length() == k + 1) {
            string prefix = sequence.substr(0, k);
            string next_char = sequence.substr(k, 1);

            if (!dict.count(prefix)) {
                dict[prefix]["total"] = 0;
            }
            dict[prefix]["total"]++;
            if (!dict[prefix].count(next_char)) {
                dict[prefix][next_char] = 0;
            }
            dict[prefix][next_char]++;
            sequence = sequence.substr(1);
        }
    }

    // Compute probabilities
    for (auto& [prefix, counts] : dict) {
        for (auto& [next_char, count] : counts) {
            if (next_char != "total") {
                counts[next_char] /= counts["total"];
            }
        }
        counts.erase("total");
    }

    // Generate output
    for (auto& [prefix, counts] : dict) {
        cout << prefix << ":" << endl;
        for (auto& [next_char, prob] : counts) {
            cout << "  -> " << next_char << ": " << prob << endl;
        }
    }
    string output="";
    if (sample == "")
    {
        sample=sequence;
    } else
    {
        sample= sample.substr(sample.length()-k-1,sample.length()-1);
    }
    
    
    srand( (unsigned)time( NULL ) );
    for(int i=0; i<n;i++){
        float total = 0.0;
        float rand_val = (float) rand()/RAND_MAX;
        std::cout << rand_val << std::endl;
        if(dict.count(sample)==0){
            //how to view lenght of alphabet

            float prob = 1/(float)alphabet.size();
            int index=floor(rand_val*(alphabet.size()-1));
            std::unordered_set<char>::iterator it = alphabet.begin();
            std::advance(it, index);
            char x = *it;
            output+=x;
            sample+=x;
            sample = sample.substr(1);
            
        }
        else{
            for(std::map<string,float>::iterator it = dict[sample].begin(); it != dict[sample].end(); ++it) {
                total += it-> second;
                if(rand_val < total ){
                    output += it-> first;
                    sample+=it->first;
                    sample = sample.substr(1);
                    break;
                }
            }
        }
    }
    cout << output << endl;
    return 0;
}


