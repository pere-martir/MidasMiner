#include "UnitTest++.h"
#include "Board.h"
#include "RandomNumberGenerator.h"

SUITE(Initialization)
{
    
    /*
TEST(RandomInitializationDoesNotCreateLines)
{
    Board board;
    // We cannot test with "true" (or psuedo) random number because
    // we cannot know which result to be expected. So here we use
    // an easily predictable random number generator to facilitate the 
    // unit tests.
    ShortRunRandomNumberGenerator rand;
    board.initRandomly(8, 5, rand);
    CHECK_EQUAL(0, board.findLines());
}*/
    
}


// A line is formed by three or more adjacent horizontal or vertical elements.
// In this unit test suite, generally the lines sexpected to be found is denoted by 3.
SUITE(FindLines)
{
    
TEST(NoLine) 
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    CHECK_EQUAL(0, board.findLines());
}
    
TEST(ZerosNotConsideredLine)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       0, 0, 0, 0,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    CHECK_EQUAL(0, board.findLines());
}

TEST(TwoDiamondsAreNotLine)
{    
    Matrix m;
    m.initWithElements(4, 4,
                       1, 2, 3, 3,
                       2, 1, 2, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1);
    Board board(m);
    CHECK_EQUAL(0, board.findLines());
}

TEST(OneHorizontalLineOfThreeDiamonds)
{    
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 3, 3, 3,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(OneHorizontalLinesOfSixDiamonds)
{    
    Matrix m;
    m.initWithElements(6, 6, 
                       2, 1, 2, 1, 2, 1,
                       3, 3, 3, 3, 3, 3,
                       2, 1, 2, 1, 2, 1,
                       1, 2, 1, 2, 1, 2,
                       2, 1, 2, 1, 2, 1,
                       1, 2, 1, 2, 1, 2);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(TwoHorizontalLines)
{    
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 1,
                       1, 3, 3, 3,
                       3, 3, 3, 3,
                       1, 2, 1, 2);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(OneVerticalLine)
{    
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 3,
                       1, 2, 1, 3,
                       2, 1, 2, 3,
                       1, 2, 1, 2);
    Board board(m);
    CHECK_EQUAL(1, board.findLines());
}

TEST(VerticalAndHorizontal)
{    
    Matrix m;
    m.initWithElements(4, 4, 
                       3, 1, 2, 1,
                       3, 2, 1, 2,
                       3, 1, 2, 1,
                       2, 3, 3, 3);
    Board board(m);
    CHECK_EQUAL(2, board.findLines());
}

TEST(VerticalCrossHorizontal)
{    
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 3, 2, 1,
                       1, 3, 1, 2,
                       3, 3, 3, 1,
                       1, 3, 1, 2);
    Board board(m);
    CHECK_EQUAL(2, board.findLines());
}
    
}

SUITE(Swap)
{

TEST(TwoHorizontallyAdjacent)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    CHECK(board.swap(0, 0, 0, 1)); // The first two elements in the first row
    
    Matrix expectedMatrix;
    expectedMatrix.initWithElements(4, 4, 
                       1, 2, 2, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    CHECK(board.matrix() == expectedMatrix);
}
    
TEST(TwoVerticallyAdjacent)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    CHECK(board.swap(0, 0, 1, 0)); // The leading elments of the first two rows.
    
    Matrix expectedMatrix;
    expectedMatrix.initWithElements(4, 4, 
                                    1, 1, 2, 1,
                                    2, 2, 1, 2,
                                    2, 1, 2, 1,
                                    1, 2, 1, 2);
    CHECK(board.matrix() == expectedMatrix);
}
    
TEST(CannotSwapNotAdjancentElements)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    CHECK(!board.swap(0, 0, 0, 2)); 
    CHECK(!board.swap(0, 0, 2, 0)); 
    CHECK(!board.swap(0, 0, 3, 3)); 
    CHECK(board.matrix() == m);
}
    
TEST(Collapse)
{
    Matrix m;
    m.initWithElements(4, 4,
                       2, 3, 1, 2,
                       3, 1, 3, 1,
                       1, 3, 1, 2,
                       2, 3, 2, 1);
    
    Board board(m);
    CHECK(board.swap(0, 1, 1, 1)); 
    Lines linesFound;
    CHECK(2 == board.findLines(&linesFound));
    
    board.removeLines(linesFound);
    Matrix expectedMatrixAfterLinesRemoved;
    expectedMatrixAfterLinesRemoved.initWithElements(4, 4,
                       2, 1, 1, 2,
                       0, 0, 0, 1,
                       1, 0, 1, 2,
                       2, 0, 2, 1);
    CHECK(board.matrix() == expectedMatrixAfterLinesRemoved);
    CHECK(0 == board.findLines());
    

    Matrix futureMatrix;
    futureMatrix.initWithElements(3, 4,  
                       4, 5, 4, 5,
                       5, 4, 5, 4,
                       4, 5, 4, 5);
    
    board.setFutureMatrix(futureMatrix);
    board.collapse();
    
    Matrix expectedMatrixAfterCollapse;
    expectedMatrixAfterCollapse.initWithElements(4, 4,  
                       4, 5, 4, 2,
                       2, 4, 1, 1,
                       1, 5, 1, 2,
                       2, 1, 2, 1);
    CHECK(board.matrix() == expectedMatrixAfterCollapse);
}
   
}
