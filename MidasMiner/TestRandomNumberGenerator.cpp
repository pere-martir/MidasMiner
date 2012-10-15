#include "UnitTest++.h"
#include "RandomNumberGenerator.h"

SUITE(RandomNumberGenerator)
{
    TEST(ShortRunRandomNumberGenerator)
    {
        ShortRunRandomNumberGenerator rand;
        CHECK_EQUAL(1, rand.next());
        CHECK_EQUAL(1, rand.next());
        CHECK_EQUAL(1, rand.next());
        CHECK_EQUAL(2, rand.next());
        CHECK_EQUAL(2, rand.next());
        CHECK_EQUAL(2, rand.next());  
        CHECK_EQUAL(3, rand.next());
        CHECK_EQUAL(3, rand.next());
        CHECK_EQUAL(3, rand.next());  
    }
}
