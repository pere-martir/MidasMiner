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
    void draw(unsigned windowWidth, unsigned windowHeight, const Board& board);
    bool pickDiamond(unsigned x, unsigned y, unsigned& picked_row, unsigned& picked_col) const;
    
private:
    void setupProjectAndModelViewMatrix(unsigned windowWidth, unsigned windowHeight);
    std::vector<GLuint> m_diamondTextures;
    void loadTexturesFromFiles();
    bool initTextureFromRawImage(char *image, int width, int height, GLuint texName);
    
#if USE_PICKING_BY_COLOR_ID
public:
    void drawInPickingMode(const Board& board);
    
private:
    GLuint fbo;
    GLuint pickingTexture;
    void initPickingByColorID();
#endif
};

#endif
