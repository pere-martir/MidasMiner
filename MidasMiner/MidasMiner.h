//
//  MidasMiner.h
//  MidasMiner
//
//  Created by Tzu-Chien Chiu on 10/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MidasMiner_MidasMiner_h
#define MidasMiner_MidasMiner_h


#include "Board.h"
#include "BoardRenderer.h"
#include "RandomNumberGenerator.h"

class MidasMiner
{
public:
    MidasMiner() : m_renderer(NULL) {}
    void init(int argc, char** argv);
    void glutKeyboardHandler(unsigned char key, int x, int y);
    void glutMouseHanlder(int button, int state, int x, int y);
    void glutDisplayHandler();

    
private:
    static const unsigned WINDOW_WIDTH = 40 * 8;
    static const unsigned WINDOW_HEIGHT = 40 * 8;
    Board m_board;
    // Delay its creation until the first draw so that we don't have to 
    // load the textures when running the unit tests.
    BoardRenderer *m_renderer; 
    StandardLibaryRandomNumberGenerator m_randNumGenerator;
};


#endif
