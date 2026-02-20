# Error formatting
[X] Fix multi-line

# Parser
[X] List init: type[]
[ ] {arg1, arg2, arg3,...} defines a list
[X] If else: if cond { ... } else if cond { ... } else { ... }
[X] Parse inner block
[X] Get expressions: used to import modules (const io = get "io";)
[X] Member expressions
[X] String literal
[X] Add all escape characters
[X] Comparison operators
[X] && || !
[#] For loop
[X] +=, -=, *=, /=, %=
[X] ++, --

# Interpreter
[ ] Add support for operator overridding for custom and built-in data types
[X] Built-in functions as data type of built_in, inherits from function
[X] Modules
[X] io.log(*args): Prints all arguments onto the screen
[X] io.prompt(*args): log the arguments and prompt for user input
[ ] Function declaration to be typed
[X] null
[ ] Every function returns null by default if not returning, but not matching the type means error
[X] Call expressions
[X] Strings
[X] Get expressions
[X] If else (you must remember to add new SCOPES)
[X] Booleans (true/false)
[X] Comparison operators
[X] && || ! (only for boolean support)
[#] For loop (only left is: for var init : )
[#] math module (halting for now)
[X] +=, -=, *=, /=, %=
[ ] ++, --

# Compiling
[ ] Remove unused variables
[ ] Inline small functions
[ ] Loop unrolling
[ ] Constant folding (3 + 4 → 7)