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

    unordered_map<string, unordered_map<string, float>> dict; // sequence -> <symbol, probability>
    int n = 100;                                              // number of generated symbols
    int k = 4;                                                // length of the sequence
    string filename;
    string sample = "";

    unordered_map<char, float> alphabet;

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
    int numTotalChar = 0;
    while (infile.get(c))
    {
        alphabet[c]++;
        numTotalChar++;

        if (sequence.length() == k + 1)
        {
            dict[sequence]["total"]++;
            dict[sequence][string(1, c)]++;
            sequence.erase(0, 1);
        }
        sequence += c;
    }

    for (auto &[symbol, count] : alphabet)
    {
        count /= numTotalChar;
    }
    
    // Compute probabilities
    for (auto &[prefix, counts] : dict)
    {
        float total = counts["total"];
        for (auto &[next_char, count] : counts)
        {
            if (next_char != "total")
            {
                count /= total;
            }
        }
        counts.erase("total");
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

    // Precompute cumulative probabilities
    std::vector<std::pair<char, float>> cum_prob;
    cum_prob.reserve(alphabet.size());
    float total_prob = 0.0f;
    for (auto &[symbol, prob] : alphabet)
    {
        total_prob += prob;
        cum_prob.emplace_back(symbol, total_prob);
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
            // Use const reference to avoid copying
            const auto &next_chars = dict.at(sample);
            float total_prob = 0.0f;
            for (auto &[next_char, prob] : next_chars)
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
