//
//  main.cpp
//  test
//
//  Created by Tobias Heinemann on 2/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include <iostream>

struct A
{
    static int k;
};

int A::k = 1;

int main(int argc, const char * argv[])
{
    A a1, a2;

    a1.k = 2;
    std::cout << a1.k << std::endl;
    std::cout << a2.k << std::endl;

    a2.k = 5;
    std::cout << a1.k << std::endl;
    std::cout << a2.k << std::endl;

    std::cout << "Hello, World!\n";
    return 0;
}

