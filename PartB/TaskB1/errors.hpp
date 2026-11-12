#ifndef HOSPITAL_ERRORS_HPP
#define HOSPITAL_ERRORS_HPP
#include <stdexcept>
namespace hospital {
class BillingError : public std::runtime_error { public: using std::runtime_error::runtime_error; };
class ValidationError : public BillingError { public: using BillingError::BillingError; };
class ClaimError : public BillingError { public: using BillingError::BillingError; };
class PackageError : public BillingError { public: using BillingError::BillingError; };
class MissingPatientError : public ValidationError { public: using ValidationError::ValidationError; };
}
#endif
