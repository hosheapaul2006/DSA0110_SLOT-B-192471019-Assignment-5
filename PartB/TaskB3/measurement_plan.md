# B3 measurement plan recorded before execution

The small correctness fixture contains ten bills with independent expected totals: 1700,50000,25000,11000,4000,7000,10000,3500,0,4500; sum116700. All representations receive the same charge inputs and common identity/package data.

Large timing uses 1000,1000000 and10000000 logical total() evaluations. Because initial available RAM was about1.244 GiB, retain at most65536 bill records in each representation, then replay that identical bounded working set in full/partial batches until the requested operation count is reached. There is no modulo operation per hot-loop iteration. Logical records repeat the documented charge pattern; this is not a measurement of ten million simultaneously resident/distinct patient objects or end-to-end hospital transactions.

Virtual owners, tagged records and variant records are built together from the same fixtures before timing. A small integer charge adjustment based on input index varies the records within the set. Expected checksum uses the independent ten hand totals plus that adjustment, not another dispatch implementation. Generation, validation, allocation, destruction, assertions and printing are excluded. Each timed loop includes total arithmetic/dispatch and one final observable checksum store. This compares traversal/computation over different actual layouts, not an isolated universal vtable-instruction cost.

Run five trials per method/size at -O2; rotate method order each trial. Use steady_clock, report milliseconds, preserve every trial, and summarize with the median. Record actual sizeof values and requested payload bytes; exclude allocator overhead/string heap characters rather than inventing their size. Guard the working-set payload budget at64 MiB. Templates/visit can be optimized differently by the compiler, which is part of the observed result.

The actual variant is std::variant of the four production derived classes. Those classes retain their inherited polymorphic base/vptr. It would be wrong to claim this variant removes all virtual-related storage; a separate nonvirtual value-type variant is another design.

Exception/return-code measurement reuses B2's identical validation predicate/input generator with one million claims, exact0/1/50% invalid, five rotated trials, independent rejected-count/approved-sum checks and no per-claim I/O. B2 separately compares optional. No speed result is predicted as fact.
