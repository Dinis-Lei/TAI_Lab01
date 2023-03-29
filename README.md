# TAI_Lab01

## Copy model program

### How to compile

```bash
g++ -O3 -o bin/cpm src/cpm.cpp
```
### How to run

Below are the parameters to run the program.
All parameters are optional except the path to the file.

```bash
./bin/cpm -k 4 -n 2 -M 5 -m 0 -a 1 -t 0.5 -s -i <path to file>  
```
- parameter k: Size of the windows and anchors; default value is 4
- parameter n: Number of anchors; default value is -1, which means all anchors will be used
- parameter M: Maximum number of symbols for the anchor to evaluate; default value is INFINITY
- parameter m: Minimum number of symbols for the anchor to evaluate; default value is 0
- parameter a: alpha; smoothing parameter; default value is 1
- parameter t: cutoff threshold; default value is 0.5
- parameter s: save; save results to file
- parameter i: ignored last; if last guess was ignored
- parameter <path to file>: path to the file for the program to process  

### Example

```bash
./bin/cpm example/chry.txt
```

## Copy model generation program

### How to compile

```bash
g++ -O3 -o bin/cpm_gen src/cpm_gen.cpp
```

### How to run

Below are the parameters to run the program.
All parameters are optional except the path to the file.
Take note that the sample given as an argument should be an excerpt of the training file with K or bigger size.

```bash
./bin/cpm_gen -n 100 -k 4 -s <sample text> <path to file>  
```
parameter k: Size of the windows; default value is 4
parameter n: Number of symbols to be generated; default value is 100
parameter s: sample text; the program generate the sequence followed by the sample text 
parameter <path to file>: path to the file for the program to be based on for the generated sequecence 

### Example

```bash
./bin/cpm_gen example/chry.txt

```

## Copy model generation 2 program

### How to compile

```bash
g++ -O3 -o bin/cpm_gen2 src/cpm_gen2.cpp
```

### How to run

Below are the parameters to run the program.
All parameters are optional except the path to the file.
Take note that the sample given as an argument should be an excerpt of the training file with K or bigger size.

```bash
./bin/cpm_gen -n 100 -k 4 -s <sample text> <path to file>  
```
- parameter k: Size of the windows; default value is 4
- parameter n: Number of symbols to be generated; default value is 100
- parameter s: sample text; the program generate the sequence followed by the sample text 
- parameter <path to file>: path to the file for the program to be based on for the generated sequecence 


### Example

```bash
./bin/cpm_gen2 example/chry.txt

```
