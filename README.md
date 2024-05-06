# Multicommand_shell_scripting
It is a custom shell program written in C that interprets and executes user commands. It leverages system calls such as `fork()`, `exec()`, and others to handle commands, supporting special operations like piping, redirection, and background processing. This project is designed to illustrate advanced process management in a UNIX-like environment.

## Features

- Infinite command input loop.
- Execution of commands with arguments count between 1 and 5.
- Special characters handling for complex command chaining such as:
  - `#` for file concatenation
  - `|` for piping between commands
  - `>`, `<`, `>>` for redirection
  - `&&`, `||` for conditional execution
  - `&` for background processing
  - `;` for sequential execution

## Getting Started

### Prerequisites

- GCC compiler
- Linux or any UNIX-like operating system
