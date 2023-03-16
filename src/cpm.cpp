#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;


struct Anchor {
    bool is_active = false;         // Check if anchor is being used or not
    int position;                   // Where the anchor is situated in the full sequence
    int hits = 0;                   // Counter of hits
    int misses = 0;                 // Counter of misses
    int length = 0;                 // How many symbols has the anchor tested
    map<char, float> info;          // Accumulative information for each symbol thoughout the anchor test
};

map<char, float> global_info;       // table of symbol - amount of information 
map<char, float>::iterator itr;     // table iterator

static int get_alphabet(string filename, unordered_set<char> &alphabet);

static double estimate_probability(int hits, int misses, double alpha, int asize);

int main(int argc, char** argv) {

    int c;                          // Opt process
    int k = 4;                      // Anchor size
    int min_size = 3;               // Minimum number of symbols to start evaluating the anchor
    float alpha = 1;                // Smoothing
    float threshold = 0.5;          // Cut off threshold
    bool reset = true;              // Reset the anchor
    int n_anchors = -1;             // Set number of anchors

    auto tic = chrono::high_resolution_clock::now();

    while ((c = getopt(argc, argv, "k:r:t:a:m:n:")) != -1)
    {
        switch (c)
        {
            case 'n':
                try {
                    n_anchors = stoi(optarg);
                }
                catch (exception &err) {
                    std::cout << "Invalid n argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Nº of Anchors = " << reset << std::endl;
                break;
            case 'm':
                try {
                    min_size = stoi(optarg);
                }
                catch (exception &err) {
                    std::cout << "Invalid m argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Min Size = " << min_size << std::endl;
                break;
            case 'a':
                try {
                    alpha = stof(optarg);
                }
                catch (exception &err) {
                    std::cout << "Invalid a argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Alpha = " << alpha << std::endl;
                break;
            case 't':
                try {
                    threshold = stof(optarg);
                }
                catch (exception &err) {
                    std::cout << "Invalid t argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Threshold = " << threshold << std::endl;
                break;
            case 'r':
                try {
                    reset = stoi(optarg);
                }
                catch (exception &err) {
                    std::cout << "Invalid r argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Reset = " << reset << std::endl;
                break;
            case 'k':
                try {
                    k = stoi(optarg);
                }
                catch (exception &err) {
                    std::cout << "Invalid K argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "K = " << k << std::endl;
                break;
            
            case '?':
                std::cout << "Got unknown option." << std::endl; 
                break;
            default:
                std::cout << "Got unknown parse returns: " << c << std::endl;
                return EXIT_FAILURE;
        }
    }
    unordered_set<char> alphabet;
    string filename = argv[optind];

    int fsize = get_alphabet(filename, alphabet);
    int asize = 0;
    for (char c: alphabet){
        cout << asize << " " << (c) << endl;
        global_info.insert(make_pair(c, 0));
        asize++;
    }
    cout << "Alphabet size: " << asize << endl;
    // Create a text string, which is used to output the text file
    char byte = 0;

    string sequence = "";
    string full_seq = "";


    // Create and open a text file
    ifstream input_file(filename);
    int i = 0;
    int n_symbols = 0;
    map<string, vector<Anchor>> dict;
    int offset = 1;
    string testing_seq;

    double sum_acc = 0;
    int count_copies = 0;

    // Use a while loop together with the getline() function to read the file line by line
    cout << endl;
    int not_copy = 0;

    vector<Anchor> testing_anchors;

    while (input_file.get(byte)) {
        if (n_symbols%1000 == 0) {
           cout << n_symbols << " out of " << fsize-1 << ", " << ((float)n_symbols) / ((float)fsize-1) * 100 << "%" << '\r'; 
        } 
        n_symbols++;
        // Test sequence accuracy 
        if (testing_seq.length() > 0) {
            bool has_active = false;
            int active_count = 0;                                                           // Count the number of anchor active
            for (Anchor& s : testing_anchors) {
                if (!s.is_active) {
                    continue;
                }

                active_count++;
                s.length++;
                char predicted_symb = full_seq[s.position + offset];                        // Next symbol in the anchor sequence
                double p_curr_symb = estimate_probability(s.hits, s.misses, alpha, asize);  // Probability of the predicted symbol
                double p_rest_symb = (1- p_curr_symb)/(asize-1);                            // Probability of the not prediceted symbols

                // Update the information of each symbol according to the probabilities of the anchor
                for(auto it = s.info.begin(); it != s.info.end(); ++it) {
                    if(it->first == predicted_symb) {
                        it->second += -log2(p_curr_symb)*p_curr_symb;
                    }
                    else {
                        it->second += -log2(p_rest_symb)*p_rest_symb;
                    }
                }

                // Update the hit/miss ratio
                if (predicted_symb == byte) {
                    s.hits++;
                }
                else {
                    s.misses++;
                }

                // Check if the anchor has a hit/miss ratio that passes the threshold
                if (offset > min_size) {
                    float acc = (float) s.hits / (float) (s.hits + s.misses);
                    if (acc < threshold) {
                        s.is_active = false;
                        active_count--;
                    }
                }
            }
            offset++;
            if (active_count < 1) {
                float max_acc = 0;
                int max_id = 0;
                int max_len = 0;
                map<char, float> best_info;
                //cout << testing_seq << ":" << endl;
                for(Anchor& s: testing_anchors) {
                    if (s.length > 0){
                        int hits = s.hits;
                        int misses = s.misses;
                        double ps = estimate_probability(hits, misses, alpha, asize);
                        //cout << "\t" << s.position - k << " " << hits << " " << misses << " " << ps << " " << -log2(ps) << endl;

                        if(ps > max_acc && s.length >= max_len) {
                            max_acc = ps;
                            max_id = i;
                            best_info = s.info;
                            max_len = s.length;
                        }
                        // Reset Anchor
                        s.hits = 0;
                        s.misses = 0;
                        s.length = 0;
                        for(auto it = s.info.begin(); it != s.info.end(); ++it) {
                        //it->second = 0;
                        }
                    }
                }
                testing_anchors.clear();
                // cout << testing_seq << ":" << endl;
                // cout << "\t" << "Acc: " << max_acc << endl;
                // cout << "\t" << "length:" << max_len << endl; 
                if (max_acc > threshold-0.1) {
                    sum_acc += max_acc;
                    count_copies++;
                    // cout << "\t" << "Info:" << endl; 
                    for (auto it = global_info.begin(); it != global_info.end(); ++it) {
                        auto it2 = best_info.find(it->first);
                        if (it2 != best_info.end()) {
                            // Update the value of the pair
                            it->second += it2->second;
                            // cout << "\t\t" << it->first << ": " << it->second/max_len << endl; 
                        }
                    }
                }
                else {
                    not_copy += max_len;
                    for (auto it = global_info.begin(); it != global_info.end(); ++it) {
                        it->second += max_len*-log2((float)1/asize)*(float)1/asize;
                    }
                }
                
                offset = 1;
                testing_seq = "";
            }
        }
        else {
            not_copy++;
            //cout << "A: " <<  asize << endl;
            for (auto it = global_info.begin(); it != global_info.end(); ++it) {
                //cout << "1 " << it->second << " " << 1/asize << " " << -log2(1/asize) << endl;
                it->second += -log2((float)1/asize)*(float)1/asize;
                //cout << "2 " << it->second << endl;
            }
        }

        if (sequence.length() == k) {
            sequence.erase(0, 1);
        }

        sequence += byte;
        full_seq += byte;

        if (sequence.length()==k){
            if (testing_seq.length() == 0 && full_seq.length() > k && dict.count(sequence) > 0) {
                testing_seq = sequence;
                int i = 0;
                auto it = dict.find(testing_seq);
                if (it != dict.end()) {
                    // Iterate through the vector in reverse order
                    std::vector<Anchor>& myVec = it->second;
                    for (auto rit = myVec.rbegin(); rit != myVec.rend(); ++rit) {
                        // Update the value of each element in the struct
                        if (i == n_anchors) {
                            break;
                        }
                        rit->is_active = true;
                        testing_anchors.push_back(*rit);
                        i++;
                    }
                }
            }

            // Save the new Anchor
            Anchor new_seq;
            new_seq.position = i;
            for (char c: alphabet) {
                new_seq.info.insert(make_pair(c, 0));
            }
            if (dict.count(sequence) == 0) {
                vector<Anchor> v;
                dict.insert( make_pair(sequence, v) );
            }
            dict[sequence].push_back(new_seq);
        }

        i++;
    }

    cout << endl;
    map<char, float>::iterator itr;
    double sum_info = 0;
    cout << "Size of the file: " << fsize << endl;
    cout << "Number of symbols codified in copy: " << fsize - not_copy << endl;
    cout << "Number of symbols codified outside copy: " << not_copy << endl;
    for (itr = global_info.begin(); itr != global_info.end(); itr++) {
        cout << itr->first << ": " << itr->second/fsize << endl;
        sum_info += itr->second;
    }
    cout << "Average amount of information per symbol: " << sum_info/(fsize*asize) << endl;

    cout << "Nº copies: " << count_copies << endl;

    cout << "Average Copy Model Accuracy: " << sum_acc / count_copies << endl;

    // Close the file
    input_file.close(); 

    auto toc = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::seconds>(toc - tic);

    // Displaying the elapsed time
    cout << "Elapsed Time: " << duration.count() << endl;

    return EXIT_SUCCESS;
}

static double estimate_probability(int hits, int misses, double alpha, int asize) {
    return (hits + alpha)/(hits+misses+asize*alpha);
}

static int get_alphabet(string filename, unordered_set<char> &alphabet) {
    char byte = 0;

    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return - 1;
    }
    int count = 0;
    while (input_file.get(byte)) {
        alphabet.insert(byte);
        count++;
    }
    input_file.close();
    return count;
}