# Word Search Solver

## Description
This project is a multi-threaded word search solver implemented in C. It uses a hashset to store a dictionary of words and a binary search tree (BST) to store found words. 
The project demonstrates the use of complex data structures, concurrency with pthreads, and low-level file handling.

## Features
- Multi-threaded word search solver
- Hashset for efficient dictionary word storage
- Binary search tree for storing and sorting found words
- Handles large input files with custom memory management
- Error handling and command line argument processing

## Technologies Used
- C
- Pthreads

## How to Use
1. Clone the repository:
    ```bash
    git clone https://github.com/arauzemilio22/Word-Search-Solver
    ```
2. Navigate to the project directory:
    ```bash
    cd WordSearchSolver
    ```
3. Compile the project using the provided Makefile:
    ```bash
    make
    ```
4. Run the solver with the required arguments:
    ```bash
    ./solve -dict dict.txt -input puzzle1mb.txt -size 1000 -nbuffer 64 -len 4:7 [-s]
    ```

## Command Line Arguments
- `-dict dict.txt`: Path to the dictionary file.
- `-input puzzle1mb.txt`: Path to the input puzzle file.
- `-size 1000`: Size of the puzzle.
- `-nbuffer 64`: Number of buffers to use.
- `-len 4:7`: Minimum and maximum length of words to search for.
- `-s`: Optional flag to sort the output.

## License
MIT License
