# B2 exact prediction before first execution

Exact output: `ok done caught negative [log too large] caught too large \n`, including the final space before newline. 100 passes validate and prints ok then done. -5 throws invalid_argument, which process does not catch, and main prints caught negative. 2e6 throws out_of_range; process logs too large and bare-rethrows the same exception, then main prints caught too large. The success-path done statement is skipped for each throwing call. No original prediction may be rewritten after observing execution.
