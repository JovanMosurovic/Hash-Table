# Hash-Table

A Hash table is defined as a data structure used to insert, look up, and remove key-value pairs quickly. It operates on the hashing concept, where each key is translated by a hash function into a distinct index in an array. The index functions as a storage location for the matching value. In simple words, it maps the keys with the value. More about Hash Tables you can read [here](https://www.geeksforgeeks.org/hash-table-data-structure/). 
<br />
<br />
This codebase encompasses the implementation of a hash table data structure featuring double hashing for efficient collision resolution. Notably, the design allows for adaptability, enabling the hash table to dynamically resize itself based on fill ratio and access statistics, ensuring sustained efficiency.

> The main objective of this project was to gain hands-on experience with hash tables and specifically explore the implementation of operations like inserting and deleting elements, resolving collisions through double hashing and maintaining the table's efficiency.
> 
> This project was developed as the [third university assignment](instructions.pdf) for "Algorithms and Data Structures 2" at the University of Belgrade School of Electrical Engineering majoring in Software Engineering. Please refer to the file for detailed assignment instructions.

## Table of Contents

- [Features](#features)
  - [Hash Table Operations](#hash-table-operations)
  - [Collision resolution](#collision-resolution)
  - [Adaptive Hash Table](#adaptive-hash-table)
  - [Performance Evaluation](#performance-evaluation)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Usage](#usage)
- [File Format](#file-format)

## Features

### Hash Table Operations

The conceptual class diagram is shown in the following image:

![diagram](https://github.com/JovanMosurovic/Hash-Table/assets/128632553/74d7d69e-20f1-49c9-8786-09aa32c1b648)

By the given instructions, these next functions are implemented:

| Function | Description |
|---|---|
| `findKey(Key k)` | Finds the given key and returns a pointer to the corresponding string (0 if the key is not found in the table). |
| `insertKey(Key k, string s)` | Inserts a key and its accompanying information into the table and returns the status (true for successful insertion, false for unsuccessful insertion). Prevents insertion of an existing key. |
| `deleteKey(Key k)` | Deletes a key from the table and returns the success status (true for successful deletion, false for unsuccessful deletion). |
| `avgAccessSuccess()` | Returns the average number of table accesses when successfully searching for keys. |
| `avgAccessUnsuccess()` | Returns the average number of table accesses when unsuccessfully searching for a key, calculated based on the number (up to that point) of unsuccessful table accesses and the number of keys not found in the table. |
| `resetStatistics()` | Sets all data needed for counting accesses for determining the average number of accesses for unsuccessful key search to the initial value. |
| `clear()` | Empties the table (deletes all keys). |
| `keyCount()` | Returns the number of inserted keys. |
| `tableSize()` | Returns the size of the table. |
| `operator<<` | Prints the contents of the table to the standard output, one table entry per line. Empty table entries should be marked with "EMPTY". |
| `fillRatio()` | Returns the degree of table fullness (real number between 0 and 1). |

## Collision resolution
The class representing the abstraction of the address function that the hash table will use for resolving collisions is specified to the hash table object during its creation.
This abstract class defines the interface for the address function used by a hash table to resolve collisions. The hash table must provide an object of this class during its creation.

```
Address getAddress(Key k, Address a, Attempt i) 
```

Parameters:
- `k`: The key.
- `a`: The base address.
- `i`: The attempt number of access.

This method returns a new address where the key should be looked for (or the location where it is needed to accommodate). The returned address **may be outside** the address range of the table. A hash table that uses this class should take care of it and reduce the returned address to the correct one range. These derived classes should specify how to determine the next address.

### Double hashing Class
The double hashing class is parameterized by the values of `p` and `q` and returns the following value:

```
return_address = a + i · (q + (k mod p))
```

## Adaptive Hash Table
The hash table has an adaptive mode, controlled by the user. When in adaptive mode, the table automatically adjusts its size based on load factors and average access counts. You can enable/disable this option through the menu. The initial mode of the table is not adaptive due to the homework requirement from the faculty.

## Performance Evaluation
A performance evaluation of a hash table is performed by inserting the given keys (in a specified range of values) into a given hash table, and then generating a specified number of keys of **pseudorandom** values and performing a search on them. After that, the results (average number of accesses in a **successful** search and calculated number of accesses in an **unsuccessful** search) are printed.<br />

The range in which random numbers are generated is determined so that it corresponds to the range of values of the keys inserted into the table (in this case these numbers are between minimum and maximum key in "[DZ3_Recnik_10K.txt](Files/DZ3_Recnik_10K.txt)" file.

## Getting Started

### Prerequisites
Before you begin, ensure you have the following prerequisites:

- C++ Compiler: Make sure you have a C++ compiler installed on your system.
- Ensure that your terminal supports ASCII escape codes to view the colorized and formatted output correctly.

### Compilation and Execution

1. Ensure you have a C++ compiler installed.
2. Clone this repository.
3. Navigate to the directory containing the source code.
4. Compile the code.
5. Run the compiled program.

 ### Usage

Upon execution, the program presents a menu with various options, as explained in [Features](#features) section, to interact with the user. Follow the on-screen prompts to perform operations.


## File format
When creating new test files, the format should be as follows:
```
<value> <\t> <key>
```
* **value** is of type *string*
* **\t** represents a *tab* between value and key 
* **key** is of type *long int*

This project includes 3 examples (test files), named [`test.txt`](Files/test.txt), [`test100.txt`](Files/test100.txt) and [`DZ3_Recnik_10K.txt`](Files/DZ3_Recnik_10K.txt) (main file given by faculty).




