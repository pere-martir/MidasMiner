//
//  BoardDelegate.h
//  MidasMiner
//
//  Created by Tzu-Chien Chiu on 10/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MidasMiner_BoardDelegate_h
#define MidasMiner_BoardDelegate_h

#include <stdlib.h>
#include <vector>

struct DiamondCoords 
{
    int row, col;
    
    // The coordinates are in the row-major order because they are 
    // the subscripts of Matrix, which is also row-major.
    DiamondCoords(int r = 0, int c = 0) : row(r), col(c) {}
    
    bool operator == (const DiamondCoords& other) const {
        return row == other.row && col == other.col;
    }
    
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


typedef std::vector<DiamondCoords> CoordsArray;
typedef std::vector<CoordsArray> Lines;

class Board;

class BoardDelegate
{
public:
    virtual ~BoardDelegate() {}
    
    virtual void onDiamondsSwapped(Board* sender, 
                                   const DiamondCoords& d1, const DiamondCoords& d2) = 0;
    
    virtual void onPreviousSwapCancelled(Board* sender,
                                         const DiamondCoords& d1, const DiamondCoords& d2) = 0;

    virtual void onDiamondsRemoved(Board* sender) = 0;
    
    // Notify thes diamonds have been moved. For each diamond, its current coordinates are 
    // stored in 'toCoordsArray', and its previous coordinates in 'fromCoordsArray' of the 
    // same array index. 'toCoordsArray' and 'fromCoordsArray' will always have the same size.
    virtual void onDiamondsFallen(Board* sender, 
                                  const CoordsArray& fromCoordsArray, 
                                  const CoordsArray& toCoordsArray) = 0;
};


class DefaultBoardDelegate : public BoardDelegate
{
public:
    
    virtual void onDiamondsSwapped(Board* sender, 
                                   const DiamondCoords& d1, const DiamondCoords& d2);
    
    virtual void onPreviousSwapCancelled(Board* sender,
                                         const DiamondCoords& d1, const DiamondCoords& d2) {}
    
    virtual void onDiamondsRemoved(Board* sender);
    
    virtual void onDiamondsFallen(Board* sender, 
                                  const CoordsArray& fromCoordsArray, 
                                  const CoordsArray& toCoordsArray);
};



#endif
