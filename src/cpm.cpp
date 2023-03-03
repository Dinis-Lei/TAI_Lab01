#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <bits/stdc++.h>

using namespace std;




void get_alphabet(string filename) {
    unordered_set<char> alphabet;

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

    unordered_set<char> :: iterator itr;
    for (itr = alphabet.begin(); itr != alphabet.end(); itr++){
        cout << (*itr) << endl;
    }

}







int main(int argc, char** argv) {

    int c;
    int k = 0;

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

    get_alphabet(argv[optind]);

}