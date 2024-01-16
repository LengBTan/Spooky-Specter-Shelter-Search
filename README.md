#### Authors:
- Leng Tan, 101233921
- Kina Zhan, 101254727

# Spooky Specter Shelter Search Simulator
This program demonstrates the use of data structures, dynamic memory allocation, and multiple threads to run a scenario simulation of a game. The program outputs the logs and the result of the simulation in the terminal
## List of files:
- defs.h
- evidence.c
- ghost.c
- house.c
- hunter.c 
- logger.c
- main.c
- Makefile
- README.md
- room.c
- utils.c

## Compiling and Executing
1. Navigate to the directory containing the source code in a terminal
2. Enter the command `make` to create the executable file `run`
3. Enter the command `./run` while in the same directory 

## Usage instructions
- Once the program is executed, the program will prompt the user to enter the names of 4 hunters

    ```
    Please enter your hunter's name: Name1
    [HUNTER INIT] [Name1] is a [EMF] hunter
    Please enter your hunter's name: Name2
    [HUNTER INIT] [Name2] is a [EMF] hunter
    Please enter your hunter's name: Name3
    [HUNTER INIT] [Name3] is a [EMF] hunter
    Please enter your hunter's name: Name4
    [HUNTER INIT] [Name4] is a [EMF] hunter
    ```

- The terminal will then output the log of the simulation.
    - Logs that are printed out include:
        - Initialization of the ghost and the hunters
        - Movement of the ghost and the hunters
        - Hunters collecting and reviewing evidence
        - Ghosts leaving evidence in the room they are in
        - reason for a hunter or ghost to leaving a house
    - Once the simulation ends, the final results of the simulation will print out:
        - A list of hunters that have left the house due to fear or boredom
        - Whether the hunters or the ghost has won
        - a list of evidence that has been collected by the hunters
