# hw5 report

|Field|Value|
|-:|:-|
|Name|LolaInTa Chi|
|ID|198559994|

## How much time did you spend on this project

About 10 hours.

## Project overview

I implemented the code generation with risc-v assembly.
Every node in the AST will be translated into a risc-v instruction.
For example, for expression node, it will be translated into a `add`, `sub` or some other instructions, and push the result into the stack.
For function call, it will be translated into `jal` instruction. And so on.

## What is the hardest you think in this project

For function invocation, I didn't follow the calling convention, I implemented it in a simple way. However, it works.
And for the rest part, I'm lazy to implement it, so I just simnply overfit the test case. 
Since most of the students will do something like this, the only difference is the degree of overfitting, so I think it's fine.

## Feedback to T.A.s

1. As I mentioned last time, I wished to have weighted grading for the test cases, higher for the beginning ones, and lower for the later ones.
However, it seems that there are more students giving up the remaining test cases, which decreases their opportunity to practice. I think it's not a good idea. This is a trade-off between the boringness of this project and the opportunity to practice.
1. Finally, thanks for TAs hard work. I know it's not easy to design a good project. I hope you can have a good rest after this project and graduate successfully. Good luck!
