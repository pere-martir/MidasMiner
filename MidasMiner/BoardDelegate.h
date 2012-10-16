//
//  BoardDelegate.h
//  MidasMiner
//
//  Created by Tzu-Chien Chiu on 10/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MidasMiner_BoardDelegate_h
#define MidasMiner_BoardDelegate_h

struct DiamondCoords 
{
    unsigned row, col;
    
    // The coordinates are in the row-major order because they are 
    // the subscripts of Matrix, which is also row-major.
    DiamondCoords(unsigned r = 0, unsigned c = 0) : row(r), col(c) {}
    
    bool isAdjacentTo(const DiamondCoords& other) const {
        if (row == other.row) {
            return 1 == abs(col - other.col);
        } else if (col == other.col) {
            return 1 == abs(row - other.row);
        } else {
            return false;
        }
    }
};


typedef std::vector<DiamondCoords> Coords;
typedef std::vector<Coords> Lines;

class Board;

class BoardDelegate
{
public:
    virtual ~BoardDelegate() {}
    
    // Notify thes diamonds have been moved. For each diamond, its current coordinates are 
    // stored in 'toCoordsArray', and its previous coordinates in 'fromCoordsArray' of the 
    // same array index. 'toCoordsArray' and 'fromCoordsArray' will always have the same size.
    virtual void onDiamondsMoved(const Board* sender, 
                                 const Coords& toCoordsArray, 
                                 const Coords& fromCoordsArray) {}
    
    //virtual void onLinesRemoved(const Board* sender) = 0;
};


#endif
