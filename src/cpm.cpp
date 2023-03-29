#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;


struct Anchor {
    bool is_active = false;                         // Check if anchor is being used or not
    int position;                                   // Where the anchor is situated in the full sequence
    int hits = 0;                                   // Counter of hits
    int misses = 0;                                 // Counter of misses
    int length = 0;                                 // How many symbols has the anchor tested
    unordered_map<char, double> sum_info_per_symbol;          // Sum of all information gained for each symbol
    double sum_info = 0;                            // Sum of all information gained by all the symbols  
    vector<double> info_per_byte;                   // Stores the history of information gained by each symbol encoded
};

/**
 * @brief Extracts the alphabet from the file and counts the size of the file
 * @param filename      Name of the file to extract the file
 * @param alphabet      Set where alphabet will be saved
 * @param symbol_count  Saves the frequency of each symbol
 * @return              Size of the file
 */
static int get_alphabet(string filename, unordered_set<char> &alphabet, unordered_map<char, int> &symbol_count);

/**
 * @brief Calculates the probability of a given copy model guess the next symbol correctly based
 *        on the number of times it has hit or failed.
 * @param hits      Number of hits
 * @param misses    Number of misses
 * @param alpha     Smoothing paramether
 * @return          Probability of the next symbol  
 */
static double estimate_probability(int hits, int misses, double alpha);

/**
 * @brief Reads the command line for arguments and sets the correct hyper parameters.
 * @param argc Number of arguments passed
 * @param argv Arguments passed
 */
static void parse_command_line(int argc, char** argv);

/**
 * @brief Selects the anchor which encoded more symbols
 * @param anchors       Array of anchors to be evaluated
 * @param symbol        Current symbol being encoded
 * @param ignored_last  Tells if the last symbol was encoded by the copy model or not
 */
static void select_best_anchor(vector<Anchor> &anchors, char symbol, bool ignored_last);

/**
 * @brief Tests and updates the accuracy of each active anchor and disables when accuracy drops below a thresshold
 * @param symbol        Current symbol being encoded
 * @param offset        Offset of symbols to skip
 * @param full_seq      Reference data for the anchors
 * @param ignored_last  Tells if the last symbol was encoded by the copy model or not
 * @param anchors       Array of anchors to be evaluated
 * @return              True if no anchor is active
 */
static bool update_anchors(char symbol, int offset, bool &ignored_last, vector<Anchor> &anchors);

unordered_map<char, float> global_info;       // table with symbol - amount of information 

/**
 * Hyper Parameters
*/

int k = 4;                      // Size of the window and anchors
float max_size = INFINITY;      // Maximum number of symbols for the anchor to evaluate
int min_size = 0;               // Minimum number of symbols for the anchor to evaluate
float alpha = 1;                // Smoothing
float threshold = 0.5;          // Cut off threshold
bool save = false;              // Save the information gain in an file
int n_anchors = -1;             // Set number of anchors, -1 means all anchors will be used
bool ignore1 = false;


double sum_acc = 0;             // Sum of accuracies of each chosen anchor of the copy model
int count_copies = 0;
int sum_len_copies = 0;

int not_copy = 0;
int copies = 0;
double accumulative_info = 0;

int asize;
int default_symb_size;
vector<double> info_per_byte;

vector<char> full_seq;                                                    // Full sequence of symbols to be encoded

int main(int argc, char** argv) {

    auto tic = chrono::high_resolution_clock::now();

    parse_command_line(argc, argv);

    unordered_set<char> alphabet;
    string filename = argv[optind];
    unordered_map<char, int> symbol_count;                                        // Frequency of each symbol

    int fsize = get_alphabet(filename, alphabet, symbol_count);         // Size of the file (number of symbols)
    asize = alphabet.size();                                            // Size of the alphabet
    for (char c: alphabet){
        cout << asize << " " << (c) << endl;
        global_info.insert(make_pair(c, 0));
    }
    default_symb_size = ceil(log2(asize));                              // Nº bits needed to represent a symbol if it is not encoded by the copy model
    cout << "Alphabet size: " << asize << endl;

    string window;                                                      // Window of symbolsto match with an anchor
    string testing_seq;                                                 // Sequence of symbols of the anchors being tested
    unordered_map<string, vector<int>> positions;                                 // Table with sequence - array of occurences in the full sequence
    vector<Anchor> anchors;                                             // Anchors to be tested

    char symbol = 0;                                                        
    int n_symbols = 0;                                                      
    int offset = 1;                                                         

    cout << endl;
    ifstream input_file(filename);
    // Main loop
    while (input_file.get(symbol)) {
        if (n_symbols%1000 == 0) {
           cout << n_symbols << " out of " << fsize-1 << ", " << ((float)n_symbols) / ((float)fsize-1) * 100 << "%" << '\r'; 
        } 

        if (testing_seq.length() > 0) {
            bool ignored_last = false;              // If current symbol should be ignored by the copy model and be encoded normally
            //cout << testing_seq << endl;
            //update_anchors(symbol, offset, full_seq, ignored_last, anchors)

            if (update_anchors(symbol, offset, ignored_last, anchors)) {
                select_best_anchor(anchors, symbol, ignored_last);
                offset = 0;
                testing_seq = "";
            }
            offset++;
        }
        else {
            not_copy++;
            accumulative_info += default_symb_size;
            global_info[symbol] += default_symb_size;
            info_per_byte.push_back(default_symb_size);
        }

        if (window.length() == k) {
            window.erase(0, 1);
        }

        window += symbol;
        full_seq.push_back(symbol);

        if (window.length()==k){
            // Check if there is no candidate sequence and it has appeared more than once
            if (testing_seq.length() == 0 && full_seq.size() > k && positions.count(window) > 0) {
                testing_seq = window;
                auto it = positions.find(testing_seq);
                if (positions.find(testing_seq) != positions.end()) {
                    // Iterate through the vector in reverse order
                    vector<int> position = it->second;
                    // Save the n-th most recent anchors
                    int i = 0;
                    for (auto rit = position.rbegin(); rit != position.rend(); ++rit) {
                        // Update the value of each element in the struct
                        if (i == n_anchors) {
                            break;
                        }
                        // Initialize a new anchor
                        Anchor new_anchor;
                        new_anchor.position = *rit;
                        new_anchor.is_active = true;
                        for (char c: alphabet) {
                            new_anchor.sum_info_per_symbol.insert(make_pair(c, 0));
                        }
                        anchors.push_back(new_anchor);
                        i++;
                    }
                }
            }

            if (positions.count(window) == 0) {
                vector<int> v;
                positions.insert( make_pair(window, v) );
            }
            positions[window].push_back(n_symbols);
        }

        n_symbols++;
    }

    if (testing_seq.length() > 0) {
        int active_count = 0;
        for (Anchor& anchor: anchors) {
            if (anchor.is_active && anchor.length > 0) {
                active_count++;
                break;
            }
        }
        if (active_count > 0)
            select_best_anchor(anchors, symbol, false);
    }

    cout << endl;
    cout << "Size of the file: " << fsize << endl;
    cout << "Number of symbols codified outside copy: " << not_copy << endl;
    cout << "Number of symbols codified inside  copy: " << copies << endl;

    double sum_info = 0;
    unordered_map<char, float>::iterator itr;
    for (itr = global_info.begin(); itr != global_info.end(); itr++) {
        cout << itr->first << ": " << itr->second/symbol_count[itr->first] << endl;
        sum_info += itr->second;
    }

    cout << "Accumulation of information: " << accumulative_info << endl;
    cout << "Average amount of information per symbol: " << accumulative_info/(fsize) << endl;
    cout << "Nº copies: " << count_copies << endl;
    cout << "Average length of copies: " << (float) sum_len_copies / (float) count_copies << endl;
    cout << "Average Copy Model Accuracy: " << sum_acc / count_copies << endl;

    // cout << info_per_byte.size() << " " << fsize << endl;

    if (save) {
        fstream file;
        file.open("vector_file.txt",ios_base::out);
        for(int i=0; i < info_per_byte.size(); i++) {
            file<<info_per_byte[i]<<endl;
        }
        file.close();
    }
    
    // Close the file
    input_file.close(); 

    auto toc = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::seconds>(toc - tic);

    // Displaying the elapsed time
    cout << "Elapsed Time: " << duration.count() << endl;

    return EXIT_SUCCESS;
}

static double estimate_probability(int hits, int misses, double alpha) {
    return (hits + alpha)/(hits+misses+2*alpha);
}

static int get_alphabet(string filename, unordered_set<char> &alphabet, unordered_map<char, int> &symbol_count) {
    char byte = 0;

    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        exit(EXIT_FAILURE);
    }
    int count = 0;
    while (input_file.get(byte)) {
        alphabet.insert(byte);                                      
        if (symbol_count.find(byte) == symbol_count.end()) {        
            symbol_count.insert(pair<char, int>(byte, 0));
        }
        symbol_count[byte]++;
        count++;                                                                    
    }
    input_file.close();
    return count;
}

static bool update_anchors(char symbol, int offset, bool &ignored_last, vector<Anchor> &anchors) {

    int active_count = 0;                                          // Count the number of anchor active

    for (Anchor& anchor : anchors) {
        if (!anchor.is_active) {
            continue;
        }
        active_count++;
        anchor.length++;
        char predicted_symb = full_seq[anchor.position + offset];                      // Next symbol in the anchor sequence
        double p_curr_symb;
        p_curr_symb = estimate_probability(anchor.hits, anchor.misses, alpha);  // Probability of the predicted symbol

        double info;
        if (predicted_symb == symbol) {
            info = -log2(p_curr_symb);
            anchor.hits++;
        }
        else {
            double prob = (double)(1 - p_curr_symb)/ (double)(asize - 1);
            info =  -log2(prob);
            anchor.misses++;
        }

        anchor.sum_info += info;
        if (info < 0) {
            cerr << "NEGATIVE INFO GAIN??" << endl;
            cout << "!!!! INFO: " << info << " " << anchor.hits << " " << anchor.misses << endl;
            exit(EXIT_FAILURE);
        }
        anchor.sum_info_per_symbol[predicted_symb] += info;
        anchor.info_per_byte.emplace_back(info);

        if (anchor.length > min_size) {
            // Check if the anchor has a hit/miss ratio that passes the threshold
            double acc = estimate_probability(anchor.hits, anchor.misses, alpha);
            // cout << acc << " " << threshold << endl;
            if (acc < threshold) {
                anchor.is_active = false;
                active_count--;
                // Remove the last guess
                if (ignore1) {
                    anchor.misses--;
                    anchor.length--;
                    anchor.info_per_byte.pop_back();
                    anchor.sum_info_per_symbol[predicted_symb] -= info;
                    anchor.sum_info -= info;
                    ignored_last = true; 
                }
                
            }
        }

        // Deactivate anchor because of max_size
        if (anchor.length >= max_size) {
            anchor.is_active = false;
            active_count--;
        }
    }

    return active_count == 0;
}

static void select_best_anchor(vector<Anchor> &anchors, char byte, bool ignored_last) {
    double max_acc = 0;                     // best accuracy
    int max_len = 0;                        // longest sequence of symbols encoded
    double best_accumulative_info = 0;      // accumulative information gain for the best sequence
    unordered_map<char, double> best_info;            // information gain of each symbol for the best sequence
    vector<double> best_info_per_byte;
    for(Anchor& anchor: anchors) {
        if (anchor.length < 1) {
            continue;
        }
        int hits = anchor.hits;
        int misses = anchor.misses;
        double ps = estimate_probability(hits, misses, alpha);
        // Select the sequence who encode the most and with better accuracy
        if (anchor.length > max_len) {
            max_acc = ps;
            best_info = anchor.sum_info_per_symbol;
            max_len = anchor.length;
            best_accumulative_info = anchor.sum_info;
            best_info_per_byte = anchor.info_per_byte;
        }
        else if (anchor.length == max_len && ps > max_acc) {
            max_acc = ps;
            best_info = anchor.sum_info_per_symbol;
            max_len = anchor.length;
            best_accumulative_info = anchor.sum_info;
            best_info_per_byte = anchor.info_per_byte;
        }
    }

    // Anchor encoded some bytes
    if (max_len > 0) {
        sum_acc += max_acc;
        sum_len_copies += max_len;
        count_copies++;
        accumulative_info += best_accumulative_info;
        copies += max_len;
        for (const auto &it : best_info) {
            auto it2 = global_info.find(it.first);
            if (it2 != global_info.end()) {
                it2->second += it.second;
            } 
            else {
                global_info.emplace_hint(it2, it.first, it.second);
            }
        }
        info_per_byte.insert(info_per_byte.end(), best_info_per_byte.begin(), best_info_per_byte.end());

        // Update 
        if (ignored_last) {
            info_per_byte.push_back(default_symb_size);
            global_info[byte] += default_symb_size;
            accumulative_info += default_symb_size;
            not_copy++;        
        }
    }
    else {
        not_copy++;
        accumulative_info += default_symb_size;
        global_info[byte] += default_symb_size;
        info_per_byte.push_back(default_symb_size);
    }

    anchors.clear();
}

static void parse_command_line(int argc, char** argv) {
    int c;                          // Opt process
    while ((c = getopt(argc, argv, "k:st:a:m:M:n:i")) != -1) {
        switch (c)
        {
            case 'i':
                ignore1 = true;
                break;
            case 'n':
                try {
                    n_anchors = stoi(optarg);
                }
                catch (exception &err) {
                    cout << "Invalid n argument" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'M':
                try {
                    max_size = stoi(optarg);
                }
                catch (exception &err) {
                    cout << "Invalid m argument" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':
                try {
                    min_size = stoi(optarg);
                }
                catch (exception &err) {
                    cout << "Invalid m argument" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'a':
                try {
                    alpha = stof(optarg);
                }
                catch (exception &err) {
                    cout << "Invalid a argument" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                try {
                    threshold = stof(optarg);
                }
                catch (exception &err) {
                    cout << "Invalid t argument" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                save = true;
                break;
            case 'k':
                try {
                    k = stoi(optarg);
                }
                catch (exception &err) {
                    cout << "Invalid K argument" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            
            case '?':
                cout << "Got unknown option." << std::endl; 
                break;
            default:
                cout << "Got unknown parse returns: " << c << std::endl;
                exit(EXIT_FAILURE);
        }
    }
    cout << "Nº of Anchors = " << n_anchors << std::endl;
    cout << "Max Size = " << max_size << std::endl;
    cout << "Min Size = " << min_size << std::endl;
    cout << "Alpha = " << alpha << std::endl;
    cout << "Threshold = " << threshold << std::endl;
    cout << "Save = " << save << std::endl;
    cout << "Ignore Last = " << ignore1 << std::endl;
    cout << "K = " << k << std::endl;
}