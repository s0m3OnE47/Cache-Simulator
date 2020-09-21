# Cache-Simulator
 
Designed various branch predictors using C++.

- One Level Branch Predictor: Simple branch predictor depending on previous state. Built 2-bit saturating counter with four states: Strongly not taken, weakly not taken, weakly taken, strongly taken.

- 2-bit Counter State Machine

- Two Level Global Branch Predictor: Correlation between different conditional jumps is taken into consideration while making the predictions. A global history register (GHR) was introduced in this branch predictor.
- Two Level Gshare Branch Predictor: In this branch predictor, GHR and program counter is XORed to get pattern history table (PHT).
- Two Level Local Branch Predictor: Address of the current instruction is taken into consideration for PHT.
- Tournament Branch Predictor: Uses more than one type of predictor (i.e., multiple algorithms) and selects the “best”prediction

For testing of branch predictor, I took a trace file of conditional branches from execution of GCC program from SPECint2000 benchmark suite. The Pattern History Table size is 1KB with 2-bit counter. After running different branch predictors, it was observed that masking 4 bits gave the best results i.e highest hit rate among others. After result analysis, I conclude that one level branch predictor is best suited to this application as this gives the least miss rate.

## Compile

Compile source code using 
```
g++ cache_simulator.cpp
```

## Run

Run using
```
./a.out > output.txt
```

Output will be stored in output.txt file. 

## Clean

Delete the file using
```
rm output.txt a.out
```
