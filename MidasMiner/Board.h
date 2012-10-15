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
#include <set>
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
    
    Matrix(unsigned columns = 0, unsigned rows = 0) : m_columns(columns), m_rows(rows)
    {
        m_data.resize(rows * columns, 0);
    }
    
    void initWithElements(unsigned columns, ...) {
        va_list vl;
        va_start(vl, columns);
        m_columns = columns;
        m_rows = m_columns;
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
};


class RandomNumberGenerator;

class Board
{
    
private:
    typedef unsigned int DiamondIndex;
    size_t m_diamondTypes;
    
public:
    Board(); 
    Board(const Matrix& m) : m_diamondMatrix(m) 
    {
        std::set<unsigned> diamonds;
        for (unsigned i = 0; i < m_diamondMatrix.elements(); ++ i) {
            diamonds.insert(m_diamondMatrix.getElementByIndex(i));
        }
        m_diamondTypes = diamonds.size();
    }
    
    void initRandomly(unsigned size, unsigned diamondTypes, 
                      RandomNumberGenerator& randNumGenerator);
    unsigned findLines() const;
    
   
    unsigned columns() const { return m_diamondMatrix.columns(); }
    unsigned rows() const { return m_diamondMatrix.rows(); }
    
    // Swap the positions of the two elements at (y1, x1) and (y2, x2)
    // Return false if they cannot be swapped, true otherwise.
    bool swap(unsigned y1, unsigned x1, unsigned y2, unsigned x2);
    
    unsigned operator () (unsigned y, unsigned x) const {
        return m_diamondMatrix(y, x);
    }

// Test helper functions
public:
    void initWithMatrix(const Matrix& m) { m_diamondMatrix = m; }
    const Matrix& matrix() const { return m_diamondMatrix; }
    
private:
    // It contains the index to m_diamondTexture array.
    Matrix m_diamondMatrix;
};


#endif
