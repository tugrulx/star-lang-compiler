# STAR Language 

STAR is a small scripting language supporting integers and text (strings) with basic arithmetic, I/O, and loops.

Variables must be declared before use using `int` for integers and `text` for strings. Declarations can include initialization, e.g., `int count is 5.` or `text msg is "Hello"`. Integers are positive only; negative results are set to 0. Maximum integer value is 99999999, and text variables cannot exceed 256 characters. All variables have global scope.

Assignments use the `is` keyword. Integers can be assigned values or simple arithmetic expressions with two operands, e.g., `myInt is myInt + 1.` Text can be concatenated using `+` or have substrings removed using `-`.

Comments are enclosed within `/*` and `*/` and can span multiple lines.

Input is read using `read` with an optional prompt, e.g., `read "Enter number:", myInt.` Output is printed using `write`, e.g., `write "Values:", a, " ", b, " ", c.` A new line can be printed using `newLine.`

Loops are supported via `loop <n> times` and can execute single statements or blocks enclosed in `{ }`. Loops can be nested.

Key points: maximum 8-digit integers, maximum 256-character strings, variables must be declared before use, initialization cannot use expressions or variables, and all assignments, arithmetic, and I/O follow the rules described above.

