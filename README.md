# Hash-Table

A Hash table is defined as a data structure used to insert, look up, and remove key-value pairs quickly. It operates on the hashing concept, where each key is translated by a hash function into a distinct index in an array. The index functions as a storage location for the matching value. In simple words, it maps the keys with the value. More about Hash Tables you can read [here](https://www.geeksforgeeks.org/hash-table-data-structure/). 
<br />
<br />
This codebase encompasses the implementation of a hash table data structure featuring double hashing for efficient collision resolution. Notably, the design allows for adaptability, enabling the hash table to dynamically resize itself based on fill ratio and access statistics, ensuring sustained efficiency.

> The main objective of this project was to gain hands-on experience with hash tables and specifically explore the implementation of operations like inserting and deleting elements, resolving collisions through double hashing and maintaining the table's efficiency.
> 
> This project was developed as the [third university assignment](instructions.pdf) for "Algorithms and Data Structures 2", University of Belgrade School of Electrical Engineering majoring Software Engineering. Please refer to the file for detailed assignment instructions.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Usage](#usage)
  - [Menu Options](#menu-options)
- [File Format](#file-format)




## File format
When creating new test files, the format should be as follows:
```
<value> <\t> <key>
```
* **value** is of type *string*
* **\t** represents a *tab* between value and key 
* **key** is of type *long int*

## Files
This project includes 3 examples (test files), filenames [`test.txt`](Files/test.txt), [`test100.txt`](Files/test100.txt) and [`DZ3_Recnik_10K.txt.txt`](Files/DZ3_Recnik_10K.txt.txt).
 <br />
