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

struct Pixel
{
    unsigned x, y;
    Pixel(unsigned _x = 0, unsigned _y = 0) : x(_x), y(_y) {}
};

struct Sprite
{
    DiamondCoords diamond;
    Pixel pos;
};

typedef std::vector<Pixel> PixelsArray;
typedef std::vector<Sprite> SpritesArray;

class BoardRenderer : public BoardDelegate
{
public:
    static BoardRenderer* getSingleton() 
    { 
        assert(s_singleton);
        return s_singleton; 
    }
    
    BoardRenderer(Board& board);
    void draw(unsigned windowWidth, unsigned windowHeight);
    
    bool pickDiamond(unsigned x, unsigned y, DiamondCoords& coord);
    
private:
    static const unsigned int DIAMOND_SIZE = 40; // FIXME
    static BoardRenderer* s_singleton;
    
    void setupProjectAndModelViewMatrix(unsigned windowWidth, unsigned windowHeight);
    std::vector<GLuint> m_diamondTextures;
    void loadTexturesFromFiles();
    bool initTextureFromRawImage(char *image, int width, int height, GLuint texName);
    
    // Return the upper-left corner
    Pixel getDiamondPosition(const DiamondCoords& diamond) const;
    
    Board& m_board;
    bool m_hasPickedDiamond;
    DiamondCoords m_pickedDiamond;
    
    bool m_animationFinished;
    SpritesArray m_sprites;
    bool doesSprtesContainDiamond(const DiamondCoords& d) const;
    
    static void glutTimerHandler(int value) 
    {
        BoardRenderer::getSingleton()->onTimer();
    }
    
    void onTimer() {}

//
// BoardDelegate methods
//
public:
    virtual void onDiamondsSwapped(Board* sender, 
                                   const DiamondCoords& d1, const DiamondCoords& d2);
    
    virtual void onPreviousSwapCancelled(Board* sender,
                                         const DiamondCoords& d1, const DiamondCoords& d2);
    
    virtual void onDiamondsDisappeared(Board* sender) {}
    
    virtual void onDiamondsMoved(Board* sender, 
                                 const CoordsArray& toCoordsArray, 
                                 const CoordsArray& fromCoordsArray);
    
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
