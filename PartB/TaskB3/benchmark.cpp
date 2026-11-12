#include "dispatch_models.hpp"
#include "../../tools/checks.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
inline volatile double dispatchChecksum=0;
int main(int argc,char** argv) {
    using namespace dispatch_experiment;
    Tests tests(argc,argv);
    std::cout << "trial,method,bills,working_set,elapsed_ms,checksum,requested_payload_bytes\n" << std::fixed << std::setprecision(6);
    const Method methods[]={Method::Virtual,Method::Switch,Method::VariantVisit};
    for(std::size_t count:{1000u,1000000u,10000000u}) {
        {
            Dataset data(std::min(count,Dataset::maximumWorkingSet));
            const double expected=data.expected(count);
            tests.check(data.requestedPayloadBytes()<=64u*1024u*1024u,"benchmark payload within memory budget");
            for(int trial=1;trial<=5;++trial) {
                for(int order=0;order<3;++order) {
                    Method method=methods[(order+trial-1)%3];
                    const auto begin=std::chrono::steady_clock::now();
                    const double checksum=data.run(method,count);
                    dispatchChecksum=checksum;
                    const auto end=std::chrono::steady_clock::now();
                    tests.check(checksum==expected,"large dispatch checksum equals independent fixture calculation");
                    std::cout << trial << ',' << methodName(method) << ',' << count << ',' << data.size() << ','
                              << std::chrono::duration<double,std::milli>(end-begin).count() << ',' << checksum
                              << ',' << data.requestedPayloadBytes() << '\n';
                }
            }
        }
        tests.check(Bill::live==0,"all working-set objects released between sizes");
    }
}
