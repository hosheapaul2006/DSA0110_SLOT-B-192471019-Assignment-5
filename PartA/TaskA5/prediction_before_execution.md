# A5 prediction before first execution

Exact output: `~f ~main caught 42\n`. f's Guard is destroyed first as throw 42 unwinds f, then the main Guard inside the try block is destroyed before entering the matching int handler. Each destructor prints a trailing space. The handler prints caught, a space, and 42 with no trailing space, then main prints newline. Execution never returns normally to the not printed statement. The catch-all is not selected. Preserve this prediction unchanged after observation.
