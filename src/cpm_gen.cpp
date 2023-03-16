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

    map<string,  float > dict;
    int c;
    int n = 100;                          // number of generated symbols
    int k;
    string filename="";

    while ((c = getopt(argc, argv, "n:o:")) != -1)
    {
        switch (c)
        {
            case 'o':
            {
                filename = optarg;
                std::cout << "o: " <<  optarg << "\n";
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
    if (filename==""){
        std::cout << "Invalid output file path argument" << std::endl;
        return 1;
    }
    ifstream cpm_output(filename);

    // map<string, map<string, float > > dict;

    // Create a text string, which is used to output the text file

    // const char * filename = argv[optind];
    // char k; 

    // Create and open a text file
    // FILE *example = fopen(filename,"r");

    // if(example== NULL){
    //     std::cout << "Invalid example file path argument" << std::endl;
    //     return 1;
    // }


    // Read data cpm output file and get dictionary with the sequence -> most likely symbol
    string line;
    while (getline(cpm_output, line)) { 
        // Print the data of the string.
        size_t pos = 0;
        string sequence;
        string simbol;
        string delimiter = ":";
        while ((pos = line.find(":")) != std::string::npos) {
            sequence = line.substr(0, pos);
            line.erase(0, pos + delimiter.length()+1);
            pos = 0;
            k = sequence.length();
            break;
        }
        std::cout << sequence << ":"<< line << "|" << std::endl;

        dict.insert( make_pair(sequence, stof(line)));  
        // delimiter = "-";

        // dict.insert( make_pair(sequence, map<string, float >()) );
        // std::cout << line << std::endl;
        // while ((pos = line.find("-")) != std::string::npos) {
        //     simbol = line.substr(0, pos);
        //     std::cout << simbol << std::endl;
        //     line.erase(0, pos + delimiter.length());
        //     pos = 0;
        //     int pos2=0;
        //     delimiter = ",";
        //     while ((pos2 = line.find(",")) != std::string::npos) {
        //         string prob = line.substr(0, pos2);
        //         dict[sequence].insert( make_pair(simbol, stof(prob)));  
        //         pos2=0;
        //         break;
        //     }
        // }

    }
    
    // Close the file object.
    cpm_output.close(); 

    
    // for(map<string, map<string, float > >::const_iterator it = dict.begin();
    // it != dict.end(); ++it)
    // {
    //     std::cout << it->first << " : "  << it->second << "\n";

    // }
    std::cout << "end File chapter 1"<< std::endl;
    // char ch;
    // string sequence = "";
    // fseek(example, 0, SEEK_END);
    // long fileLength = ftell(example);
    // fseek(example, (fileLength - k), SEEK_SET);
    
    // std::cout << "end File chapter 2"<< std::endl;
    // do {
    //     ch = fgetc(example);
    //     if (ch != EOF){
    //         sequence += ch; 
    //     }
        

    // } while (ch != EOF);


    // fclose(example);
    string example = argv[optind];
    
    srand( (unsigned)time( NULL ) );
    for(int i=0; i<n;i++){
        float total = 0.0;
        float rand_val = (float) rand()/RAND_MAX;
        std::cout << rand_val << std::endl;
        for(std::map<string,float>::iterator it = dict.begin(); it != dict.end(); ++it) {
            total += it-> second;
            if(rand_val < total ){
                example += it-> first;
                break;
            }
        }

    }


    std::cout << "output: " << example<< std::endl;



    // Close the file
    // fclose(example);

}