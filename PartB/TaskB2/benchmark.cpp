#include "validation.hpp"
int main(int argc, char** argv) {
    Tests tests(argc,argv);
    validation_experiment::benchmark(tests,{validation_experiment::Method::Optional,validation_experiment::Method::Exceptions});
}
