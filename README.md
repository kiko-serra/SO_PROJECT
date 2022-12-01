# SO_PROJECT

## Description
To compile and run each program, run the following command inside the corresponding folder:
```
make
```
## Q1
For Q1 after compiling, you can either run:
```
$ make run
```
that runs `./samples quote.txt 4 8`.  Or manually by:
```
$ ./samples quote.txt n m
```
Where n is the number of fragments and m is the number of chatacters.

## Q2
For Q2 after compiling,you can either run:
```
$ make run
```
that runs `./txt2epub f1.txt f2.txt f3.txt f4.txt`.  Or manually by:
```
$ ./txt2epub f1.txt f2.txt f3.txt ... fn.tx
```
Where n is the last file you wish to process.

## Q3
For Q3 after compiling, you can either run:
```
$ make run
```
that runs `./tokenring 5 0.01 10`.  Or manually by:
```
$ ./tokenring n p t
```
Where n is the number of pipes, p is the probability and t is the waiting time.


## Clean
To clean each folder, run the following command:
```
make clean
```
## Valgrind
To run valgrind on each program (with the recommended flags), run the following command:
```
make valgrind
```

Francisco Pimentel Serra 202007723 up202007723@fe.up.pt<br>
Pedro Miguel da Silva Correia 202006199 up202006199@fe.up.pt<br>
Guilherme de Sousa Ribeiro 202108731 up202108731@fe.up.pt<br>
João Pedro Silva Duarte 201504089 up201504089@fe.up.pt<br>
