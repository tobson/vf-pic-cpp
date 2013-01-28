//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"
#include "boundaries.h"
#include "config.h"
#include "global.h"
#include "scalar-field.h"
#include "vector-field.h"

#include <iostream>
#include <libconfig.h++>
#include <vector>

int main(int argc, const char * argv[])
{
  std::string srcdir = ".";
  if (argc > 1) srcdir = argv[1];
  
  try
  {
    config::read (srcdir + "/config.in");
    global::computeVariables();
    config::write (srcdir + "/config.out");
  }
  catch (const libconfig::ConfigException& e)
  {
    std::cout << e.what () << std::endl;
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cout << e.what () << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cout << "Unknown exception" << std::endl;
    return 1;
  }
  
  std::cout << "Hello, World! My name is " << argv[0] << ".\n";
  return 0;
}