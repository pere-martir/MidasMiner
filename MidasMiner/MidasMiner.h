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


class GLUTEventHandler
{
public:
    GLUTEventHandler() : m_renderer(NULL), m_windowWidth(0), m_windowHeight(0) {}
    ~GLUTEventHandler() { delete m_renderer; }
    
    void init(int argc, char** argv);
    void handleKeyboard(unsigned char key, int x, int y);
    void handleMouse(int button, int state, int x, int y);
    void handleDisplay();
    void handleReshape(int width, int height) {
        m_windowWidth = width;
        m_windowHeight = height;
        glViewport(0, 0, width, height);
        glutPostRedisplay();
    }
    void handleIdle() {}
    
private:
    unsigned m_windowWidth, m_windowHeight;
    Board m_board;
    // Delay its creation until the first draw so that we don't have to 
    // load the textures when running the unit tests.
    BoardRenderer *m_renderer; 
    StandardLibaryRandomNumberGenerator m_randNumGenerator;
    
    std::vector<DiamondCoords> m_pickedDiamonds;
};


#endif
