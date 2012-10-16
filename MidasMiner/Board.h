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



// NOT VALID COMMENT
#if 0
// The elements are stored in row-major order while the accessor accepts
// the parameters in column-major order. This is because it's easier to 
// stored the elements in row-major order, while I want to make the accessor
// coincide with the order of the coordinate system of OpenGL and that of the 
// mathematic subscripts of matrix elements, which is usually (x,y)
#endif
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
    
    bool isValidCoordinates(unsigned y, unsigned x) const {
        return y < m_rows & x < m_columns;
    }
    
    unsigned operator () (unsigned y, unsigned x) const {
        assert(isValidCoordinates(y, x));
        unsigned index = y * m_columns + x;
        return m_data[index];
    }
    
    unsigned& operator () (unsigned y, unsigned x) {
        assert(isValidCoordinates(y, x));
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


struct DiamondCoord 
{
    unsigned row, col;
    DiamondCoord(unsigned r = 0, unsigned c = 0) : row(r), col(c) {}
};

typedef std::vector<DiamondCoord> Line;
typedef std::vector<Line> Lines;

class RandomNumberGenerator;

class Board
{
    
private:
    typedef unsigned int DiamondIndex;
    static const unsigned HOLE = 0;

    
public:
    Board(); 
    Board(const Matrix& m) : m_diamondMatrix(m) {}
    
    void initRandomly(unsigned size, unsigned diamondTypes, 
                      RandomNumberGenerator& randNumGenerator);
    
    unsigned findLines(Lines* result = NULL) const { return findLines(m_diamondMatrix, result); }
    unsigned findLines(const Matrix& matrix, Lines* result = NULL) const;
    
    bool isLineCreatedByAddingDiamond(unsigned y, unsigned x, unsigned diamond);
    
    // Move each column independently "downwards" (toward to last row) until all empty diamonds in the 
    // Board are occupied. The new diamonds enter from the top and are suppiled by m_futureMatrix.
    // The board keeps collapsing until there is no line.
    void collapse();
  
//
// Matrix functions
//
public:
   
    unsigned columns() const { return m_diamondMatrix.columns(); }
    unsigned rows() const { return m_diamondMatrix.rows(); }
    
    // Swap the positions of the two elements at (y1, x1) and (y2, x2)
    // Return false if they cannot be swapped, true otherwise.
    bool swap(unsigned y1, unsigned x1, unsigned y2, unsigned x2);
    
    unsigned operator () (unsigned y, unsigned x) const {
        return m_diamondMatrix(y, x);
    }
    
private:
    // It contains the index to m_diamondTexture array.
    Matrix m_diamondMatrix;
    
    static unsigned getMaxElement(const Matrix& matrix) {
        unsigned maxElement = 0;
        for (unsigned i = 0; i < matrix.elements(); ++ i) {
            unsigned element = matrix.getElementByIndex(i);
            if (element > maxElement) maxElement = element;
        }
        assert(maxElement > 0);
        return maxElement;
    }
    
    bool hasHole() const
    {
        for (unsigned i = 0; i < m_diamondMatrix.elements(); ++ i)
            if (HOLE == m_diamondMatrix.getElementByIndex(i)) return true;
        return false;
    }
    
    bool removeLines();
    
    // (row, col) must be a hole.
    void moveColumnDownward(unsigned row, unsigned col);
    void rotateColumnDownward(Matrix& matrix, unsigned col);
    
    
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
