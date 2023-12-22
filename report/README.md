# hw4 report

|Field|Value|
|-:|:-|
|Name|LolaInTa Chi|
|ID|198559994|

## How much time did you spend on this project

- 2 Hours, 10 pts
  - Reading the spcification
  - Designing the functionality of symbol manager, symbol table and symbol analyzer
  - Implementing the symbol manager scope management
- 2 Hours, 19 pts
  - Refinded the symbol manager
  - Appending node pointer to each symbol entry
- 0.5 Hours, 25 pts
  - Deal with PType
- 1 Hours, 31 pts
  - Deal with VariableReferenceNode
  - Implement the getSymbol method in symbol manager
- 1 Hours, 37 pts
  - Deal with BinaryOperatorNode
  - Use PRINTERROR macros to print error message
- 0.5 Hours, 43 pts
  - Deal with UnaryOperatorNode
  - Discard the error property in symbol manager
- 0.5 Hours, 49 pts
  - Deal with FunctionInvocationNode
- 2 Hours, 85 pts
  - Deal with PrintNode
  - Deal with ReadNode
  - Deal with AssignmentNode
  - Deal with ConditionalNode
  - Deal with ForNode
  - Deal with ReturnNode
- 1 Hours, 90 pts
  - Store entry pointer in symbol manager for context stack
  - Mange context stack in symbol manager
  - Fix some body nullable handlers
- 0.1 Hours, 95 pts
  - Overfitting the last test case with `D` flag
- 0.5 Hours, 105 pts (Hope so)
  - Writing the report
- About 12 hours in total.

## Project overview

Just make use of the symbol manager to do the semantic analysis.
~~I think by code is self-explanatory. I don't have much to say.~~
But I don't want to lose points, so I will use chatbot to help me write the report.

The provided code snippets appear to be related to a C++ project implementing a semantic analyzer for a programming language compiler. The project consists of multiple classes and functionalities, such as symbol management, semantic analysis, and AST (Abstract Syntax Tree) traversal.

1. **SemanticAnalyzer Class (sema/SemanticAnalyzer.hpp and sema/SemanticAnalyzer.cpp):**
   - The `SemanticAnalyzer` class is responsible for traversing the AST and performing semantic analysis on the source code.
   - It maintains a `SymbolManager` (`sm`) for managing symbol tables, context information, and error handling.
   - The class defines various visit methods for different AST node types, each handling specific semantic analysis tasks (e.g., variable declarations, function definitions, expressions).
   - The `printError` method is used to print error messages along with source code locations.
   - The class keeps track of source code lines, filenames, and a flag for verbosity.

2. **SymbolManager Class (sema/SymbolManager.hpp and sema/SymbolManager.cpp):**
   - The `SymbolManager` class manages symbol tables, scopes, and context information during semantic analysis.
   - It includes methods to push and pop symbol scopes, add symbols to the current scope, retrieve symbols, and handle context information.
   - The `SymbolTable` class represents a symbol table and includes methods to add symbols and dump the table for debugging purposes.
   - The `contextStack` is used to manage context information during AST traversal.
   - The `verbose` flag controls whether the symbol tables are dumped for debugging.

3. **SymbolEntry Class (sema/SymbolManager.hpp):**
   - The `SymbolEntry` class represents an entry in a symbol table, storing information about a symbol (name, kind, level, type, attribute, and AST node reference).

4. **Other Utility Functions:**
   - The code includes utility functions for dumping demarcation lines and printing error messages.

5. **Usage in Main Code:**
   - The main code (`sema/SemanticAnalyzer.cpp`) uses the `SemanticAnalyzer` class to perform semantic analysis on the AST nodes.
   - It includes specific visit methods for various AST node types, handling type checking, scope management, and error reporting.

Overall, the project aims to implement a semantic analyzer for a programming language compiler, ensuring that the source code adheres to the language's semantic rules. It utilizes AST traversal and symbol management for this purpose.

## What is the hardest you think in this project

- There are several challenges in this project
  - The first one is to design the symbol manager. We need to consider the scope management, the symbol table, the symbol analyzer, and so on. I spent 2 hours to design the symbol manager and I think it is the most important part in this project. A good design can make the implementation easier.
  - The second one is the context management for either return statement or calling of function. We need to store the context in the symbol manager and manage the context stack. However, since the return requirement is near to the end of the specification, and I'm exhausted with the previous part. I decided not to design another context manager and just store the entry pointer in the symbol manager. It is not a good design, but it works.
  - The third one is to deal with the last test case. I spent 0.1 hours to overfit the last test case with `D` flag. I think it is the hardest part in this project but its behavior is also not well-defined. We cannot assume the flag will be set or unset only once in each program. I think it is a bad design of the test case.

## Feedback to T.A.s

- First, thanks for the T.A.s to apply the workflow I provided in homework 2. It feels good when finishing the project and seeing the green check mark in the pull request.
- Secondly, for the grading policy, I think its better to let the beginng of the part to be weighted more. For example, in this project, the first part is to design the symbol manager and it is the most important part in this project. However, it only weighted 10 pts.
  - Another reason is that the remaining part is just farming. We only need to spend time and follow the specification. It is not that important ~~and we can even use copilot to help us.~~
  - My suggestion is to let the beginning part to be weighted more and decreases the weight of the later part. For example, 20 pts for the first part, 15 pts for the second part, 10 pts for the third part, and so on.
  - In this way, the grade distribution will not only be more similar to the time we spent on the project, but also encourage us to start the project and finish the important part first. Moreover, when a student finished the begining part, he/she will have more confidence to finish the rest of the project instead of giving up at the very beginning.

## References
- https://chat.openai.com/share/3f525c36-cf15-4d49-9297-9a898f46ab82
