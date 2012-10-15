
#include "Board.h"

Board::Board()
{}


void Board::initRandomly(unsigned size, unsigned diamondTypes)
{
    srand(time(NULL));
    m_diamondMatrix = Matrix(size, size);
    for (unsigned int i = 0; i < m_diamondMatrix.columns(); ++ i) {
        for (unsigned int j = 0; j < m_diamondMatrix.rows(); ++ j) {
            m_diamondMatrix(j, i) = rand() % diamondTypes;
        }
    }
}

unsigned Board::findLines() const
{
    unsigned linesFound = 0;
    for (unsigned d = 0; d < m_diamondTypes; ++ d) {
        //unsigned diamond = m_diamondTextures[t];
        
        // Find horizonal lines
        for (unsigned i = 0; i < m_diamondMatrix.columns(); ++ i) {
            for (unsigned j = 0; j < m_diamondMatrix.rows(); ++ j) {
                unsigned x = i;
                unsigned lineLength = 0;
                while (x < m_diamondMatrix.columns() && m_diamondMatrix(j, x) == d) {
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
        for (unsigned i = 0; i < m_diamondMatrix.columns(); ++ i) {
            for (unsigned j = 0; j < m_diamondMatrix.rows(); ++ j) {
                unsigned y = j;
                unsigned lineLength = 0;
                while (y < m_diamondMatrix.rows() && m_diamondMatrix(y, i) == d) {
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

