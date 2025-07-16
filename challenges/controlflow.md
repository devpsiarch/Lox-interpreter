# Using functional programming for control flow 

## conditional 
- we can use functions to impliment dispatch as we do in **lambda calculus**.

## loops 
- functions also can allow us to impliment loops and that is by recursion , only problem is that we are faced with the limitations of the stack.
what if we want to wait for a long time for something to happen ? or handle computations for a longer ? well a solution is **instead of using a new stack frame , we use the same one **
this is called the **Tail call optimization **.
