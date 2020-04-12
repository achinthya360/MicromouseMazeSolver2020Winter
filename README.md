# A Micromouse Maze Simulator

Useful for testing pathfinding algorithms without having to debug embedded devices.

## Compilation

### Mac/OSX, Linux

If you are on a Unix system you can use the provided Makefile:
* `make` will compile the code and create an executable `main`.
* `make run` will compile the code and run the simulation, waiting for an <ENTER> prompt after each move.
* `make sim` will compile the code and run the simulation from start to finish, without prompting.
  
  The make functionality does not currently work...

### Windows

The easiest way is to import the project into Visual Studio C++. If you would rather use the Makefile, you can use the bash subsystem or use a package manager like Scoop to download CMake or Make.

## Getting started

Create a new class and inherit from the abstract class `PathFinder`. You will need to implement the `MouseMovement nextMovement(unsigned x, unsigned y, const Maze &maze)` method so that you can instruct the maze how you want the mouse to navigate.

Then pass an instance of your class to the Maze and call `maze.start()` to start the simulation!

Check out the default `main.cpp` for an example of how to get a simulation running.
