# Cache-Simulator
 
This project is done in C++. I have designed L1 and L2 cache in this project. It has capability to modify cache size, associativity and block size. Also cache can be customized to have split or combined cache (data cache and instruction cache). Hit rate, miss rate of L1 and L2 cache is reported as output. The output accuracy can be compared with <a href="http://pages.cs.wisc.edu/~markhill/DineroIV/">DineroIV</a>.

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
