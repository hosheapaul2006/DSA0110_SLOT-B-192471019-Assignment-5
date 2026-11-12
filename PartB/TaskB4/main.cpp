#include "../TaskB1/test_suite.hpp"
#include "extension_suite.hpp"
#include <iostream>
int main(int argc,char** argv) {
    Tests tests(argc,argv);
    std::cout << std::fixed << std::setprecision(2);
    runBillingTests(tests,std::cout); // Exact unchanged baseline suite.
    runExtensionTests(tests,std::cout);
}
