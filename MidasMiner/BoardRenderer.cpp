#include "BoardRenderer.h"
#include "pngLoad.h"


BoardRenderer* BoardRenderer::s_singleton = NULL;

BoardRenderer::BoardRenderer(Board& board) : m_board(board), m_boardPos(335, 110), m_hasPickedDiamond(false)
{
    assert(NULL == BoardRenderer::s_singleton);
    BoardRenderer::s_singleton = this;
    m_board.setDelegate(this);
    
    m_totalTime = m_remainingTime = 60 * 1000; // one minute, in ms
    
    loadTextures();
#if USE_PICKING_BY_COLOR_ID
    initPickingByColorID();
#endif
}

void BoardRenderer::loadTextures()
{
    //
    // Background texture
    //
    unsigned int width = 0, height = 0;
    char* imageRawData = NULL;
    int result = pngLoad(const_cast<char*>("Background.png"), &width, &height, &imageRawData);
    if (result == 0) {
        printf("(pngLoad) Background.png FAILED.\n");
        abort();
    } 
    
    m_backgroundTexture = 0;
    glGenTextures(1, &m_backgroundTexture);
    assert(m_backgroundTexture != 0);
    initTextureFromRawImage(imageRawData, width, height, m_backgroundTexture);
    free(imageRawData);
    m_backgroundWidth = width;
    m_backgroundHeight = height;

    //
    // Diamond textures
    //
    assert(m_diamondTextures.empty());
    
    const char* filenames[] = {"Blue.png", "Green.png", "Purple.png", "Red.png", "Yellow.png" };
    for (unsigned int i = 0; i < sizeof(filenames) / sizeof(filenames[0]); ++ i) {
        width = height = 0;
        imageRawData = NULL;
        result = pngLoad(const_cast<char*>(filenames[i]), &width, &height, &imageRawData);
        if (result == 0) {
            printf("(pngLoad) %s FAILED.\n", filenames[i]);
            abort();
        }
        assert(width <= DIAMOND_SIZE && height <= DIAMOND_SIZE);
        
        GLuint textureName = 0;
        glGenTextures(1, &textureName);
        assert(textureName != 0);
        initTextureFromRawImage(imageRawData, width, height, textureName);
        free(imageRawData);
        m_diamondTextures.push_back(textureName);
    }
    
    assert(!m_diamondTextures.empty());
}


bool BoardRenderer::initTextureFromRawImage(char *image, int width, int height, GLuint texName) 
{
    if (image == NULL)
    {
        return false;
    }
    
    printf("(loadTexture) width: %d height: %d\n", width, height); 
    
    /* create a new texture object
     * and bind it to texname (unsigned integer > 0)
     */
    glBindTexture(GL_TEXTURE_2D, texName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    return true; 
}

void BoardRenderer::setupProjectAndModelViewMatrix(unsigned windowWidth, unsigned windowHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Displacement trick for exact pixelization
    // http://basic4gl.wikispaces.com/2D+Drawing+in+OpenGL
    glTranslatef(0.375, 0.375, 0);    
}


void BoardRenderer::draw(unsigned windowWidth, unsigned windowHeight)
{
    if (m_totalTime == m_remainingTime) setCountdownTimer();
    
    setupProjectAndModelViewMatrix(windowWidth, windowHeight);
    
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    drawBackground();
    drawTimeBar();
    glTranslatef(m_boardPos.x, m_boardPos.y, 0);
    drawDiamonds();
    drawCrossOnRecentlyRemovedDiamonds();
    drawPickedSquare();
}

void BoardRenderer::drawBackground()
{
    glEnable(GL_TEXTURE_2D); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glBindTexture(GL_TEXTURE_2D, m_backgroundTexture);
    
    glBegin(GL_QUADS);
    
    glTexCoord2f(0.0f, 1.0f); 
    glVertex2f(0, 0);
    
    glTexCoord2f(1.0f, 1.0f); // upper right
    glVertex2f(m_backgroundWidth, 0);
    
    glTexCoord2f(1.0f, 0.0f); // lower right
    glVertex2f(m_backgroundWidth, m_backgroundHeight);
    
    glTexCoord2f(0.0f, 0.0f); // lower left
    glVertex2f(0, m_backgroundHeight);
    
    glEnd();
}

void BoardRenderer::drawTimeBar()
{
    glDisable(GL_BLEND);
    const unsigned LEFT = 50;
    const unsigned TOP = 180;
    const unsigned MAX_WIDTH = 130;
    const unsigned HEIGHT = 10;
    const unsigned VERTICAL_CENTER = HEIGHT / 2;
    unsigned width = MAX_WIDTH * (float(m_remainingTime) / m_totalTime);
    glPushMatrix();
    glTranslatef(LEFT, TOP, 0);
    
    glColor3f(1.0f, 0.0f, 0.0f);
    
    // Draw a thiner bar to indicate the total time
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex2f(0, VERTICAL_CENTER - 1);
    glVertex2f(MAX_WIDTH, VERTICAL_CENTER - 1);
    glVertex2f(MAX_WIDTH, VERTICAL_CENTER + 1);
    glVertex2f(0, VERTICAL_CENTER + 1);
    glEnd();
    
    // Draw the remaining time
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();
    
    glPopMatrix();
    glColor4f(1, 1, 1, 1);
}

void BoardRenderer::drawDiamonds()
{
    glEnable(GL_TEXTURE_2D); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //
    // Clip the drawing outside the "frame" of the mine entrance in the  
    // background image.
    //
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glColorMask(0, 0, 0, 0);

    unsigned boardSize = m_board.size() * DIAMOND_SIZE;
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(boardSize, 0);
    glVertex2f(boardSize, boardSize);
    glVertex2f(0, boardSize);
    glEnd();

    glColorMask(1, 1, 1, 1);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    //
    // Draw diamongd
    //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (unsigned int i = 0; i < m_board.columns(); ++ i) {
        for (unsigned int j = 0; j < m_board.rows(); ++ j) {
            unsigned diamond = m_board(j, i);
            if (diamond == 0) continue;
            
            DiamondCoords corrds = DiamondCoords(j, i);            
            Vector2D pos = getDiamondCurrentPosition(corrds);
            assert(diamond <= m_diamondTextures.size());
            glBindTexture(GL_TEXTURE_2D, m_diamondTextures[diamond - 1]);
            
            glBegin(GL_QUADS);
            
            glTexCoord2f(0.0f, 1.0f); // upper left 
            glVertex2f(pos.x, pos.y);

            glTexCoord2f(1.0f, 1.0f); // upper right
            glVertex2f(pos.x + DIAMOND_SIZE, pos.y);
            
            glTexCoord2f(1.0f, 0.0f); // lower right
            glVertex2f(pos.x + DIAMOND_SIZE, pos.y + DIAMOND_SIZE);
            
            glTexCoord2f(0.0f, 0.0f); // lower left
            glVertex2f(pos.x, pos.y + DIAMOND_SIZE);
            
            glEnd();
        }
    }
    glDisable(GL_STENCIL_TEST);
}

// Draw a X on the position when a diamond has just been removed
void BoardRenderer::drawCrossOnRecentlyRemovedDiamonds()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    CoordsArray::const_iterator coords = m_recentlyRemovedDiamonds.begin();
    for (; m_recentlyRemovedDiamonds.end() != coords; ++ coords) {
        glDisable(GL_TEXTURE_2D);
        glLineWidth(5);
        Vector2D pos = getDiamondFixedPosition(*coords);
        glBegin(GL_LINES);
        glVertex2f(pos.x, pos.y);
        glVertex2f(pos.x + DIAMOND_SIZE, pos.y + DIAMOND_SIZE);
        glVertex2f(pos.x + DIAMOND_SIZE, pos.y);
        glVertex2f(pos.x, pos.y + DIAMOND_SIZE);
        glEnd();
    }
    glColor4f(1, 1, 1, 1);
}

void BoardRenderer::drawPickedSquare() 
{
    if (m_hasPickedDiamond) {    
        glDisable(GL_TEXTURE_2D);
        glLineWidth(2);
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 1.0f, 1.0f);
        Vector2D pos = getDiamondFixedPosition(m_pickedDiamond);
        glVertex2f(pos.x, pos.y);
        glVertex2f(pos.x + DIAMOND_SIZE, pos.y);
        glVertex2f(pos.x + DIAMOND_SIZE, pos.y + DIAMOND_SIZE);
        glVertex2f(pos.x, pos.y + DIAMOND_SIZE);
        glEnd();
        glColor4f(1, 1, 1, 1);
    }
}

Vector2D BoardRenderer::getDiamondFixedPosition(const DiamondCoords& diamond) const
{
    return Vector2D(diamond.col * DIAMOND_SIZE, diamond.row * DIAMOND_SIZE);
}

Vector2D BoardRenderer::getDiamondCurrentPosition(const DiamondCoords& diamond) const
{
    SpritesArray::const_iterator s = m_sprites.begin();
    for (; m_sprites.end() != s; ++ s) {
        if (s->diamond == diamond) {
            return s->pos;
        }
    }
    
    return getDiamondFixedPosition(diamond);
}

void BoardRenderer::countdown()
{
    if (m_remainingTime > 0) {
        m_remainingTime -= COUNTDOWN_TIMER_INTERVAL;
        glutPostRedisplay();
        setCountdownTimer();
    }
}

void BoardRenderer::setCountdownTimer()
{
    glutTimerFunc(100, BoardRenderer::glutTimerHandler, COUNTDOWN_TIMER_ID);
}

void BoardRenderer::setAnimationTimer(unsigned milliseconds)
{
    m_hasPickedDiamond = false;
    glutTimerFunc(0 != milliseconds ? milliseconds : ANIMATION_TIMER_INTERVAL, 
                  BoardRenderer::glutTimerHandler, ANIMATION_TIMER_ID);
}

void BoardRenderer::animateSprites()
{
    bool finished = true;
    SpritesArray::iterator s = m_sprites.begin();
    for (; m_sprites.end() != s; ++ s) {
        if (s->pos != s->finalPos) {
            finished = false;
            s->pos += s->velocity;
            
            if (s->velocity.y == 0) {
                if (s->velocity.x > 0) {
                    if (s->pos.x > s->finalPos.x) 
                        s->pos.x = s->finalPos.x;
                } else if (s->velocity.x < 0) {
                    if (s->pos.x < s->finalPos.x)
                        s->pos.x = s->finalPos.x;
                } else {
                    assert(false);
                }
            } else if (s->velocity.x == 0) {
                if (s->velocity.y > 0) {
                    if (s->pos.y > s->finalPos.y) 
                        s->pos.y = s->finalPos.y;
                } else if (s->velocity.y < 0) {
                    if (s->pos.y < s->finalPos.y)
                        s->pos.y = s->finalPos.y;
                } else {
                    assert(false);
                }
            } else {
                // The diamonds can only move horizonally or vertically
                assert(false);
            }   
        }
    }
    
    if (!finished) {
        glutPostRedisplay();
        setAnimationTimer();
    } else {
        m_sprites.clear();
        m_recentlyRemovedDiamonds.clear();
        m_board.onAnimationFinished(m_currentAnimation);
    } 
}

void BoardRenderer::onDiamondsSwapped(Board* sender, 
                                      const DiamondCoords& d1, const DiamondCoords& d2)
{
    m_currentAnimation = Board::ANIMATION_SWAPPING;
    setupSwapAnimation(d1, d2);
}


void BoardRenderer::onPreviousSwapCancelled(Board* sender,
                                            const DiamondCoords& d1, const DiamondCoords& d2)
{
    m_currentAnimation = Board::ANIMATION_SWAPPING_BACK;
    setupSwapAnimation(d1, d2);
}

void BoardRenderer::setupSwapAnimation(const DiamondCoords& d1, const DiamondCoords& d2)
{
    assert(m_sprites.empty()); // we're not currently animating anything
    m_sprites.clear();
    
    Sprite s1;
    s1.diamond = d1;
    s1.pos = getDiamondCurrentPosition(d2);
    s1.finalPos = getDiamondCurrentPosition(d1);
    s1.velocity = (s1.finalPos - s1.pos) / 10;
    
    Sprite s2;
    s2.diamond = d2;
    s2.pos = getDiamondCurrentPosition(d1);
    s2.finalPos = getDiamondCurrentPosition(d2);
    s2.velocity = (s2.finalPos - s2.pos) / 10;
    
    m_sprites.push_back(s1);
    m_sprites.push_back(s2);
    
    setAnimationTimer();
}

void BoardRenderer::onDiamondsRemoved(Board* sender, const CoordsArray& removedDiamonds)
{
    assert(m_sprites.empty()); // we're not currently animating anything
    m_sprites.clear();
    m_currentAnimation = Board::ANIMATION_REMOVING;
    glutPostRedisplay();
    m_recentlyRemovedDiamonds.insert(m_recentlyRemovedDiamonds.end(), 
                                     removedDiamonds.begin(), removedDiamonds.end());
    setAnimationTimer(500);
}

void BoardRenderer::onDiamondsFallen(Board* sender, 
                                     const CoordsArray& fromCoordsArray, 
                                     const CoordsArray& toCoordsArray)
{
    assert(m_sprites.empty()); // we're not currently animating anything
    m_sprites.clear();
    
    for (unsigned i = 0; i < toCoordsArray.size(); ++ i) {
        Sprite s;
        s.diamond = toCoordsArray[i];
        s.pos = getDiamondFixedPosition(fromCoordsArray[i]);
        s.finalPos = getDiamondFixedPosition(toCoordsArray[i]);
        s.velocity = (s.finalPos - s.pos) / 5;
        m_sprites.push_back(s);
    }
    m_currentAnimation = Board::ANIMATION_FALLING;
    setAnimationTimer();
}

bool BoardRenderer::pickDiamond(unsigned x, unsigned y, DiamondCoords& coord)
{
    if (anyAnimationInProgress() || m_remainingTime == 0) return false; 
    
#if !defined(USE_PICKING_BY_COLOR_ID)  
    int row = (y - m_boardPos.y) / DIAMOND_SIZE;
    int col = (x - m_boardPos.x) / DIAMOND_SIZE;
    if (row >= 0 && row < m_board.rows() && col >= 0 && col < m_board.columns()) {
        coord = DiamondCoords(row, col);
        m_pickedDiamond = coord;
        m_hasPickedDiamond = true;
    }
    
    return m_hasPickedDiamond;
#else
    //glDrawBuffer(GL_BACK);
    /*glBindTexture(GL_TEXTURE_2D, 0);
     glEnable(GL_TEXTURE_2D);
     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
     */
    setupProjectAndModelViewMatrix();
    renderer->drawInPickingMode(*board);   
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    GLubyte pixel[4];
    //glReadBuffer(GL_BACK);
    
    //glReadBuffer(GL_COLOR_ATTACHMENT0);
    
    glReadBuffer(GL_BACK);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, viewport[3] - y, 1, 1,
                 GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixel);
    GLenum error = glGetError();
    //if (error != GL_NO_ERROR)
    //    assert(false && "opengl error");
    printf("\npicked (%d,%d)", pixel[0], pixel[1]);
    
    // draw();
    //glutPostRedisplay();
    
    //glReadBuffer(GL_NONE);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#endif
}

//
// This is not used
//
#if USE_PICKING_BY_COLOR_ID
void BoardRenderer::initPickingByColorID()
{
    glGenFramebuffers(1, &fbo); 
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // Create the texture object for the primitive information buffer
    glGenTextures(1, &pickingTexture);
    glBindTexture(GL_TEXTURE_2D, pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kWindowWidth, kWindowHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                           pickingTexture, 0); 
    
    // Verify that the FBO is correct
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        abort();
    }
    
    // Restore the default framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BoardRenderer::drawInPickingMode(const Board& board)
{
    setupProjectAndModelViewMatrix();
    
    glDisable(GL_TEXTURE_2D); 
    //glDisable(GL_DITHER);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClear(GL_COLOR_BUFFER_BIT);
    
    const unsigned int DIAMOND_SIZE = 40; // FIXME
    for (unsigned int i = 0; i < board.columns(); ++ i) {
        for (unsigned int j = 0; j < board.rows(); ++ j) {
            unsigned int left = i * DIAMOND_SIZE, top = j * DIAMOND_SIZE;
            glBegin(GL_QUADS);
            glColor4ub(j, i, 0, 0);
            //glColor4ub(100, 100, 0, 0);
            glVertex2f(left, top);
            glVertex2f(left + DIAMOND_SIZE, top);
            glVertex2f(left + DIAMOND_SIZE, top + DIAMOND_SIZE);
            glVertex2f(left, top + DIAMOND_SIZE);
            glEnd();
        }
    }
    //glEnable(GL_DITHER);
    glEnable(GL_TEXTURE_2D); 
}
#endif