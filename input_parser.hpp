//
//  input_parser.hpp
//  SchedulingAndDeadlockAvoidance
//
//  Created by Ziyao Yan on 5/23/18.
//

#ifndef input_parser_hpp
#define input_parser_hpp

#include <stdio.h>
#include <string>

class Input_Parser {
    vector<string> lines;
    
public:
    Input_Parser(std::string filename);
    analyze();
};

#endif /* input_parser_hpp */
