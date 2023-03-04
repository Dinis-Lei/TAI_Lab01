#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>

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


    map<string, vector<int> > dict;

    // Create a text string, which is used to output the text file
    char byte = 0;

    string sequence = "";


    // Create and open a text file
    // cout << k << "\n";
    ifstream input_file("./example/example2.txt");

    int i =0;
    // Use a while loop together with the getline() function to read the file line by line
    while (input_file.get(byte)) {
        // Output the text from the file
        sequence+=byte;


        if (sequence.length()==k){
            if (dict.count(sequence) == 0){
                dict.insert( make_pair(sequence, vector<int>()) );
            }
            dict[sequence].push_back(i+1);
            // cout << i+k << "\n";
            // cout << sequence << "\n";
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

    for(map<string, vector<int> >::const_iterator it = dict.begin();
    it != dict.end(); ++it)
    {
        std::cout << it->first << " : " ;

        for (int index: it->second){
            if (index != 0)
                std::cout  << index << " " ;

         }

        std::cout << "\n";
    }

    // Close the file
    input_file.close(); 

}