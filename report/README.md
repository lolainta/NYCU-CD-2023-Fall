# hw3 report

|Field|Value|
|-:|:-|
|Name|LolaInTa Chi|
|ID|198559994|

## How much time did you spend on this project

- 2 Hours for reading the specification and understanding what should we do.
    - 30 Minutes for learning visitor pattern and how to use it.
- 2 Hours for dealing with the first and the second test case.
    - Define the members of the `ProgramNode` class.
    - Got 14/95 points.
- 2 Hours for rest of the test cases.
    - Earned 95/106 points.
- 1 hours for restyling the code.
    - Remove pure `print` member functions.
    - Reorder include files.
    - Replace `const char*` with `std::string*`.
    - Construct every node with base `AstNode` only.
- 30 minitues for writing the report.
- 7.5 hours in total.


## Project overview

In this project, I used visitor pattern to implement the dump ast feature.

First, I start by defining the `ProgramNode` class with variable declarations only. Of course, I used visitor pattern to implement it at the same time.

To complete this part, I have to understand the visitor pattern and how to use it. I also have to finish the `DeclNode`, `VariableNode`, `ConstantValueNode`, and so on.
Unfortunately, I still failed the first two test cases since I didn't implement the `CompoundStatementNode` yet.

After that, I implemented the `CompoundStatementNode` and passed the first two test cases. Those part tooks me about 3 hours.

Then, I implemented the rest of the nodes and passed the rest of the test cases. The remaining part is simple and no need to think too much, we only need to spend time and follow the specification.

Finally, I restyled the code and wrote this report.

## What is the hardest you think in this project

The hardest part the first two hours. We should not only understand the visitor pattern, but also have a clear idea of how to define those nodes and how they are related to each other.

The second challenge is to debug the code. When the test case is failed, it is hard to find out which node is wrong. I have to use `printf` to print out the information of each node to find out the problem.

The rest of them are not that hard. I just need to follow the specification and implement the visitor pattern. In fact, the last two hours are boring. And thanks for copilot, it helps me a lot in this part, especially when I need to write the `accept` function since it is almost the same for each node.

## Feedback to T.A.s

- The workflow I provided in homework 2 did not be applied in this project. I hope that we can have it in the next project.
- Interesting, for the feedback section in the first homework, I requested the colored diff output. And it is implemented in the second homework. And for the second homework, I requested and provided the workflow at the feedback section. However, I am running out of ideas for writing the feedback section in this homework.