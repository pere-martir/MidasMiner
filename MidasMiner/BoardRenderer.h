//
//  BoardRenderer.h
//  MidasMiner
//
//  Created by Tzu-Chien Chiu on 10/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MidasMiner_BoardRenderer_h
#define MidasMiner_BoardRenderer_h

#include <assert.h>
#include <vector>
#include <GLUT/glut.h>

#include "Board.h"

class BoardRenderer
{
public:
    BoardRenderer();
    void draw(const Board& board);
    
private:
    std::vector<GLuint> m_diamondTextures;
    void loadTexturesFromFiles();
    bool initTextureFromRawImage(char *image, int width, int height, GLuint texName);
};

#endif
