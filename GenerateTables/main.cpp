//
//  main.cpp
//  GenerateTables
//
//  Created by Ellis Sparky Hoag on 7/4/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

#include "CubeDataStructure.h"

int main(int argc, const char * argv[]) {
    
    CubeDataStructure::Solver my_solver("/Users/ellis/GitHub/CubeSolver/CubeDataStructure/Tables");
    
    my_solver.generate_tables();
    
    return 0;
}
