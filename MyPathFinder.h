//
//  MyPathFinder.h
//  MicromouseMazeSolver
//
//  Created by Achinthya Poduval on 4/5/20.
//  Copyright © 2020 UCLA. All rights reserved.
//

#ifndef MyPathFinder_h
#define MyPathFinder_h

#include "Dir.h"

class MyPathFinder : public PathFinder {
public:
    MyPathFinder(bool shouldPause = false);

    MouseMovement nextMovement(unsigned x, unsigned y, const Maze &maze);
    

protected:
    struct Coord{
        Coord(unsigned x, unsigned y){
            m_x = x;
            m_y = y;
        }
        unsigned m_x;
        unsigned m_y;
    };
    
    // Helps us determine that we should go forward if we have just turned left.
    bool shouldGoForward;

    // Helps us determine if we've made a loop around the maze without finding the center.
    bool visitedStart;

    // Indicates we should pause before moving to next cell.
    // Useful for command line usage.
    const bool pause;

    bool isAtCenter(unsigned x, unsigned y) const;
    
    // wall data storage arrays
    bool vertWalls[16][17];
    bool horizWalls[17][16];
    
    // Manhattan Distance storage arrays
    unsigned MDs[16][16];
    
    // Method to perform floodfill calculations
    void doFloodfill(unsigned x, unsigned y, const Maze &maze);
    
    // Method to return array position in direction wanted
    void arrayPosInDirection(unsigned& x, unsigned& y, Dir dir, bool updateWall = false);
};

#endif /* MyPathFinder_h */
