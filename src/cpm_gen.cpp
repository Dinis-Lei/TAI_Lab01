#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <time.h>

using namespace std;

int main(int argc, char** argv) {

    map<string,  map<string,  float > > dict;   // sequence -> <symbol, probability>
    int c;                                // for getopt
    int n = 100;                          // number of generated symbols
    int k = 4;                            // length of the sequence

    while ((c = getopt(argc, argv, "n:k:")) != -1)
    {
        switch (c)
        {
            case 'k':
            {
                try
                {
                    k = stoi(optarg);
                }
                catch (exception &err)
                {
                    std::cout << "Invalid k argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "K: " << k << std::endl;
                break;
            }
            case 'n':
                try
                {
                    n = stoi(optarg);
                }
                catch (exception &err)
                {
                    std::cout << "Invalid n argument" << std::endl;
                    return EXIT_FAILURE;
                }
                std::cout << "Number of generated symbols: " << n << std::endl;
                break;
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
    string filename =  argv[optind];
    if (filename==""){
        std::cout << "Invalid output file path argument" << std::endl;
        return 1;
    }
    ifstream example(filename);
    char byte = 0;

    string sequence = "";
    
    int i =0;
    while (example.get(byte)) {
        // Output the text from the file
        string nextChar = "";
        nextChar+= byte;

        if (sequence.length()==k){
            if (dict.count(sequence) == 0){
                dict.insert( make_pair(sequence, map<string,  float >()) );
                dict[sequence].insert( make_pair("total", 0 ));
            }
            if (dict[sequence].count(nextChar) == 0){
                dict[sequence].insert(make_pair(nextChar, 0));
            }
            dict[sequence][nextChar]++;
            dict[sequence]["total"]++;

            sequence.erase(0, 1);

        }

        sequence+=nextChar;
        i++;
    }
    std::cout << "Probs" << std::endl;

    for(std::map<string,map<string,float>>::iterator it = dict.begin(); it != dict.end(); ++it) {
        map<string,float> map2 = it->second;
        string key = it->first; 
        for(std::map<string,float>::iterator it2 =map2.begin(); it2 != map2.end(); ++it) {
            dict[key][it2->first] = (it2->second) / dict[key]["total"];
        }
        dict[key].erase("total");
    }
    std::cout << "Print" << std::endl;

    for(std::map<string,map<string,float>>::iterator it = dict.begin(); it != dict.end(); ++it) {
        map<string,float> map2 = it->second;
        string key = it->first; 
        std::cout << key <<  ":"  << std::endl; 
        for(std::map<string,float>::iterator it2 =map2.begin(); it2 != map2.end(); ++it) {
            std::cout << "  -> "<< it2->second << std::endl; 

        }
    }
    std::cout << "End" << std::endl;


    // Read data cpm output file and get dictionary with the sequence -> most likely symbol
    // string line;
    // while (getline(example  , line)) { 
    //     // Print the data of the string.
    //     size_t pos = 0;
    //     string sequence;
    //     string simbol;
    //     string delimiter = ":";
    //     while ((pos = line.find(":")) != std::string::npos) {
    //         sequence = line.substr(0, pos);
    //         line.erase(0, pos + delimiter.length()+1);
    //         pos = 0;
    //         k = sequence.length();
    //         break;
    //     }
    //     std::cout << sequence << ":"<< line << "|" << std::endl;

    //     dict.insert( make_pair(sequence, stof(line)));  

    // }
    
    // Close the file object.
    example.close(); 

    



    // fclose(example);
    
    // srand( (unsigned)time( NULL ) );
    // for(int i=0; i<n;i++){
    //     float total = 0.0;
    //     float rand_val = (float) rand()/RAND_MAX;
    //     std::cout << rand_val << std::endl;
    //     for(std::map<string,float>::iterator it = dict.begin(); it != dict.end(); ++it) {
    //         total += it-> second;
    //         if(rand_val < total ){
    //             example += it-> first;
    //             break;
    //         }
    //     }

    // }


    // std::cout << "outp  ut: " << example << std::endl;



    // Close the file
    // fclose(example);

}