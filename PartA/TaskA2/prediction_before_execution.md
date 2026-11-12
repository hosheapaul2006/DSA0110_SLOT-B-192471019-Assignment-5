# A2 exact prediction before first execution

Exact output: `500 ~OPBill ~Bill \n`, including the final space before newline. show() is nonvirtual and resolves to Bill::show, but the call to total() inside it is virtual and reaches OPBill::total. Deleting the derived object through b uses the virtual destructor: OPBill's body first, then Bill's body. This prediction must remain unchanged after observation.
