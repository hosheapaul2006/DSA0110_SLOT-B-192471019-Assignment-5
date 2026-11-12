# Bill UML

```mermaid
classDiagram
class Bill {
  <<abstract>>
  -billID
  -Patient patient
  -string packageCode
  +total() const double*
  +type() const text*
  +print(stream) const
  +~Bill() virtual noexcept
}
class Patient {
  -recordID
  -string name
}
class OutPatientBill {
  -consultation
  -tests
  +total() const double
  +type() const text
}
class InPatientBill {
  -days
  -roomPerDay
  -procedures
  -medicines
  +total() const double
  +type() const text
}
class DayCareBill {
  -packagePrice
  +total() const double
  +type() const text
}
class EmergencyBill {
  -basePrice
  -hours
  -hourlyPrice
  +total() const double
  +type() const text
}
Bill *-- Patient
Bill <|-- OutPatientBill
Bill <|-- InPatientBill
Bill <|-- DayCareBill
Bill <|-- EmergencyBill
```

total/type are pure virtual in Bill and overridden in every concrete type. print is a shared nonvirtual wrapper. Generalization denotes public inheritance; the composition diamond denotes the owned Patient value. Asterisks denote abstract operations in this Mermaid notation. The standalone SVG version was rendered and visually reviewed.

![Standalone diagram](hierarchy.svg)
