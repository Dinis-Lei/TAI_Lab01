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

    int c;
    string o;
    fstream cpm_output;

    while ((c = getopt(argc, argv, "o:")) != -1)
    {
        switch (c)
        {
            case 'o':
            {
                // try {
                //     o = optarg;
                // }
                // catch (exception &err) {
                //     std::cout << "Invalid o argument" << std::endl;
                //     return 1;
                // }
                cpm_output.open(o,ios::in);

                if (!cpm_output.is_open()){
                    std::cout << "Invalid output file path argument" << std::endl;
                    return 1;
                }
                std::cout << "o = " << o << std::endl;
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


    map<string, map<string, float > > dict;

    // Create a text string, which is used to output the text file

    string sequence = "";
    const char * filename = argv[optind];
    char k; 

    // Create and open a text file
    FILE *example = fopen(filename,"r");

    if(example== NULL){
        std::cout << "Invalid example file path argument" << std::endl;
        return 1;
    }


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
            std::cout << sequence << std::endl;
            line.erase(0, pos + delimiter.length());
            pos = 0;
            k=sequence.length();
            break;
        }
        delimiter = "-";

        dict.insert( make_pair(sequence, map<string, float >()) );
        std::cout << line << std::endl;
        while ((pos = line.find("-")) != std::string::npos) {
            simbol = line.substr(0, pos);
            std::cout << simbol << std::endl;
            line.erase(0, pos + delimiter.length());
            pos = 0;
            int pos2=0;
            delimiter = ",";
            while ((pos2 = line.find(",")) != std::string::npos) {
                string prob = line.substr(0, pos2);
                dict[sequence].insert( make_pair(simbol, stof(prob)));  
                pos2=0;
                break;
            }
        }

    }
    
    // Close the file object.
    cpm_output.close(); 

    
    // for(map<string, map<string, float > >::const_iterator it = dict.begin();
    // it != dict.end(); ++it)
    // {
    //     std::cout << it->first << " : "  << it->second << "\n";

    // }
    char ch;
    string sequence = "";
    fseek(example, 0, SEEK_END);
    long fileLength = ftell(example);
    fseek(example, (fileLength - k), SEEK_SET);
    
    do {
        ch = fgetc(example);
        if (ch != EOF){
            sequence += ch; 
        }
        

    } while (ch != EOF);


    fclose(example);
    std::ofstream outfile;

    outfile.open(filename, std::ios_base::app); // append instead of overwrite

    for(int i=0; i<10;i++){
        

        outfile << "Data"; 
    }

    srand( (unsigned)time( NULL ) );
    float rand_val = (float) rand()/RAND_MAX;




    // Close the file
    fclose(example);

}