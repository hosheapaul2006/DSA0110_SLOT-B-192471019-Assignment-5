#include "dispatch_models.hpp"
#include "../../tools/checks.hpp"
#include <iostream>
int main(int argc,char** argv) {
    using namespace dispatch_experiment;
    Tests tests(argc,argv);
    tests.check(Bill::live==0,"no initial dispatch objects");
    {
        Dataset data(10);
        std::cout << "Index Expected Virtual Switch Variant\n";
        for(std::size_t i=0;i<10;++i) {
            const double expected=independentExpected(i);
            tests.check(data.at(Method::Virtual,i)==expected,"ten-bill virtual matches hand calculation");
            tests.check(data.at(Method::Switch,i)==expected,"ten-bill switch matches hand calculation");
            tests.check(data.at(Method::VariantVisit,i)==expected,"ten-bill variant matches hand calculation");
            std::cout << i << ' ' << expected << ' ' << data.at(Method::Virtual,i) << ' '
                      << data.at(Method::Switch,i) << ' ' << data.at(Method::VariantVisit,i) << '\n';
        }
        tests.check(data.expected(10)==116700,"independent ten-bill sum");
        for(Method method:{Method::Virtual,Method::Switch,Method::VariantVisit}) {
            tests.check(data.run(method,10)==116700,"each design totals ten bills");
            tests.check(data.run(method,23)==data.expected(23),"full/partial replay batches preserve checksum");
            tests.check(data.run(method,0)==0,"zero logical records");
        }
        bool rejected=false;
        try {(void)data.at(Method::Virtual,10);}catch(const std::out_of_range&){rejected=true;}
        tests.check(rejected,"dataset index bounds");
        rejected=false;
        try {(void)data.run(static_cast<Method>(99),1);}catch(const ValidationError&){rejected=true;}
        tests.check(rejected,"invalid dispatch tag");
        std::cout << "Ten-bill total=116700; requested payload bytes=" << data.requestedPayloadBytes() << '\n';
    }
    tests.check(Bill::live==0,"all virtual and variant objects destroyed");
    for(std::size_t invalid:{0u,65537u}) {
        bool rejected=false;
        try {Dataset data(invalid);}catch(const ValidationError&){rejected=true;}
        tests.check(rejected,"bounded working-set guard");
    }
    Input invalid=fixture(0);invalid.consultation=-1;
    bool rejected=false;
    try {TaggedBill bill(1,invalid);}catch(const ValidationError&){rejected=true;}
    tests.check(rejected,"invalid tagged charge rejected");
    invalid.kind=static_cast<Kind>(99);
    rejected=false;
    try {(void)makeVirtual(1,invalid);}catch(const ValidationError&){rejected=true;}
    tests.check(rejected,"unknown bill type rejected");
    const double roundedExpected[]={0.22,0.44,0.11,0.28};
    for(int kind=0;kind<4;++kind) {
        Input fractional;
        fractional.kind=static_cast<Kind>(kind);
        fractional.consultation=0.105;fractional.tests=0.105;
        fractional.days=2;fractional.perDay=0.105;fractional.procedures=0.105;fractional.medicines=0.105;
        fractional.packagePrice=0.105;fractional.base=0.105;fractional.hours=1.5;fractional.hourlyRate=0.105;
        auto virtualBill=makeVirtual(99,fractional);
        TaggedBill tagged(99,fractional);
        Variant variant=makeVariant(99,fractional);
        tests.check(virtualBill->total()==roundedExpected[kind] && tagged.total()==roundedExpected[kind]
                    && visitTotal(variant)==roundedExpected[kind],"all designs normalize individual paise components identically");
    }
    tests.check(Bill::live==0,"fractional-model objects cleaned");
    std::cout << "sizeof Bill=" << sizeof(Bill) << " OP=" << sizeof(OutPatientBill) << " IP=" << sizeof(InPatientBill)
              << " DayCare=" << sizeof(DayCareBill) << " Emergency=" << sizeof(EmergencyBill) << '\n'
              << "sizeof TaggedBill=" << sizeof(TaggedBill) << " Variant=" << sizeof(Variant)
              << " owner pointer=" << sizeof(std::unique_ptr<Bill>) << '\n';
}
