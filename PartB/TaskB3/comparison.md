# B3 - Measured dispatch and validation trade-offs

## Correctness and equivalent data

main.cpp constructs the same ten inputs in three representations: production Bill objects owned through unique_ptr<Bill>, one tagged record with all category fields and a switch, and std::variant of the four production derived classes visited with std::visit. Every per-bill value is checked against the independent hand expectation, and each sum is116700. Zero, full/partial replay batches, invalid tags/indices/charges and payload bounds are tested. Additional fractional-paise fixtures confirm that every representation normalizes individual monetary components identically; the switch does not defer component rounding or perform an unnecessary day-care rounding call.

Input dimensions and timing scope were recorded in measurement_plan.md before measurement. inputs/ten_bills.csv transcribes the ten embedded charge fixtures and hand totals. benchmark_results.md gives every raw trial and median; outputs/*_verification.json contains actual source/dependency hashes, commands, flags, UTC times, exits and check counts.

## Memory-bounded scaling

The requested1000,1000000 and10000000 logical bill calculations were all executed at -O2. At most65536 records per representation are retained and replayed in full/partial batches; input IDs/charge patterns repeat and are not inserted into a transaction ledger. The benchmark does not claim ten million simultaneously resident/distinct patients. The comparison remains equivalent because the three designs process the same ordered values the same number of times and verify the same independent checksum. It measures computation/traversal of these bounded layouts, not streaming bandwidth of a multi-gigabyte resident dataset.

Construction, validation, allocation, destruction, I/O and assertions occur outside the timed region. total arithmetic/rounding, dispatch/traversal and one final observable checksum store occur inside. Five trials rotate method order. steady_clock gives milliseconds; no trials are discarded. A64 MiB payload guard prevents impractical storage. Actual requested payload bytes are recorded in the raw CSV and exclude allocator headers, string-character allocations and vector control objects; no resident-memory number is invented.

Virtual traversal has pointer indirection and separately allocated objects. Switch records and variants are contiguous but have different per-element footprints. Differences in locality, branching, compiler inlining/devirtualization and the billing formulas are inseparable from this experiment's observed timing; it is not a universal isolated cost of one virtual instruction. Requested logical count changes work, while the maximum working set stays bounded, so do not infer real large-dataset cache behavior from n alone.

## Observed dispatch result

In the recorded trials, switch and variant traversal were faster than virtual traversal at the large logical counts. The small1000-record timings are especially sensitive to timer/cache/scheduling noise. Exact medians and all five samples are in benchmark_results.md rather than a predicted speedup. These observations favor compact value-oriented processing for this closed, arithmetic-heavy batch workload; they do not outweigh extensibility automatically or establish production request throughput.

All three totals are O(n) for n processed logical bills with a fixed type set. Each charge formula is O(1) arithmetic. A virtual hierarchy generally extends by adding a derived class and application construction. A switch needs central enum/branch changes; a variant needs its compile-time alternative list updated/recompiled and any non-generic visitors adjusted. The generic total visitor itself can remain unchanged for a type with the same total interface.

## Actual storage and variant nuance

main.cpp prints actual sizeof Bill, each concrete type, TaggedBill, Variant and owner pointer. The single tagged record reserves irrelevant fields for every bill. The hierarchy specializes fields, commonly adds a vptr and stores owner pointers; separately allocated objects also have unmeasured allocator overhead. A variant stores enough for its largest alternative plus its discriminator and padding. In this specific variant, alternatives inherit Bill and retain their vptr/common fields, so claiming the variant removes all polymorphic storage would be false. A variant of the separate nonvirtual B1 value types could be smaller but is a different model.

sizeof excludes dynamic string characters and runtime metadata. Fixed storage is O(1) per element for this finite type set, and O(n) overall. Name/package text adds O(total text length). A variant grows when a larger alternative is added, potentially increasing every element; a hierarchy can allocate the new type at its own size. Shared type/vtable metadata is per type, not freshly copied for every object.

## Return codes versus exceptions

validation_benchmark.cpp compares exactly one million claims at0%,1%,50% invalid with identical shuffled input and predicate, five rotated trials per method. It includes validation, exception construction/throw/catch or status testing, and one checksum store; it excludes input generation and logging. Approved sums/rejection counts are independently checked for every timed run. B2 separately compares optional using the same framework.

The observed zero-error medians were similar. At1% and50% invalid, exceptions were substantially slower than return codes in this runtime. Both perform O(n) validation, but exceptions add object creation/unwinding on the error path. A status/optional result is appropriate for common business rejection; exceptions preserve detailed typed context and cleanly propagate unexpected failures across layers. Actual production error rates, stack depth and logging may differ. These microbenchmarks are not an SLA or a replacement for whole-system load testing.
