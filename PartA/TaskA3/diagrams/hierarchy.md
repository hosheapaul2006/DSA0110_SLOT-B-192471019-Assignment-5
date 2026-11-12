# Payer UML

```mermaid
classDiagram
class Payer {
  <<abstract>>
  +amountCovered(Bill_ref) const double*
  +name() const text*
  +~Payer() virtual noexcept
}
class SelfPay {
  +amountCovered(Bill_ref) const double
}
class Insurance {
  -coverLimit
  -coPaymentPercent
  +amountCovered(Bill_ref) const double
}
class GovernmentScheme {
  -packageAllowances
  +amountCovered(Bill_ref) const double
}
class CorporatePayer {
  -coverageFraction
  -cap
  +amountCovered(Bill_ref) const double
}
Payer <|-- SelfPay
Payer <|-- Insurance
Payer <|-- GovernmentScheme
Payer <|-- CorporatePayer : later extension
```

All inheritance is public. amountCovered is pure virtual in Payer and overridden in the concrete classes. CorporatePayer is planned here and implemented separately in B4; no existing payer implementation requires a new case or type check.

![Standalone diagram](hierarchy.svg)
