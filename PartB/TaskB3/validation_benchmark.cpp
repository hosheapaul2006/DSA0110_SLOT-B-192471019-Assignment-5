#include "../TaskB2/validation.hpp"
int main(int argc,char** argv) {
    Tests tests(argc,argv);
    validation_experiment::benchmark(tests,{validation_experiment::Method::ReturnCode,validation_experiment::Method::Exceptions});
}
