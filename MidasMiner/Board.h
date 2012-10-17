//
//  Board.h
//  MidasMiner
//
//  Created by Tzu-Chien Chiu on 10/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MidasMiner_Board_h
#define MidasMiner_Board_h

#include <vector>
#include <string>
#include <assert.h>
#include "BoardDelegate.h"

class Matrix
{
private:
    std::vector<unsigned> m_data; // Row-major order
    unsigned m_columns, m_rows;
    
public:
    
    Matrix(unsigned rows = 0, unsigned columns = 0) : m_columns(columns), m_rows(rows)
    {
        m_data.resize(rows * columns, 0);
    }
    
    void initWithElements(unsigned rows, unsigned columns, ...) {
        va_list vl;
        va_start(vl, columns);
        m_columns = columns;
        m_rows = rows;
        unsigned elements = m_columns * m_rows;
        m_data.clear();
        for (unsigned i = 0; i < elements; ++ i)
            m_data.push_back(va_arg(vl, unsigned));
        
        va_end(vl);
    }
    
    bool inside(unsigned y, unsigned x) const {
        return y < m_rows & x < m_columns;
    }
    
    unsigned operator () (unsigned y, unsigned x) const {
        assert(inside(y, x));
        unsigned index = y * m_columns + x;
        return m_data[index];
    }
    
    unsigned& operator () (unsigned y, unsigned x) {
        assert(inside(y, x));
        unsigned index = y * m_columns + x;
        return m_data[index];
    }
    
    unsigned columns() const { return m_columns; }
    unsigned rows() const { return m_rows; }
    unsigned elements() const { return m_columns * m_rows; }
    unsigned getElementByIndex(unsigned index) const {
        assert(index < m_data.size());
        return m_data[index];
    }
    
    bool operator == (const Matrix& other) const { return m_data == other.m_data; }
    
    std::string string() const;
};


class RandomNumberGenerator;


class Board
{
    
private:
    typedef unsigned int DiamondIndex;
    static const unsigned HOLE = 0;

    
public:
    Board() : m_delegate(&m_defaultDelegate) {}
    Board(const Matrix& m) : m_diamondMatrix(m), m_delegate(&m_defaultDelegate) {}
 
    void setDelegate(BoardDelegate* delegate) { m_delegate = delegate; }
    
    void initRandomly(unsigned size, unsigned diamondTypes, 
                      RandomNumberGenerator& randNumGenerator);
    
    void swap(const DiamondCoords& d1, const DiamondCoords& d2);
    
    // Swap the positions of the two elements at (y1, x1) and (y2, x2)
    // Return false if they cannot be swapped, true otherwise.
    void swap(unsigned y1, unsigned x1, unsigned y2, unsigned x2) {
        swap(DiamondCoords(y1, x1), DiamondCoords(y2, x2));
    }
    
    enum Animaton { ANIMATION_SWAPPING, 
                    // when a swap doesn't create lines, the swap isn't valie
                    // and the diamonds are automatically swapped back.
                    ANIMATION_SWAPPING_BACK, 
                    ANIMATION_REMOVING, ANIMATION_FALLING };
    void onAnimationFinished(Animaton ani);
    
    
// These methods are made public only for unit tests
public:    
    unsigned findLines(Lines* result = NULL) const { return Board::findLines(m_diamondMatrix, result); }
    static unsigned findLines(const Matrix& matrix, Lines* result = NULL);
    
    // Move each column independently "downwards" (toward to last row) until all empty diamonds in the 
    // Board are occupied. The new diamonds enter from the top and are suppiled by m_futureMatrix.
    // The board keeps collapsing until there is no line.
    void collapse(bool firstIteration = true);
    
    CoordsArray generateCoordinatesOfVerticalLine(const DiamondCoords& top, const DiamondCoords& bottom);
    

//
// Getters similiar to Matrix ones. Board doesn't inherit from Matrix because I don't want the 
// internal matrix to be modified by Matrix's setters.
//
public:
    unsigned size() const {
        assert(columns() == rows());
        return rows();
    }
    
    unsigned columns() const { return m_diamondMatrix.columns(); }
    unsigned rows() const { return m_diamondMatrix.rows(); }

    unsigned operator () (unsigned y, unsigned x) const {
        return m_diamondMatrix(y, x);
    }
    
private:
    // It contains the index to m_diamondTexture array.
    Matrix m_diamondMatrix;
    
    BoardDelegate* m_delegate;
    DefaultBoardDelegate m_defaultDelegate;
    
    static unsigned getMaxElement(const Matrix& matrix) {
        unsigned maxElement = 0;
        for (unsigned i = 0; i < matrix.elements(); ++ i) {
            unsigned element = matrix.getElementByIndex(i);
            if (element > maxElement) maxElement = element;
        }
        assert(maxElement > 0);
        return maxElement;
    }
    
    static void initMatrixRandomly(Matrix& matrix, unsigned diamondTypes, RandomNumberGenerator& rand);
    static bool isLineCreatedByAddingDiamond(const Matrix& matrix, unsigned y, unsigned x, unsigned diamond);
    
    bool hasHole() const
    {
        for (unsigned i = 0; i < m_diamondMatrix.elements(); ++ i)
            if (HOLE == m_diamondMatrix.getElementByIndex(i)) return true;
        return false;
    }
    
    bool removeLines();
    
    void moveBoardDownwardOneStep();
    // (row, col) must be a hole.
    void moveColumnDownward(unsigned row, unsigned col);
    void rotateColumnDownward(Matrix& matrix, unsigned col);
    
    unsigned m_iteration;
    
    DiamondCoords m_lastSwappedDiamonds[2];
    
    // This arrary is indexed by the column, each element is the row index of
    // the last known empty diamond. The element -1 means this column has no empty
    // diamond.
    std::vector<int> m_rowsOfLastKnownEmptyDiamond;
    
    void onAnimationSwappingFinished(); 
    void onAnimationRemovingFnished();
    void onAnimationFallingFinished();
//
// Test helper functions
//
public:
    void initWithMatrix(const Matrix& m) { m_diamondMatrix = m; }
    void setFutureMatrix(const Matrix& m) { m_futureMatrix = m; }
    const Matrix& matrix() const { return m_diamondMatrix; }
    
private:
    // This matrix contains the "future" diamond which will enter the
    // bord when it coolapse. The last row of Future Matrix enters the 
    // board first from the top. 
    //
    // Imagine that the first row of the borad is right below the last row 
    // of the future matrix:
    //
    //  |  4, 5, 4, 5   -----+
    //  |  5, 4, 5, 4        +-- Future Matrix
    //  |  4, 5, 4, 5   -----+
    //  |  1, 1, 1, 2   --+
    //  |  0, 0, 0, 1     +-- Board
    //  |  1, 0, 1, 2     |
    //  |  2, 0, 2, 1   --+
    //  V
    // Collapse direction
    //
    // Each column is a rotated list. When the element of the last row is rotated
    // out, it fill the top row of the board and the future matrix. In the real 
    // game play, this matrix should contains many rows so that the user
    // should not notice this psuedo randomness. 
    //
    // This design also makes the unit test easier.
    //
    Matrix m_futureMatrix;
};


#endif
