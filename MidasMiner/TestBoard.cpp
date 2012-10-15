#include "UnitTest++.h"
#include "Board.h"


SUITE(FindLines)
{
    
TEST(ZeroLines)
{
    Matrix m;
    m.initWithElements(4,  
                       0, 1, 0, 1,
                       1, 0, 1, 0,
                       0, 1, 0, 1,
                       1, 0, 1, 0);
    Board board(m);
    CHECK_EQUAL(0, board.findLines());
}

TEST(TwoDiamondsAreNotLine)
{    
    Matrix m;
    m.initWithElements(4, 
                       1, 0, 2, 2,
                       0, 1, 0, 1,
                       1, 0, 1, 0,
                       0, 1, 0, 1);
    Board board(m);
    CHECK_EQUAL(0, board.findLines());
}

TEST(OneHorizontalLineOfThreeDiamonds)
{    
    Matrix m;
    m.initWithElements(4,  
                       0, 2, 2, 2,
                       1, 0, 1, 0,
                       0, 1, 0, 1,
                       1, 0, 1, 0);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(OneHorizontalLinesOfSixDiamonds)
{    
    Matrix m;
    m.initWithElements(6,  
                       0, 1, 0, 1, 0, 1,
                       2, 2, 2, 2, 2, 2,
                       0, 1, 0, 1, 0, 1,
                       1, 0, 1, 0, 1, 0,
                       0, 1, 0, 1, 0, 1,
                       1, 0, 1, 0, 1, 0);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(TwoHorizontalLines)
{    
    Matrix m;
    m.initWithElements(4,  
                       0, 1, 0, 1,
                       1, 2, 2, 2,
                       2, 2, 2, 2,
                       1, 0, 1, 0);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(OneVerticalLine)
{    
    Matrix m;
    m.initWithElements(4,  
                       0, 1, 0, 1,
                       1, 0, 1, 1,
                       0, 1, 0, 1,
                       1, 0, 1, 0);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(VerticalAndHorizontal)
{    
    Matrix m;
    m.initWithElements(4,  
                       2, 1, 0, 1,
                       2, 0, 1, 0,
                       2, 1, 0, 1,
                       0, 2, 2, 2);
    Board board(m);
    CHECK_EQUAL(2, board.findLines());
}

TEST(VerticalCrossHorizontal)
{    
    Matrix m;
    m.initWithElements(4,  
                       0, 2, 0, 1,
                       1, 2, 1, 0,
                       2, 2, 2, 1,
                       1, 2, 1, 0);
    Board board(m);
    CHECK_EQUAL(2, board.findLines());
}
    
}

SUITE(Swap)
{

TEST(TwoHorizontallyAdjacent)
{
    Matrix m;
    m.initWithElements(4,  
                       0, 1, 0, 1,
                       1, 0, 1, 0,
                       0, 1, 0, 1,
                       1, 0, 1, 0);
    Board board(m);
    CHECK(board.swap(0, 0, 0, 1)); // The first two elements in the first row
    
    Matrix expectedMatrix;
    expectedMatrix.initWithElements(4,  
                       1, 0, 0, 1,
                       1, 0, 1, 0,
                       0, 1, 0, 1,
                       1, 0, 1, 0);
    CHECK(board.matrix() == expectedMatrix);
}
    
TEST(TwoVerticallyAdjacent)
{
    Matrix m;
    m.initWithElements(4,  
                       0, 1, 0, 1,
                       1, 0, 1, 0,
                       0, 1, 0, 1,
                       1, 0, 1, 0);
    Board board(m);
    CHECK(board.swap(0, 0, 1, 0)); // The leading elments of the first two rows.
    
    Matrix expectedMatrix;
    expectedMatrix.initWithElements(4,  
                                    1, 1, 0, 1,
                                    0, 0, 1, 0,
                                    0, 1, 0, 1,
                                    1, 0, 1, 0);
    CHECK(board.matrix() == expectedMatrix);
}
    
TEST(CannotSwap)
{
    Matrix m;
    m.initWithElements(4,  
                       0, 1, 0, 1,
                       1, 0, 1, 0,
                       0, 1, 0, 1,
                       1, 0, 1, 0);
    Board board(m);
    CHECK(!board.swap(0, 0, 0, 2)); 
    CHECK(!board.swap(0, 0, 2, 0)); 
    CHECK(!board.swap(0, 0, 3, 3)); 
    CHECK(board.matrix() == m);
}
    
}
