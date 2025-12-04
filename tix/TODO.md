# Parser
[X] List init: type[]
[ ] {arg1, arg2, arg3,...} defines a list
[ ] If else: if cond { ... } else if cond { ... } else { ... }
[ ] Get expressions: used to import modules (const io = get "io";)
[X] Member expressions
[X] String literal
[X] Add all escape characters

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
[ ] Get expressions