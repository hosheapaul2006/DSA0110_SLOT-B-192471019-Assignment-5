#include "../../TaskB1/errors.hpp"
#include "../../../tools/checks.hpp"
#include <iostream>

class DetailedClaimError : public hospital::ClaimError {
    int detail_, identity_;
public:
    inline static int copies=0, nextIdentity=0;
    explicit DetailedClaimError(int detail) : ClaimError("detailed claim"), detail_(detail), identity_(++nextIdentity) {}
    DetailedClaimError(const DetailedClaimError& other)
        : ClaimError(other), detail_(other.detail_), identity_(++nextIdentity) { ++copies; }
    int detail() const noexcept { return detail_; }
    int identity() const noexcept { return identity_; }
};
void throughExactCatch(bool copy, int& originalIdentity) {
    try { throw DetailedClaimError(7); }
    catch(const DetailedClaimError& error) {
        originalIdentity=error.identity();
        DetailedClaimError::copies=0; // isolate copies caused by the rethrow itself
        if(copy) throw error;
        throw;
    }
}
void throughBaseCatch(bool copy, int& originalIdentity) {
    try { throw DetailedClaimError(7); }
    catch(const hospital::ClaimError& error) {
        originalIdentity=dynamic_cast<const DetailedClaimError&>(error).identity();
        DetailedClaimError::copies=0;
        if(copy) throw error;
        throw;
    }
}
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    for(bool baseCatch:{false,true}) {
        for(bool copy:{false,true}) {
            int original=0, observed=0, detail=0;
            bool derived=false;
            try {
                if(baseCatch) throughBaseCatch(copy,original);
                else throughExactCatch(copy,original);
            } catch(const DetailedClaimError& error) {
                derived=true;observed=error.identity();detail=error.detail();
            } catch(const hospital::ClaimError&) {}
            const bool sliced=baseCatch && copy;
            tests.check(derived==!sliced,"only copied base expression loses derived dynamic type");
            if(!sliced) tests.check(detail==7,"derived payload retained");
            if(!copy) tests.check(observed==original && DetailedClaimError::copies==0,"bare rethrow preserves object identity without copying");
            if(copy && !baseCatch) tests.check(observed!=original && DetailedClaimError::copies==1,"exact-type throw-expression copies object but retains dynamic type");
            std::cout << (baseCatch?"Base catch":"Exact catch") << ' ' << (copy?"throw e":"throw;")
                      << ": " << (derived?"DetailedClaimError retained":"ClaimError copy; derived detail sliced") << '\n';
        }
    }
}
