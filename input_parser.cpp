//
//  input_parser.cpp
//  SchedulingAndDeadlockAvoidance
//
//  Created by Ziyao Yan on 5/23/18.
//

#include "input_parser.hpp"

using namespace std;

//constructor
Input_Parse :: Input_Parser(string filename) {
    int number_of_lines = 0;
    string line;
    ifstream myfile(filename); // reading in input file.
    while (getline(myfile, line)) {
        //reading in number of lines in text file.
        lines.push_back(line);
    }
}

Input_Parser :: analyze() {
    for(string command : lines) {
        char command = (char)line[0];
        switch(command) {
            //System Configuration
            case 'C' : cout << '1'; // prints "1"
                break;       // and exits the switch
            //Job Arrival
            case 'A' : cout << '2';
                break;
            //Request for Devices
            case 'Q' : cout << '2';
                break;
            //Release for Devices
            case 'L' : cout << '2';
                break;
            //A display of the current system status in Readable format (with headings and properly aligned)
            case 'D' : cout << '2';
                break;
        }
    }
}
