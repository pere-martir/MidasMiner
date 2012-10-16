#include <sstream>
#include "Board.h"
#include "RandomNumberGenerator.h"


std::string Matrix::string() const
{
    std::stringstream ss;
    for (unsigned int j = 0; j < rows(); ++ j) {
        for (unsigned int i = 0; i < columns(); ++ i) {
            ss << operator() (j, i) << " ";
        }
        ss << std::endl;
    }
    return ss.str();
}

Board::Board()
{}

bool Board::isLineCreatedByAddingDiamond(unsigned y, unsigned x, unsigned diamond)
{
    Matrix matrix = m_diamondMatrix;
    assert(matrix(y, x) == 0);
    matrix(y, x) = diamond;
    return 0 != findLines(matrix, NULL);
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
    
    // Some lines may be formed. Don't care. Let them be the bonus (free) lines
    // for the player.
    m_futureMatrix = Matrix(size, 100);
    for (unsigned int i = 0; i < m_futureMatrix.columns(); ++ i) {
        for (unsigned int j = 0; j < m_futureMatrix.rows(); ++ j) {
            m_futureMatrix(j, i) = 1 + rand.next() % diamondTypes;
        }
    }
}

unsigned Board::findLines(const Matrix& matrix, Lines* result) const
{
    if (result) result->clear();
    
    unsigned diamondTypes = getMaxElement(matrix);
    unsigned linesFound = 0;
    for (unsigned d = 1; d <= diamondTypes; ++ d) {
        
        // Find horizonal lines
        for (unsigned j = 0; j < matrix.rows(); ++ j) {
            for (unsigned i = 0; i < matrix.columns(); ++ i) {
                Line line;
                unsigned x = i;
                unsigned lineLength = 0;
                while (x < matrix.columns() && matrix(j, x) == d) {
                    line.push_back(DiamondCoord(j, x));
                    lineLength ++;
                    x ++;
                }
                
                if (line.size() >= 3) {
                    linesFound ++;
                    i = x;
                    if (result) result->push_back(line);
                }
            }
        }
        
        // Find vertical lines
        for (unsigned i = 0; i < matrix.columns(); ++ i) {
            for (unsigned j = 0; j < matrix.rows(); ++ j) {
                unsigned y = j;
                unsigned lineLength = 0;
                Line line;
                while (y < matrix.rows() && matrix(y, i) == d) {
                    line.push_back(DiamondCoord(y, i));
                    lineLength ++;
                    y ++;
                }
                
                if (lineLength >= 3) {
                    linesFound ++;
                    j = y;
                    if (result) result->push_back(line);
                }
            }
        }
    }
    
    return linesFound;
}

bool Board::removeLines()
{
    Lines lines;
    if (findLines(m_diamondMatrix, &lines) > 0) {
        Lines::const_iterator lineIt = lines.begin();
        for (; lines.end() != lineIt; ++ lineIt) {
            const Line& line = *lineIt;
            Line::const_iterator diamondCoord = line.begin();
            for (; line.end() != diamondCoord; diamondCoord ++) {
                m_diamondMatrix(diamondCoord->row, diamondCoord->col) = HOLE;
            }
        }
        return true;
    } else {
        return false;
    }
}

bool Board::swap(unsigned y1, unsigned x1, unsigned y2, unsigned x2)
{
    assert(m_diamondMatrix.isValidCoordinates(y1, x1));
    assert(m_diamondMatrix.isValidCoordinates(y2, x2));
    if (y1 == y2) {
        if (1 != abs(x1 - x2)) return false;
    } else if (x1 == x2) {
        if (1 != abs(y1 - y2)) return false;
    } else {
        return false;
    }
    
    unsigned d1 = m_diamondMatrix(y1, x1);
    unsigned d2 = m_diamondMatrix(y2, x2);
    if (d1 == d2) 
        return false;
    else {
        Matrix tmpMatrix = m_diamondMatrix;
        tmpMatrix(y1, x1) = d2;
        tmpMatrix(y2, x2) = d1;

        if (findLines(tmpMatrix) == 0)
            return false;
        else
        {
            m_diamondMatrix = tmpMatrix;
            return true;
        }
    }
}


/* It's not the simplest way to implement the collapse of the board but this design
   makes the implementation of animation easier.
*/
void Board::collapse()
{
    unsigned iteration = 0;
    while (true)
    {
        if (!removeLines()) {
            // The unit tests may setup a board with holes
            if (!hasHole())
                break;
        }
        
        printf("\n--\nbefore collapse:\n%s\n%s", 
               m_futureMatrix.string().c_str(),
               m_diamondMatrix.string().c_str());
        
        // This arrary is indexed by the column, each element is the row index of
        // the last known empty diamond. The element -1 means this column has no empty
        // diamond.
        std::vector<int> rowsOfLastKnownEmptyDiamond;
        rowsOfLastKnownEmptyDiamond.resize(m_diamondMatrix.columns(), m_diamondMatrix.rows() - 1);
        
        bool finished = false;
        while (!finished) {
            for (unsigned i = 0; i < m_diamondMatrix.columns(); ++ i) {  
                
                if (rowsOfLastKnownEmptyDiamond[i] >= 0) {
                    bool emptyDiamondFound = false;
                    for (int j = rowsOfLastKnownEmptyDiamond[i]; j >= 0; -- j) {
                        if (m_diamondMatrix(j, i) == HOLE) {
                            rowsOfLastKnownEmptyDiamond[i] = j;
                            emptyDiamondFound = true;
                            moveColumnDownward(j, i);
                            printf("\n--\n#%d collapse (%d,%d):\n%s\n%s", iteration, j, i, 
                                   m_futureMatrix.string().c_str(),
                                   m_diamondMatrix.string().c_str());
                            
                            // There may be more holes above but we will handle them in the next iteration 
                            // so that all diamonds drop at the same speed.

                            break; 
                        }
                    }
                    
                    if (!emptyDiamondFound) rowsOfLastKnownEmptyDiamond[i] = -1;
                }
            }
            
            finished = true;
            for (unsigned c = 0; c < rowsOfLastKnownEmptyDiamond.size(); ++ c) {
                if (rowsOfLastKnownEmptyDiamond[c] != -1) {
                    finished = false;
                    break;
                }
            }
            
            iteration ++;
        }
    }
}

void Board::moveColumnDownward(unsigned row, unsigned col)
{
    assert(m_diamondMatrix.columns() == m_futureMatrix.columns());
    assert(m_diamondMatrix(row, col) == HOLE);
    for (int r = row; r > 0; --r) {
        m_diamondMatrix(r, col) = m_diamondMatrix(r - 1, col);
    }
    m_diamondMatrix(0, col) = m_futureMatrix(m_futureMatrix.rows() - 1, col);
    rotateColumnDownward(m_futureMatrix, col);
}

void Board::rotateColumnDownward(Matrix& matrix, unsigned col)
{
    unsigned lastRow = matrix.rows() - 1;
    unsigned lastRowElement = matrix(lastRow, col);
    for (int j = lastRow; j > 0; -- j) {
        matrix(j, col) = matrix(j - 1, col);
    }
    matrix(0, col) = lastRowElement;
}