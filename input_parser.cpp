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
        char  option = (char)line[0];
        switch(option) {
            case 'C' :
                int time, mainMemory, numOfSerialDevice, timeQuantum;
                createJob();
            case 'A' :
                int jobNo,mainMemory, maxDemand, unitOfTime, priority;
                createJob();
            case 'Q' :
                int time, jobNo;
                createJob();
            case 'L' :
                int time, jobNo, noOfDevices;
                createJob();
            case 'D' :
                int time;
                createJob();
            default :
                cout<<command<<" is not valid."<<endl;
        }
    }
    
    Job* createJob(vector<string> split_line){
        int time, jobNo, memory, maxDemand, runTime, priority;
        istringstream(split_line[1]) >> time;
        istringstream(split_line[2].substr(2)) >> jobNo;
        istringstream(split_line[3].substr(2)) >> memory;
        istringstream(split_line[4].substr(2)) >> maxDemand;
        istringstream(split_line[5].substr(2)) >> runTime;
        istringstream(split_line[6].substr(2)) >> priority;
        return new Job(time, jobNo ,memory,maxDemand,runTime,priority);
    }
}
