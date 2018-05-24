//
//  input_parser.cpp
//  SchedulingAndDeadlockAvoidance
//
//  Created by Ziyao Yan on 5/23/18.
//

#include "input_parser.hpp"

using namespace std;

//constructor
Input_Parser(string filename) {
    int number_of_lines = 0;
    string line;
    ifstream fh(filename); // reading in input file.
    while (getline(myfile, line)) {
        //reading in number of lines in text file.
        ++number_of_lines;
        lines = parse(line);
        //doing some shit.
        
    }
}
