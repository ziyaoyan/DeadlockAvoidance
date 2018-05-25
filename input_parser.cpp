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
            //System Configuration
            case 'C' :
                int time, mainMemory, numOfSerialDevice, timeQuantum;

                break;       // and exits the switch
            //Job Arrival
            case 'A' :
                int jobNo,mainMemory, maxDemand, unitOfTime, priority;
                break;
            //Request for Devices
                /*The example above states that at time 10, job number 5 releases one device. A job only releases devices when it is running on the CPU. Quantum is interrupted. One or more jobs may be taken off the Device Wait queue due to this.*/
            case 'Q' :
                int time, jobNo;
                break;
            //Release for Devices
            case 'L' :
                int time, jobNo, noOfDevices;
                break;
            //A display of the current system status in Readable format (with headings and properly aligned)
            case 'D' :
                int time;
                cout<<"Some shit"<<endl;
                break;
            default :
                cout<<command<<" is not valid."<<endl;
        }
    }
}
