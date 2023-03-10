#include <iostream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
using namespace std;

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

    std::ifstream file("../example/file.txt");  // Open file for reading
    std::map<string, std::vector<int>> dict;  // Create dictionary

    if (file.is_open()) {  // Check if file was opened successfully
        int position = 0;
        std::string mystring;
        file >> mystring;

        //queue where puts k elements and "screenshots" the queue to a hashmap
        queue<char> symbol;
        

        while (position < ) {  // Read file character by character
            dict[symbol[position:position+k]].push_back(position);  // Add position to symbol's list
            position++;
        }

        // Print dictionary
        for (auto it = dict.begin(); it != dict.end(); ++it) {
            std::cout << it->first << ": ";
            for (auto pos : it->second) {
                std::cout << pos << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    return 0;





}