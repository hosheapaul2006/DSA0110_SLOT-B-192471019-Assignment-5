#ifndef CLAIM_VALIDATION_EXPERIMENT_HPP
#define CLAIM_VALIDATION_EXPERIMENT_HPP
#include "../TaskB1/errors.hpp"
#include "../../tools/checks.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <vector>

namespace validation_experiment {
inline double approveThrow(double billed, double cover) {
    if (!std::isfinite(billed) || !std::isfinite(cover) || billed<0 || cover<0)
        throw std::invalid_argument("negative or nonfinite amount");
    if (billed>cover) throw hospital::ClaimError("exceeds cover");
    return billed;
}
inline std::optional<double> approveOptional(double billed, double cover) noexcept {
    if (!std::isfinite(billed) || !std::isfinite(cover) || billed<0 || cover<0 || billed>cover) return std::nullopt;
    return billed;
}
enum class Status { Approved, InvalidAmount, ExceedsCover };
struct StatusResult { Status status; double amount; };
inline StatusResult approveStatus(double billed, double cover) noexcept {
    if (!std::isfinite(billed) || !std::isfinite(cover) || billed<0 || cover<0) return {Status::InvalidAmount,0};
    if (billed>cover) return {Status::ExceedsCover,0};
    return {Status::Approved,billed};
}
enum class Method { Optional, Exceptions, ReturnCode };
inline const char* name(Method method) noexcept {
    switch(method) {
        case Method::Optional:return "optional";
        case Method::Exceptions:return "exceptions";
        case Method::ReturnCode:return "return_code";
    }
    return "unknown";
}
inline std::vector<double> makeClaims(std::size_t count, unsigned invalidPercent) {
    if(count==0 || count%100!=0 || invalidPercent>100) throw std::invalid_argument("invalid experiment dimensions");
    std::vector<double> values(count);
    for(std::size_t i=0;i<count;++i) values[i]=(i%100<invalidPercent)?60000.0:40000.0;
    std::mt19937 random(192471019u+invalidPercent);
    std::shuffle(values.begin(),values.end(),random); // same seeded input reused by every method
    return values;
}
struct Result { double paid=0; std::size_t rejected=0; };
inline Result process(Method method, const std::vector<double>& claims) {
    Result result;
    switch(method) {
        case Method::Optional:
            for(double amount:claims) {
                auto paid=approveOptional(amount,50000);
                if(paid) result.paid+=*paid; else ++result.rejected;
            }
            break;
        case Method::Exceptions:
            for(double amount:claims) {
                try { result.paid+=approveThrow(amount,50000); }
                catch(const hospital::ClaimError&) { ++result.rejected; }
            }
            break;
        case Method::ReturnCode:
            for(double amount:claims) {
                auto paid=approveStatus(amount,50000);
                if(paid.status==Status::Approved) result.paid+=paid.amount; else ++result.rejected;
            }
            break;
    }
    return result;
}
inline volatile double observableChecksum=0;
inline void benchmark(Tests& tests, const std::vector<Method>& methods) {
    constexpr std::size_t count=1000000;
    constexpr int trials=5;
    if(methods.empty()) throw std::invalid_argument("no benchmark methods");
    std::cout << "trial,method,claims,invalid_percent,elapsed_ms,rejected,approved_sum\n" << std::fixed << std::setprecision(6);
    for(unsigned percentage:{0u,1u,50u}) {
        const auto claims=makeClaims(count,percentage);
        const std::size_t expectedRejected=count*percentage/100;
        const double expectedPaid=(count-expectedRejected)*40000.0;
        for(int trial=1;trial<=trials;++trial) {
            for(std::size_t turn=0;turn<methods.size();++turn) {
                const Method method=methods[(turn+static_cast<std::size_t>(trial-1))%methods.size()];
                const auto begin=std::chrono::steady_clock::now();
                Result result=process(method,claims);
                observableChecksum=result.paid+static_cast<double>(result.rejected); // one store, not one per claim
                const auto end=std::chrono::steady_clock::now();
                tests.check(result.rejected==expectedRejected,"benchmark exact rejection count");
                tests.check(result.paid==expectedPaid,"benchmark independent approved-sum checksum");
                std::cout << trial << ',' << name(method) << ',' << count << ',' << percentage << ','
                          << std::chrono::duration<double,std::milli>(end-begin).count() << ','
                          << result.rejected << ',' << result.paid << '\n';
            }
        }
    }
}
}
#endif
