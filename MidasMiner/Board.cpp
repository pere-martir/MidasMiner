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
    m_futureMatrix = Matrix(3, size);
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
                CoordsArray coordsArray;
                unsigned x = i;
                unsigned lineLength = 0;
                while (x < matrix.columns() && matrix(j, x) == d) {
                    coordsArray.push_back(DiamondCoords(j, x));
                    lineLength ++;
                    x ++;
                }
                
                if (coordsArray.size() >= 3) {
                    linesFound ++;
                    i = x;
                    if (result) result->push_back(coordsArray);
                }
            }
        }
        
        // Find vertical lines
        for (unsigned i = 0; i < matrix.columns(); ++ i) {
            for (unsigned j = 0; j < matrix.rows(); ++ j) {
                unsigned y = j;
                unsigned lineLength = 0;
                CoordsArray coordsArray;
                while (y < matrix.rows() && matrix(y, i) == d) {
                    coordsArray.push_back(DiamondCoords(y, i));
                    lineLength ++;
                    y ++;
                }
                
                if (coordsArray.size() >= 3) {
                    linesFound ++;
                    j = y;
                    if (result) result->push_back(coordsArray);
                }
            }
        }
    }
    
    return linesFound;
}


void Board::onAnimationFinished(Animaton ani)
{
    if (ANIMATION_SWAPPING == ani) 
        onAnimationSwappingFinished();
    else if (ANIMATION_SWAPPING_BACK == ani) {
        // intentionally left blank
    } else if (ANIMATION_REMOVING == ani)
        onAnimationRemovingFnished();
    else if (ANIMATION_FALLING == ani)
        onAnimationFallingFinished();
    else
        assert(false);
}


void Board::swap(const DiamondCoords& d1, const DiamondCoords& d2)
{
    assert(m_diamondMatrix.inside(d1.row, d1.col));
    assert(m_diamondMatrix.inside(d2.row, d2.col));
    if (!d1.isAdjacentTo(d2)) return;

    unsigned diamondValue1 = m_diamondMatrix(d1.row, d1.col);
    unsigned diamondValue2 = m_diamondMatrix(d2.row, d2.col);
    if (diamondValue1 == diamondValue2) return;
    
    m_diamondMatrix(d1.row, d1.col) = diamondValue2;
    m_diamondMatrix(d2.row, d2.col) = diamondValue1;
    m_lastSwappedDiamonds[0] = d1;
    m_lastSwappedDiamonds[1] = d2;
    
    if (m_delegate) {
        m_delegate->onDiamondsSwapped(this, d1, d2);
        // continues in onAnimationSwappingFinished()
    }
}

void Board::onAnimationSwappingFinished() 
{
    if (findLines(m_diamondMatrix) == 0) {
        const DiamondCoords& d1 = m_lastSwappedDiamonds[0];
        const DiamondCoords& d2 = m_lastSwappedDiamonds[1];
        
        unsigned diamondValue1 = m_diamondMatrix(d1.row, d1.col);
        unsigned diamondValue2 = m_diamondMatrix(d2.row, d2.col);
        
        m_diamondMatrix(d1.row, d1.col) = diamondValue2;
        m_diamondMatrix(d2.row, d2.col) = diamondValue1;
        
        if (m_delegate)
            m_delegate->onPreviousSwapCancelled(this, d1, d2);
        
    } else {
        if (m_futureMatrix.rows() > 0) { // it's empty only in unit tests
            collapse();
        }
    }
}


/* It's not the simplest way to implement the collapse of the board but this design
   makes the implementation of animation easier.
*/
void Board::collapse(bool firstIteration)
{
    bool holesFound = false;
    
    // Find lines and replace them with holes
    Lines lines;
    CoordsArray removedDiamonds;
    if (findLines(m_diamondMatrix, &lines) > 0) {
        Lines::const_iterator lineIt = lines.begin();
        for (; lines.end() != lineIt; ++ lineIt) {
            const CoordsArray& line = *lineIt;
            CoordsArray::const_iterator coords = line.begin();
            for (; line.end() != coords; coords ++) {
                removedDiamonds.push_back(*coords);
                m_diamondMatrix(coords->row, coords->col) = HOLE;
                holesFound = true;
            }
        }
    } else if (hasHole()) { // The unit tests may setup a board with holes
        holesFound = true;
    }
    
    if (!holesFound) return;
    
    if (firstIteration) {
        m_iteration = 0;
        m_rowsOfLastKnownEmptyDiamond.clear();
        m_rowsOfLastKnownEmptyDiamond.resize(m_diamondMatrix.columns(), m_diamondMatrix.rows() - 1);
    
        printf("\n--\nbefore collapse:\n%s\n%s", 
               m_futureMatrix.string().c_str(),
               m_diamondMatrix.string().c_str());
    }
    
    if (m_delegate) {
        m_delegate->onDiamondsRemoved(this, removedDiamonds);
        // continue in onAnimationRemovingFnished
    }
}

void Board::onAnimationRemovingFnished()
{
    moveBoardDownwardOneStep();
}

CoordsArray Board::generateCoordinatesOfVerticalLine(const DiamondCoords& top, const DiamondCoords& bottom)
{
    assert(top.col == bottom.col);
    assert(top.row <= bottom.row);
    CoordsArray array;
    for (int j = top.row; j <= bottom.row; ++ j)
        array.push_back(DiamondCoords(j, top.col));
    return array;
}

void Board::moveBoardDownwardOneStep()
{
    CoordsArray fromCoordsArray, toCoordsArray;
    for (unsigned i = 0; i < m_diamondMatrix.columns(); ++ i) {  
        
        if (m_rowsOfLastKnownEmptyDiamond[i] >= 0) {
            
            bool holeFound = false;
            for (int j = m_rowsOfLastKnownEmptyDiamond[i]; j >= 0; -- j) {
                if (m_diamondMatrix(j, i) == HOLE) {
                    holeFound = true;
                    m_rowsOfLastKnownEmptyDiamond[i] = j;
                    moveColumnDownward(j, i);
            
                    CoordsArray currPos = generateCoordinatesOfVerticalLine(DiamondCoords(0, i),
                                                                            DiamondCoords(j, i));
                    CoordsArray prevPos;
                    CoordsArray::const_iterator c = currPos.begin();
                    for (; currPos.end() != c; ++c)
                        prevPos.push_back(DiamondCoords(c->row - 1, c->col));
                    
                    toCoordsArray.insert(toCoordsArray.end(), currPos.begin(), currPos.end());
                    fromCoordsArray.insert(fromCoordsArray.end(), prevPos.begin(), prevPos.end());
                    
                    // There may be more holes above but we will handle them in the next iteration 
                    // so that all diamonds drop at the same speed.
                    break; 
                }
            }
            
            if (!holeFound) m_rowsOfLastKnownEmptyDiamond[i] = -1;
        }
    }
    
    printf("\n--\n#%d collapsed: \n%s\n%s", m_iteration, 
           m_futureMatrix.string().c_str(),
           m_diamondMatrix.string().c_str());
    
    if (m_delegate) {
        m_delegate->onDiamondsFallen(this, fromCoordsArray, toCoordsArray); 
        // continue in onAnimationFallingFinished() 
    }
}

void Board::onAnimationFallingFinished() 
{
    if (hasHole()) {
        m_iteration ++; 
        moveBoardDownwardOneStep();
    } else {
        collapse();
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