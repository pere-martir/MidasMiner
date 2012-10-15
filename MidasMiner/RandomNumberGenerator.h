//
//  RandomNumberGenerator.h
//  MidasMiner
//
//  Created by Tzu-Chien Chiu on 10/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MidasMiner_RandomNumberGenerator_h
#define MidasMiner_RandomNumberGenerator_h

class RandomNumberGenerator 
{
public:
    // Return an integer in [0, 2^31 - 1]
    virtual unsigned next() = 0;
};


class StandardLibaryRandomNumberGenerator : public RandomNumberGenerator
{
public:
    StandardLibaryRandomNumberGenerator() {
        srand(time(NULL));
    }
        
    virtual unsigned next() {
        return rand();
    }
};

// 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4
class ShortRunRandomNumberGenerator : public RandomNumberGenerator
{
private:
    unsigned m_nextNumber;
    unsigned m_totalNumbersGenerated;
    
public:
    ShortRunRandomNumberGenerator() : m_nextNumber(0), m_totalNumbersGenerated(0) {}
    
    virtual unsigned next() {
        if (m_totalNumbersGenerated % 3 == 0)
            m_nextNumber ++;
        m_totalNumbersGenerated ++;
        return m_nextNumber;
    }
};


#endif
