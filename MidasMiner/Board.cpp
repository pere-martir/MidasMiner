
#include "Board.h"
#include "RandomNumberGenerator.h"

Board::Board()
{}

bool Board::isLineCreatedByAddingDiamond(unsigned y, unsigned x, unsigned diamond)
{
    Matrix matrix = m_diamondMatrix;
    assert(matrix(y, x) == 0);
    matrix(y, x) = diamond;
    return 0 != findLines(matrix);
}

void Board::initRandomly(unsigned size, unsigned diamondTypes, RandomNumberGenerator& rand)
{
    assert(diamondTypes > 0);
    m_diamondMatrix = Matrix(size, size);
    for (unsigned int i = 0; i < m_diamondMatrix.columns(); ++ i) {
        for (unsigned int j = 0; j < m_diamondMatrix.rows(); ++ j) {
            while (true) {
                unsigned diamond = 1 + rand.next() % diamondTypes;
                if (!isLineCreatedByAddingDiamond(j, i, diamond)) {
                    m_diamondMatrix(j, i) = diamond;
                    break;
                }
            }
        }
    }
}

unsigned Board::findLines(const Matrix& matrix) const
{
    unsigned diamondTypes = getMaxElement(matrix);
    unsigned linesFound = 0;
    for (unsigned d = 1; d <= diamondTypes; ++ d) {
        
        // Find horizonal lines
        for (unsigned i = 0; i < matrix.columns(); ++ i) {
            for (unsigned j = 0; j < matrix.rows(); ++ j) {
                unsigned x = i;
                unsigned lineLength = 0;
                while (x < matrix.columns() && matrix(j, x) == d) {
                    lineLength ++;
                    x ++;
                }
                
                if (lineLength >= 3) {
                    linesFound ++;
                    i = x;
                }
            }
        }
        
        // Find vertical lines
        for (unsigned i = 0; i < matrix.columns(); ++ i) {
            for (unsigned j = 0; j < matrix.rows(); ++ j) {
                unsigned y = j;
                unsigned lineLength = 0;
                while (y < matrix.rows() && matrix(y, i) == d) {
                    lineLength ++;
                    y ++;
                }
                
                if (lineLength >= 3) {
                    linesFound ++;
                    j = y;
                }
            }
        }
    }
    
    return linesFound;
}

bool Board::swap(unsigned y1, unsigned x1, unsigned y2, unsigned x2)
{
    assert(m_diamondMatrix.isValidCoordinates(y1, x1));
    assert(m_diamondMatrix.isValidCoordinates(y2, x2));
    if (y1 == y2)
        if (1 != abs(x1 - x2)) return false;
    else if (x1 == x2)
        if (1 != abs(y1 - y2)) return false;
    else
        return false;
    
    unsigned d1 = m_diamondMatrix(y1, x1);
    unsigned d2 = m_diamondMatrix(y2, x2);
    if (d1 == d2) 
        return false;
    else {
        m_diamondMatrix(y1, x1) = d2;
        m_diamondMatrix(y2, x2) = d1;
    }
        
    return true;
}

