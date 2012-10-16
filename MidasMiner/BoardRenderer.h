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
#include "BoardDelegate.h"

class BoardRenderer : public BoardDelegate
{
public:
    BoardRenderer(Board& board);
    void draw(unsigned windowWidth, unsigned windowHeight);
    
    bool pickDiamond(unsigned x, unsigned y, DiamondCoords& coord);
    void cancelPickedDiamond() { m_hasPickedDiamond = false; }
    
private:

    void setupProjectAndModelViewMatrix(unsigned windowWidth, unsigned windowHeight);
    std::vector<GLuint> m_diamondTextures;
    void loadTexturesFromFiles();
    bool initTextureFromRawImage(char *image, int width, int height, GLuint texName);
    
    Board& m_board;
    bool m_hasPickedDiamond;
    DiamondCoords m_pickedDiamond;

    //
    // BoardDelegate methods
    //
public:
    virtual void onDiamondsMoved(const Board* sender, 
                                 const Coords& toCoordsArray, 
                                 const Coords& fromCoordsArray);
    
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
