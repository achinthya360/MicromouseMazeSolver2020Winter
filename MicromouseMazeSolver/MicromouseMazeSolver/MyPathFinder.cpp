//
//  MyPathFinder.cpp
//  MicromouseMazeSolver
//
//  Created by Achinthya Poduval on 4/5/20.
//  Copyright © 2020 UCLA. All rights reserved.
//

/*
enum MouseMovement {
     MoveForward,            // Move in the direction mouse is facing
     MoveBackward,           // Move opposite of the direction mouse is facing
     TurnClockwise,          // Self explanatory
     TurnCounterClockwise,   // Self explanatory
     TurnAround,             // Face the opposite direction currently facing
     Wait,                   // Do nothing this time, do some computation, then try again later
     Finish                  // Mouse has achieved goals and is ending the simulation
};
*/

#include <stdio.h>
#include "PathFinder.h"
#include <stack>
#include <queue>
#include <iostream>
#include "MyPathFinder.h"
#include "Maze.h"
#include "MazeDefinitions.h"
using namespace std;

enum bestDir{
    Right,
    Left,
    Front,
    Behind,
    None
};

void PrintMaze(unsigned MDs[][16], bool horizWalls[][16], bool vertWalls[][17]);

MyPathFinder::MyPathFinder(bool shouldPause) : pause(shouldPause){
    shouldGoForward = false;
    visitedStart = false;
    
    // set vertical walls to False for no known walls except for exterior
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 17; j++){
            if(j == 0 || j == 16){
                vertWalls[i][j] = true;
            }
            else{
                vertWalls[i][j] = false;
            }
        }
    }
    
    // set horizontal walls to False for no known walls
    for(int h = 0; h < 17; h++){
        for(int m = 0; m < 16; m++){
            if(h == 0 || h == 16){
                horizWalls[h][m] = true;
            }
            else{
                horizWalls[h][m] = false;
            }
        }
    }
    
    // fill Manhattan Distances appropriately
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            unsigned md = 14 - i - j;
            MDs[i][j] = md;
            MDs[i][15 - j] = md;
            MDs[15 - i][j] = md;
            MDs[15 - i][15 - j] = md;
        }
    }
    
    PrintMaze(MDs, horizWalls, vertWalls);
}

MouseMovement MyPathFinder::nextMovement(unsigned x, unsigned y, const Maze &maze) {
    PrintMaze(MDs, horizWalls, vertWalls);
    // get wall readings
    bool wallLeft = maze.wallOnLeft();
    bool wallRight = maze.wallOnRight();
    bool wallFront = maze.wallInFront();

    // Pause at each cell if the user requests it.
    // It allows for better viewing on command line.
    if(pause) {
        std::cout << "Hit enter to continue..." << std::endl;
        std::cin.ignore(10000, '\n');
        std::cin.clear();
    }

    std::cout << maze.draw(5) << std::endl << std::endl;

    // If we somehow miraculously hit the center
    // of the maze, just terminate and celebrate!
    if(isAtCenter(x, y)) {
        std::cout << "Found center! Good enough for the demo, won't try to get back." << std::endl;
        return Finish;
    }

    // If we hit the start of the maze a second time, then
    // we couldn't find the center and never will...
    if(x == 0 && y == 0) {
        if(visitedStart) {
            std::cout << "Unable to find center, giving up." << std::endl;
            return Finish;
        } else {
            visitedStart = true;
        }
    }
    
    // do movement based on minimum MD and current wall readings
    Dir curDir = maze.heading;
    bestDir best = None;
    unsigned curx = x;
    unsigned cury = 15 - y;
    unsigned minDist = MDs[cury][curx];
    unsigned curDist = minDist;
    // update wall arrays and array positions in each direction
    // left
    unsigned leftx = curx, lefty = cury;
    arrayPosInDirection(leftx, lefty, counterClockwise(curDir), wallLeft);
    cout << "1";
    // right
    unsigned rightx = curx, righty = cury;
    arrayPosInDirection(rightx, righty, clockwise(curDir), wallRight);
    cout << "2";
    // front
    unsigned frontx = curx, fronty = cury;
    arrayPosInDirection(frontx, fronty, curDir, wallFront);
    cout << "3";
    // back (don't update any walls b/c no info here)
    unsigned backx = curx, backy = cury;
    arrayPosInDirection(backx, backy, opposite(curDir));
    cout << "4";
    
    // movement logic for cell to the left
    if(!wallLeft){
        if(MDs[lefty][leftx] < minDist){
            minDist = MDs[lefty][leftx];
            best = Left;
        }
    }
    cout << "5";
    
    // movement logic for cell to the right
    if(!wallRight){
        if(MDs[righty][rightx] < minDist){
            minDist = MDs[righty][rightx];
            best = Right;
        }
    }
    cout << "6";
    
    // movement logic for cell to the front
    if(!wallFront){
        cout << "row: " << fronty << " col: " << frontx << endl;
        if(MDs[fronty][frontx] <= minDist){
            cout << "10";
            minDist = MDs[fronty][frontx];
            best = Front;
            cout << endl << "Best in Front!" << endl;
        }
    }
    cout << "7";
    
    
    // movement logic for cell behind (no check for walls)
    if((best == None) && MDs[backy][backx] < minDist){
        minDist = MDs[backy][backx];
        best = Behind;
    }
    cout << "8";
    
    //When none of the adjacent traversible cells has a lower distance than the current cell.
    if(minDist >= curDist){
        doFloodfill(curx, cury, maze);
        cout << "floodfill time!";
        return Wait;
    }
    
    switch (best) {
        case Left:
            cout << "Left Turn!";
            return TurnCounterClockwise;
            break;
        case Right:
            cout << "Right Turn!";
            return TurnClockwise;
            break;
        case Front:
            cout << "Forward!";
            return MoveForward;
            break;
        case Behind:
            cout << "Turn Around!";
            return TurnAround;
        default:
            break;
    }

    // If we get stuck somehow, just terminate.
    std::cout << "Got stuck..." << std::endl;
    return Finish;
}

void MyPathFinder::doFloodfill(unsigned x, unsigned y, const Maze &maze)
{
    //When none of the adjacent traversible cells has a lower distance than the current cell.
    stack<Coord> toProcess; //Stack of cells to be processed (can also use queue)
    Coord curCell(x, y);
    toProcess.push(curCell);
    unsigned minDist;
    unsigned curDist;
    while(!toProcess.empty()){
        curCell = toProcess.top();
        toProcess.pop();
        curDist = MDs[curCell.m_y][curCell.m_x];
        if(curDist == 0){
            cout << "curDist is zero yee!" << endl;
            continue; //don’t want to process the end goal
        }
        minDist = UINT_MAX; //placeholder/”invalid” distance
        unsigned curx = curCell.m_x;
        unsigned cury = curCell.m_y;
        
        /* following code's purpose:
         for each neighboring cell of cur:
         if no wall between cur and neighbor:
             if neighbor.distance < min_distance:
                     min_distance = neighbor.distance
         */
    
        // left cell open
        if(vertWalls[cury][curx] == false){
            if(MDs[cury][curx - 1] < minDist){
                minDist = MDs[cury][curx-1];
            }
        }
        // right cell open
        if(vertWalls[cury][curx+1] == false){
            if(MDs[cury][curx + 1] < minDist){
                minDist = MDs[cury][curx + 1];
            }
        }
        // cell above open
        if(horizWalls[cury][curx] == false){
            if(MDs[cury - 1][curx] < minDist){
                minDist = MDs[cury - 1][curx];
                cout << "bruh moment";
            }
        }
        // cell below open
        if(horizWalls[cury+1][curx] == false){
            if(MDs[cury + 1][curx] < minDist){
                minDist = MDs[cury + 1][curx];
            }
        }
        
        if(minDist == UINT_MAX){ //something went wrong. terminate
            cout << "something went wrong, terminate!" << endl;
            continue;
        }
        if(curDist > minDist){ //everything is fine, move on
            cout << "everything is fine, move on!" << endl;
            continue;
        }
        if(curDist <= minDist){ //we reach this point
            MDs[cury][curx] = minDist + 1; //new minimum distance
            cout << "floodfill did something yay!" << endl;
            
            // push all neighboring cells to stack
            // left cell:
            if(vertWalls[cury][curx] == false){
                Coord c(curx - 1, cury);
                toProcess.push(c);
            }
            // right cell
            if(vertWalls[cury][curx + 1] == false){         // might have to fix these lines if implementation doesn't work because of not all neighboring cells are pushed
                Coord c(curx + 1, cury);
                toProcess.push(c);
            }
            // cell above
            if(horizWalls[cury][curx] == false){
                Coord c(curx, cury - 1);
                toProcess.push(c);
            }
            // cell below
            if(horizWalls[cury + 1][curx] == false){
                Coord c(curx, cury + 1);
                toProcess.push(c);
            }
        }
    }
}

bool MyPathFinder::isAtCenter(unsigned x, unsigned y) const {
    unsigned midpoint = MazeDefinitions::MAZE_LEN / 2;

    if(MazeDefinitions::MAZE_LEN % 2 != 0) {
        return x == midpoint && y == midpoint;
    }

    return  (x == midpoint     && y == midpoint    ) ||
    (x == midpoint - 1 && y == midpoint    ) ||
    (x == midpoint     && y == midpoint - 1) ||
    (x == midpoint - 1 && y == midpoint - 1);
}

void PrintMaze(unsigned MDs[][16], bool horizWalls[][16], bool vertWalls[][17]){
    cout << endl;
    for(int i = 0; i < 16; i++){
        // print horizontal walls on top
        cout << "     ";
        for(int k = 0; k < 16; k++){
            if(horizWalls[i][k] == true)
                cout << "--" << "     ";
        }
        cout << endl;
        
        // print manhattan distances
        for(int j = 0; j < 16; j++){
            // print vertical wall to the left
            if(vertWalls[i][j] == true)
                cout << "  |  ";
            else
                cout << "     ";
            
            cout << MDs[i][j];
            if((MDs[i][j] / 10) < 1)
                cout << " ";
        }
        // print vertical wall to right
        if(vertWalls[i][16] == true)
            cout << "|";
        cout << endl;
    }
    
    // print last line of horizontal walls
    cout << "  ";
    for(int k = 0; k < 16; k++){
        if(horizWalls[16][k] == true)
            cout << "   --" << "  ";
    }
    cout << endl;
}

void MyPathFinder::arrayPosInDirection(unsigned& x, unsigned& y, Dir dir, bool updateWall){
    if(dir == NORTH){
        if(updateWall){
            horizWalls[y][x] = true;
            cout << "9";
        }
        y--;
    }
    else if(dir == EAST){
        x++;
        if(updateWall){
            vertWalls[y][x] = true;
        }
    }
    else if(dir == WEST){
        if(updateWall){
            vertWalls[y][x] = true;
        }
        x--;
    }
    else if(dir == SOUTH){
        y++;
        if(updateWall){
            horizWalls[y][x] = true;
        }
    }
}
