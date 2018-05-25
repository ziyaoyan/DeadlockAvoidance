//
//  main.cpp
//  SchedulingAndDeadlockAvoidance
//
//  Created by Ziyao Yan on 5/23/18.
//

#include <stdio.h>
#include "input_parser.hpp"

using namespace std;

int main(int argc, const char* argv[]){
    

    //if user does not provide input file.
    if(argc != 2) {
        return 1;
    }
    Input_Parser input = new Input_Parser(argv[1]);
    input.analyze(); 
    return 0;
}
