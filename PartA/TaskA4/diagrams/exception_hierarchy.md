# Exception UML

```mermaid
classDiagram
class exception {
  <<std>>
  +what() const noexcept text
}
class runtime_error {
  <<std>>
}
class logic_error {
  <<std>>
}
class bad_alloc {
  <<std>>
}
class invalid_argument {
  <<std>>
}
class out_of_range {
  <<std>>
}
class BillingError
class ValidationError
class MissingPatientError
class ClaimError
class PackageError
exception <|-- runtime_error
exception <|-- logic_error
exception <|-- bad_alloc
logic_error <|-- invalid_argument
logic_error <|-- out_of_range
runtime_error <|-- BillingError
BillingError <|-- ValidationError
ValidationError <|-- MissingPatientError
BillingError <|-- ClaimError
BillingError <|-- PackageError
```

Standard-library branches remain distinct from the custom billing hierarchy. Catch the most specific types before BillingError/std::exception, then a final catch-all. bad_alloc is not converted into an invented successful claim.

![Standalone diagram](exception_hierarchy.svg)
