#include "extension_suite.hpp"
#include <iomanip>
#include <iostream>
int main(int argc,char** argv) {
    Tests tests(argc,argv);
    std::cout << std::fixed << std::setprecision(2);
    runExtensionTests(tests,std::cout);
}
