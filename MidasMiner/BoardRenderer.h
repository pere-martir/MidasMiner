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

struct Vector2D
{
    int x, y;
    Vector2D(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    
    bool operator == (const Vector2D& other) {
        return x == other.x && y == other.y;
    }
    
    bool operator != (const Vector2D& other) {
        return !(*this == other);
    }
    
    Vector2D& operator += (const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    // No need to be defined as 'friend' since we don't only
    // operate with Vector2D.
    const Vector2D operator - (const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    const Vector2D operator / (int divisor) const {
        assert(divisor);
        return Vector2D(x / divisor,  y / divisor);
    }
};

struct Sprite
{
    DiamondCoords diamond; // the final coordinates
    Vector2D pos; // the current position
    Vector2D velocity; // velocity is added periodically to pos until pos reaches finalPos
    Vector2D finalPos;
};

typedef std::vector<Sprite> SpritesArray;

class BoardRenderer : public BoardDelegate
{
public:
    static const unsigned int DIAMOND_SIZE = 40; // FIXME
    
    static BoardRenderer* getSingleton() 
    { 
        assert(s_singleton);
        return s_singleton; 
    }
    
    BoardRenderer(Board& board);
    
    void draw(unsigned windowWidth, unsigned windowHeight);
    bool pickDiamond(unsigned x, unsigned y, DiamondCoords& coord);
    unsigned backgroundWidth() const { return m_backgroundWidth; }
    unsigned backgroundHeight() const { return m_backgroundHeight; }
    
private:
    static BoardRenderer* s_singleton;
    
    unsigned m_backgroundWidth, m_backgroundHeight;
    GLuint m_backgroundTexture;
    std::vector<GLuint> m_diamondTextures;
    void loadTextures();
    bool initTextureFromRawImage(char *image, int width, int height, GLuint texName);
    
    void setupProjectAndModelViewMatrix(unsigned windowWidth, unsigned windowHeight);
    void drawBackground();
    void drawDiamonds();
    void drawPickedSquare();
    
    Vector2D m_boardPos;     // The offset of board in BackGround.png
    Vector2D getDiamondCurrentPosition(const DiamondCoords& diamond) const;
    Vector2D mapDiamondCoordinatesToPosition(const DiamondCoords& diamond) const;
    
    Board& m_board;
    bool m_hasPickedDiamond;
    DiamondCoords m_pickedDiamond;
    
    Board::Animaton m_currentAnimation;
    SpritesArray m_sprites;
    
    static void glutTimerHandler(int value) 
    {
        BoardRenderer::getSingleton()->onTimer();
    }
    
    void setTimer(unsigned milliseconds = 0);
    void onTimer();
    
    void setupSwapAnimation(const DiamondCoords& d1, const DiamondCoords& d2);

//
// BoardDelegate methods
//
public:
    virtual void onDiamondsSwapped(Board* sender, 
                                   const DiamondCoords& d1, const DiamondCoords& d2);
    
    virtual void onPreviousSwapCancelled(Board* sender,
                                         const DiamondCoords& d1, const DiamondCoords& d2);
    
    virtual void onDiamondsRemoved(Board* sender);
    
    virtual void onDiamondsFallen(Board* sender, 
                                 const CoordsArray& fromCoordsArray, 
                                 const CoordsArray& toCoordsArray);
    
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
