#include "UnitTest++.h"
#include "Board.h"
#include "RandomNumberGenerator.h"

#if 1
SUITE(Initialization)
{
    
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
}
    
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
    CHECK_EQUAL(2, board.findLines());
}
    
TEST(TwoVerticalLines)
{    
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 3, 2, 1,
                       1, 3, 1, 3,
                       3, 3, 2, 3,
                       1, 3, 1, 3);
    Board board(m);
    CHECK_EQUAL(2, board.findLines());
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

TEST(SwappingMustCreateLines)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    // Swap the first two elements in the first row
    CHECK(!board.swap(DiamondCoords(0, 0), DiamondCoords(0, 1))); 
    CHECK(board.matrix() == m);
}
    
TEST(CannotSwapDiamondsOfSameColor)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 2, 1, 1,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    // Swap the first two elements in the first row
    CHECK(!board.swap(DiamondCoords(0, 0), DiamondCoords(0, 1)));
    CHECK(board.matrix() == m);
}
    
TEST(SwapTwoVerticallyAdjacentDiamonds)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 1, 2,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    CHECK(board.swap(0, 0, 1, 0)); // The leading elments of the first two rows.
    
    Matrix expected;
    expected.initWithElements(4, 4, 
                       1, 1, 1, 2,
                       2, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    CHECK(board.matrix() == expected);
}
    
TEST(CannotSwapNotAdjacentDiamonds)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 1, 3,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    // Swapping (0, 0) and (2, 1) creates three lines but it's not allowed
    // because they are not adjacent.
    CHECK(!board.swap(0, 0, 2, 1)); 
    CHECK(board.matrix() == m);
}
}


SUITE(Collapse)
{

TEST(FindLinesBeforeCollapse)
{
    Matrix m;
    m.initWithElements(4, 4,
                       3, 3, 3, 3,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    
    Matrix future;
    future.initWithElements(1, 4,  
                       4, 5, 6, 7);
    board.setFutureMatrix(future);
    
    Matrix expected;
    expected.initWithElements(4, 4,  
                       4, 5, 6, 7,
                       1, 2, 1, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    board.collapse();
    CHECK(board.matrix() == expected);
}
    
TEST(HolesInEachColumn)
{
    Matrix m;
    m.initWithElements(4, 4,
                       2, 1, 2, 1,
                       0, 0, 0, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    Board board(m);
    
    Matrix future;
    future.initWithElements(3, 4,  
                       3, 4, 5, 6,
                       8, 9, 1, 2,
                       4, 5, 6, 7);
    
    board.setFutureMatrix(future);
    
    Matrix expected;
    expected.initWithElements(4, 4,  
                       4, 5, 6, 1,
                       2, 1, 2, 2,
                       2, 1, 2, 1,
                       1, 2, 1, 2);
    board.collapse();
    CHECK(board.matrix() == expected);
}
    
TEST(TwoSeparatedEmptyDiamondsInSameColumn)
{
    Matrix m;
    m.initWithElements(4, 4, 
                       1, 0, 0, 0, 
                       2, 1, 2, 1, 
                       1, 2, 1, 2,  
                       0, 0, 0, 1);
    Board board(m);
    
    Matrix future;
    future.initWithElements(2, 4,  
                       7, 8, 9, 3,
                       3, 4, 5, 6);
    board.setFutureMatrix(future);
    
    Matrix expected;
    expected.initWithElements(4, 4,
                       3, 8, 9, 6, 
                       1, 4, 5, 1, 
                       2, 1, 2, 2, 
                       1, 2, 1, 1);
    board.collapse(); 
    CHECK(board.matrix() == expected);
}
    
TEST(LinesCreatedByNewDiamondsAreRemoved)
{
    Matrix m;
    m.initWithElements(4, 4,
                       1, 0, 1, 2,
                       2, 0, 2, 1,
                       1, 0, 1, 2,
                       2, 1, 2, 1);
    Board board(m);
    Matrix future;
    future.initWithElements(5, 4, 
                       6, 7, 6, 7,
                       7, 6, 7, 6,
                       4, 1, 4, 5,
                       5, 4, 5, 4,
                       4, 1, 4, 5);
    board.setFutureMatrix(future);

    Matrix expected;
    expected.initWithElements(4, 4,
                       5, 7, 5, 2,
                       4, 6, 4, 1,
                       2, 4, 2, 2,
                       2, 1, 2, 1);
    board.collapse();
    
    // When all holes are filled:
    //                 1, 1, 1, 2,
    //                 2, 4, 2, 1,
    //                 1, 1, 1, 2,
    //                 2, 1, 2, 1
    //
    // the lines of 1 in the first and the third row should be removed:
    //
    //                 0, 0, 0, 2,
    //                 2, 4, 2, 1,
    //                 0, 0, 0, 2,
    //                 2, 1, 2, 1
    //
    // and then keep collasping the board.
    //
    CHECK(board.matrix() == expected);    
}
    
}
#endif

SUITE(GamePlay)
{
TEST(Foo)
{
    
}
}
