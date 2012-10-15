#include "UnitTest++.h"
#include "Board.h"

TEST(FindLinesNone)
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

TEST(FindLines_TwoDiamondsAreNotLine)
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

TEST(FindLines_OneHorizontalOfThreeDiamonds)
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

TEST(FindLines_OneHorizontalOfSixDiamonds)
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

TEST(FindLines_TwoHorizontals)
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

TEST(FindLines_OneVertical)
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

TEST(FindLines_VerticalAndHorizontal)
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
