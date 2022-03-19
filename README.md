# Set-Calculator
### VUT University : IZP_2021 Project2

# Description

The aim of the project is to create a program that will implement basic mathematical operations on sets and binary relations. The input of the program will be text data representing sets and sessions and input operations. The program will print the result of processing on standard output.

## (notices)
It was supposed to be a team project, but due to circumstances, I completed it by myself.

## Input file format
The text file consists of three consecutive parts:

1. Universe definition - one line starting with "U" and continuing with space separated elements,
2. Definition of sets and relations - one or more lines starting with "S" or "R" and continuing with space-separated elements (line starting with "S" indicates the definition of the set, "R" is used to define the relation),
3. Commands over sets and relations - one or more lines starting with "C" and continuing with the command identifier.

#### Universe
The elements of the universe are strings containing lowercase and uppercase letters of the English alphabet. The length of the string is a maximum of 30 characters. Universe elements cannot contain command identifiers (see below) and the keywords true and false. All elements in sets and relations must belong to the universe. 

#### Sets
Each set is defined on one line by space-separated elements from the universe. The set identifier is the line number on which the set is defined (since the first line of the file contains a universe, the set identifiers therefore start with the number 2). Set identifiers are used in operations (see below).

#### Relation
Each relation is defined by an enumeration of pairs. The pair is enclosed in parentheses, the first and second elements of the pair are separated by a space. Each pair is separated by a space.

#### Commands
Each command is defined on one line, starting with the command identifier, and the command arguments are separated by a space (from the identifier and each other). The arguments of the command are numeric identifiers of sets and relations (positive integers, the number 1 identifies the universe set).

#### Commands over sets
The command works on sets and its result is either a set (in which case it prints the set in the same format as expected in the input file, ie it starts with "S" and continues with space separated elements) or the result is a truth value (in this case it prints true or false on a single line) or is the result of a natural number (which is printed on a single line).

- empty A - prints true or false depending on whether the set defined on line A is empty or non-empty.
- card A - prints the number of elements in the set A (defined on line A).
- complement A - prints the complement of set A.
- union A B - prints the unification of sets A and B.
- intersect A B - prints the intersection of sets A and B.
- minus A B - prints the difference of sets A \ B.
- subseteq A B - prints true or false depending on whether set A is a subset of set B.
- subset A B - prints true or false if set A is a proper subset of set B.
- equals A B - prints true or false if sets are equal.

#### Relations commands
The command works on sessions and results in either a truth value (prints true or false) or a set (prints a set in the format as in the input file).

- reflexive R - prints true or false if the relation is reflexive.
- symmetric R - prints true or false if the relation is symmetric.
- antisymmetric R - prints true or false if the relation is antisymmetric.
- transitive R - prints true or false if the relation is transitive.
- function R - prints true or false if the R relation is a function.
- domain R - prints the domain of the R function (can also be applied to relations - the first elements of pairs).
- codomain R - prints the range of values of the R function (can also be applied to relations - second elements of pairs).
- injective R A B - prints true or false if the R function is injective. A and B are sets; a∈A, b∈B, (a, b) ∈R.
- surjective R A B - prints true or false if the R function is surjective. A and B are sets; a∈A, b∈B, (a, b) ∈R.
- bijective R A B - prints true or false if the R function is bijective. A and B are sets; a∈A, b∈B, (a, b) ∈R.

## Implementation details
- The maximum number of rows supported is 1000.
- The order of the elements in the set and in the output session does not matter.
- All elements of sets and in relations must belong to the universe. If an element in a set or pair repeats in a session, this is an error.

## Detailed specifications
Implement the program in the source file setcal.c (Set Calculator). The input data will be read from a file whose name will be specified by the command line argument. The program performs the operations specified in the file line by line in one pass. The program prints the result of processing each line to standard output (that is, the number of lines in the output corresponds to the number of lines in the input file).

# Syntax to run the program
````
$ gcc -std=c99 -Wall -Wextra -Werror setcal.c -o setcal
./setcal FILE
````
- (./setcal indicates the location and name of the program)

#### FILE
name of file (especially test.txt)

# Project results
### 14/14 points
