#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>

using namespace std;


struct Sequence 
{
    bool is_active = false;
    int position;
    int hits = 0;
    int misses = 0;
};

static int get_alphabet(string filename, unordered_set<char> &alphabet);

static double estimate_probability(int hits, int misses, double alpha);

int main(int argc, char** argv) {

    int c;
    int k = 4;
    int min_size = 3;
    float alpha = 1;
    float threshold = 0.5;
    bool reset = true;

    while ((c = getopt(argc, argv, "k:r:t:a:m:")) != -1)
    {
        switch (c)
        {
            case 'm':
                try
                {
                    min_size = stoi(optarg);
                }
                catch (exception &err)
                {
                    std::cout << "Invalid m argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Min Size = " << min_size << std::endl;
                break;
            case 'a':
                try
                {
                    alpha = stoi(optarg);
                }
                catch (exception &err)
                {
                    std::cout << "Invalid a argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Alpha = " << alpha << std::endl;
                break;
            case 't':
                try
                {
                    threshold = stoi(optarg);
                }
                catch (exception &err)
                {
                    std::cout << "Invalid t argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Threshold = " << threshold << std::endl;
                break;
            case 'r':
                try
                {
                    reset = stoi(optarg);
                }
                catch (exception &err)
                {
                    std::cout << "Invalid r argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Reset = " << reset << std::endl;
                break;
            case 'k':
            {
                try
                {
                    k = stoi(optarg);
                }
                catch (exception &err)
                {
                    std::cout << "Invalid K argument" << std::endl;
                    return EXIT_FAILURE;
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

    int size = get_alphabet(filename, alphabet);
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
    ifstream input_file(filename);
    int i = 0;
    int h = 0;
    map<string, vector<Sequence>> dict;
    int offset = 1;
    string testing_seq;

    double sum_acc = 0;
    int count_copies = 0;

    // Use a while loop together with the getline() function to read the file line by line
    cout << endl;
    while (input_file.get(byte)) {
        if (h%1000 == 0) {
           cout << h << " out of " << size-1 << ", " << ((float)h) / ((float)size-1) * 100 << "%" << '\r'; 
        } 
        h++;
        // Test sequence accuracy 
        if (testing_seq.length() > 0) {
            bool has_active = false;
            int active_count = 0;
            for (Sequence& s : dict.find(testing_seq)->second)
            {
                if (!s.is_active)
                {
                    continue;
                }
                //cout << "T1: " <<  full_seq[s.position + offset] << " " << s.position << " " << s.hits << " " << s.misses << endl;
                active_count++;
                char predicted_byte = full_seq[s.position + offset];
                if (predicted_byte == byte)
                {
                    s.hits++;
                }
                else
                {
                    s.misses++;
                }

                if (offset > min_size)
                {
                    float acc = s.hits / (s.hits + s.misses);
                    if (acc < threshold)
                    {
                        s.is_active = false;
                        active_count--;
                    }
                }
            }
            offset++;
            if (active_count < 1)
            {
                float max_acc = 0;
                int max_id = 0;
                int i = 0;
                //cout << testing_seq << ":" << endl;
                for(Sequence& s: dict.find(testing_seq)->second) {
                    int hits = s.hits;
                    int misses = s.misses;
                    double ps = estimate_probability(hits, misses, alpha);
                    //cout << "\t" << s.position - k << " " << hits << " " << misses << " " << ps << " " << -log2(ps) << endl;

                    if(ps > max_acc) {
                        max_acc = ps;
                        max_id = i;
                    }
                    s.hits = 0;
                    s.misses = 0;
                    i++;
                }

                sum_acc += max_acc;
                count_copies += 1;

                offset = 1;
                testing_seq = "";
            }
        }

        if (sequence.length() == k) {
            sequence.erase(0, 1);
        }

        sequence += byte;
        full_seq += byte;

        // cout << "FSeq:" << full_seq << endl;
        // cout << "Test: " << testing_seq << endl;

        if (sequence.length()==k){
            if (testing_seq.length() == 0 && full_seq.length() > k && dict.count(sequence) > 0) {
                testing_seq = sequence;
                for(Sequence& s: dict.find(testing_seq)->second) {
                    s.is_active = true;
                }
            }

            // Save the new Sequence
            Sequence new_seq;
            new_seq.position = i;
            if (dict.count(sequence) == 0){
                vector<Sequence> v;
                dict.insert( make_pair(sequence, v) );
            }
            dict[sequence].push_back(new_seq);
        }

        i++;
    }

    // map<string, vector<Sequence>>::iterator it;
    // for(it = dict.begin(); it != dict.end(); it++) {
    //     cout << it-> first << ":" << endl;
    //     for (Sequence s: it->second) {
    //         int hits = s.hits;
    //         int misses = s.misses;
    //         double ps = estimate_probability(hits, misses, alpha);
    //         cout << "\t" << s.position-k << " " << hits << " " << misses << " " << ps << " " << -log2(ps) << endl;
    //     }
    // }
    cout << endl;
    cout << "Average Copy Model Accuracy: " << sum_acc / count_copies << endl;

    // Close the file
    input_file.close(); 

    return EXIT_SUCCESS;
}

static double estimate_probability(int hits, int misses, double alpha) {
    return (hits + alpha)/(hits+misses+2*alpha);
}

static int get_alphabet(string filename, unordered_set<char> &alphabet) {
    char byte = 0;

    ifstream input_file(filename);
    if (!input_file.is_open())
    {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return - 1;
    }
    int count = 0;
    while (input_file.get(byte))
    {
        alphabet.insert(byte);
        count++;
    }
    input_file.close();
    return count;
}