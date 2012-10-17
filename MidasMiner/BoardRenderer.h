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
    Vector2D finalPos;
    Vector2D velocity; // velocity is added periodically to pos until pos reaches finalPos
};

typedef std::vector<Sprite> SpritesArray;

class BoardRenderer : public BoardDelegate
{
public:
    static const unsigned int DIAMOND_SIZE = 40;
    
    static BoardRenderer* getSingleton() 
    { 
        assert(s_singleton);
        return s_singleton; 
    }
    
    BoardRenderer(Board& board);
    
    void draw(unsigned windowWidth, unsigned windowHeight, unsigned remainingTime);
    bool pickDiamond(unsigned x, unsigned y, DiamondCoords& coord);
    unsigned backgroundWidth() const { return m_backgroundWidth; }
    unsigned backgroundHeight() const { return m_backgroundHeight; }
    
//
// Drawing
//
private:
    static BoardRenderer* s_singleton;
    Board& m_board;
    unsigned m_windowWidth, m_windowHeight;
    Vector2D m_boardPos;  // The offset of board in BackGround.png
    
    unsigned m_backgroundWidth, m_backgroundHeight;
    GLuint m_backgroundTexture;
    std::vector<GLuint> m_diamondTextures;
    
    void loadTextures();
    bool initTextureFromRawImage(char *image, int width, int height, GLuint texName);
    
    void setupProjectAndModelViewMatrix(unsigned windowWidth, unsigned windowHeight,
                                        bool originAtLeftUpperCorner = true);
    void drawBackground();
    void drawDiamonds();
    void drawCrossOnRecentlyRemovedDiamonds();
    void drawPickedSquare();
    void drawTimeBar(unsigned remainingTimePercentage);
    void drawGameOver();
    
    Vector2D getDiamondCurrentPosition(const DiamondCoords& diamond) const;
    Vector2D getDiamondFixedPosition(const DiamondCoords& diamond) const;
    
//    
// Animation
//
private:
    Board::Animaton m_currentAnimation;
    SpritesArray m_sprites;
    
    bool m_hasPickedDiamond;
    DiamondCoords m_pickedDiamond;
    CoordsArray m_recentlyRemovedDiamonds;
    
    static const unsigned ANIMATION_TIMER_ID = 0;
    static const unsigned ANIMATION_TIMER_INTERVAL = 50; // ms
    
    static void glutTimerHandler(int value) 
    {
        if (ANIMATION_TIMER_ID == value)
            BoardRenderer::getSingleton()->animateSprites();
        else
            assert(false);
    }
    
    void setAnimationTimer(unsigned milliseconds = 0);
    void animateSprites();
    void setupSwapAnimation(const DiamondCoords& d1, const DiamondCoords& d2);
    bool anyAnimationInProgress() const { 
        return !m_sprites.empty() || !m_recentlyRemovedDiamonds.empty(); 
    }

//
// BoardDelegate methods
//
public:
    virtual void onDiamondsSwapped(Board* sender, 
                                   const DiamondCoords& d1, const DiamondCoords& d2);
    
    virtual void onPreviousSwapCancelled(Board* sender,
                                         const DiamondCoords& d1, const DiamondCoords& d2);
    
    virtual void onDiamondsRemoved(Board* sender, const CoordsArray& removedDiamonds);
    
    virtual void onDiamondsFallen(Board* sender, 
                                 const CoordsArray& fromCoordsArray, 
                                 const CoordsArray& toCoordsArray);
};

#endif
