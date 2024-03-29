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

int main(int argc, char **argv)
{
    //create list with integers

    unordered_map<string, vector<int>> dict; // sequence -> <symbol, probability>
    int n = 100;                                              // number of generated symbols
    int k = 4;                                                // length of the sequence
    string filename;
    string sample = "";

    unordered_map<char, float> alphabet;

    unordered_map<string, unordered_map<string, float>> dictFinal; // sequence -> <symbol, probability>



    // Process command line arguments
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-k")
        {
            i++;
            if (i >= argc)
            {
                cerr << "Invalid k argument" << endl;
                return EXIT_FAILURE;
            }
            try
            {
                k = stoi(argv[i]);
            }
            catch (exception &e)
            {
                cerr << "Invalid k argument: " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-n")
        {
            i++;
            if (i >= argc)
            {
                cerr << "Invalid n argument" << endl;
                return EXIT_FAILURE;
            }
            try
            {
                n = stoi(argv[i]);
            }
            catch (exception &e)
            {
                cerr << "Invalid n argument: " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-s")
        {
            i++;
            if (i >= argc)
            {
                cerr << "Invalid s argument" << endl;
                return EXIT_FAILURE;
            }
            try
            {
                sample = argv[i];
            }
            catch (exception &e)
            {
                cerr << "Invalid s argument: " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        }
        else if (filename.empty())
        {
            filename = arg;
        }
        else
        {
            cerr << "Unknown argument: " << arg << endl;
            return EXIT_FAILURE;
        }
    }

    if (filename.empty())
    {
        cerr << "No input file specified" << endl;
        return EXIT_FAILURE;
    }

    // Open the input file
    ifstream infile(filename);
    if (!infile.is_open())
    {
        cerr << "Failed to open input file: " << filename << endl;
        return EXIT_FAILURE;
    }

    // Read the input file
    string sequence = "";
    char c;
    int Index = 0;
    while (infile.get(c))
    {
        alphabet[c]++;

        if (sequence.length() == k)
        {
            //append to the vector of the [sequence] the index of the next char
            dict[sequence].push_back(Index);
            sequence.erase(0, 1);
        }
        Index++;
        sequence += c;
    }

    //Compute probabilities for each symbol of the alphabet
    for (auto &[symbol, count] : alphabet)
    {
        count /= Index;
    }


    // Generate output
    string output;
    if (sample.empty())
    {
        sample = sequence.substr(0, k);
    }
    else
    {
        for (const char &c : sample)
        {
            auto it = alphabet.find(c);
            if (it == alphabet.end())
            {
                cerr << "Wrong sample provided: " << sample << endl;
                return EXIT_FAILURE;
            }
        }
    }

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    output.reserve(n);

    // Precompute cumulative probabilities for the alphabet
    std::vector<std::pair<char, float>> cum_prob;
    cum_prob.reserve(alphabet.size());
    float total_prob = 0.0f;
    output+=sample;
    for (auto &[symbol, prob] : alphabet)
    {
        total_prob += prob;
        cum_prob.emplace_back(symbol, total_prob);
    }


    //opening the file again to reset the pointer
    infile.close(); // Close file

    infile.open(filename); // Open file


    if (!infile.is_open()) {
        std::cerr << "Failed to open file\n";
        return 1;
    }


    for (int i = 0; i < n; i++)
    {
        float rand_val = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
        if (dict.count(sample) == 0)
        {
            // Use binary search to find the next symbol
            auto it = std::lower_bound(cum_prob.begin(), cum_prob.end(), rand_val,
                                       [](const std::pair<char, float> &lhs, const float &rhs)
                                       {
                                           return lhs.second < rhs;
                                       });

            output += it->first;
            sample.push_back(it->first);
            sample.erase(0, 1);
        }
        else
        {
            const auto &next_chars = dict[sample];
            // Use const reference to avoid copying
            
            //Calculates the probability of each symbol appearing after the sequence
            //Checks if the next char is in the map
            if (dictFinal.count(sample) == 0){
                for (int e = 0; e < next_chars.size(); e++){
                    //convert seekg to string
                    char nchar;
                    infile.seekg(next_chars[e]);
                    infile.get(nchar);
                    dictFinal[sample][string(1, nchar)]++;
                    dictFinal[sample]["total"]++;  // add 1 to the total count

                }
                for (auto &[nextChar, count] : dictFinal[sample])
                {
                    if (nextChar != "total")
                    {
                        dictFinal[sample][nextChar] /= dictFinal[sample]["total"];
                    }
                }
                dictFinal[sample].erase("total");
            }

            // Calculates the next character
            float total_prob = 0.0f;
            for (auto &[next_char, prob] : dictFinal[sample])
            {
                total_prob += prob;
                if (rand_val < total_prob)
                {
                    output += next_char;
                    sample += next_char;
                    sample.erase(0, 1);
                    break;
                }
            }
        }
    }
    cout << output << endl;
    return 0;
}
