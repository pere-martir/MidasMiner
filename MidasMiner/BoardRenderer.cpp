#include "BoardRenderer.h"
#include "pngLoad.h"


BoardRenderer* BoardRenderer::s_singleton = NULL;

BoardRenderer::BoardRenderer(Board& board) : m_board(board), m_hasPickedDiamond(false), 
                                             m_animationFinished(true)
{
    assert(NULL == BoardRenderer::s_singleton);
    BoardRenderer::s_singleton = this;
    
    m_board.setDelegate(this);
    assert(m_diamondTextures.empty());
    
    const char* filenames[] = {"Blue.png", "Green.png", "Purple.png", "Red.png", "Yellow.png" };
    for (unsigned int i = 0; i < sizeof(filenames) / sizeof(filenames[0]); ++ i) {
        unsigned int width = 0, height = 0;
        char* imageRawData = NULL;
        int result = pngLoad(const_cast<char*>(filenames[i]), &width, &height, &imageRawData);
        if (result == 0)
        {
            printf("(pngLoad) %s FAILED.\n", filenames[i]);
            abort();
        }
        
        GLuint textureName = 0;
        glGenTextures(1, &textureName);
        assert(textureName != 0);
        initTextureFromRawImage(imageRawData, width, height, textureName);
        m_diamondTextures.push_back(textureName);
    }
    
    assert(!m_diamondTextures.empty());
    
#if USE_PICKING_BY_COLOR_ID
    initPickingByColorID();
#endif
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
    glTranslatef (0.375, 0.375, 0);
}


void BoardRenderer::draw(unsigned windowWidth, unsigned windowHeight)
{
    setupProjectAndModelViewMatrix(windowWidth, windowHeight);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_TEXTURE_2D); 
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    for (unsigned int i = 0; i < m_board.columns(); ++ i) {
        for (unsigned int j = 0; j < m_board.rows(); ++ j) {
            DiamondCoords corrds = DiamondCoords(j, i);
            
            Vector2D pos = getDiamondPosition(corrds);
            unsigned diamond = m_board(j, i);
            assert(diamond <= m_diamondTextures.size());
            glBindTexture(GL_TEXTURE_2D, m_diamondTextures[diamond - 1]);
            
            glBegin(GL_QUADS);
            
            glTexCoord2f (0.0f, 0.0f); // upper left 
            glVertex2f(pos.x, pos.y);
            
            glTexCoord2f (1.0f, 0.0f); // upper right
            glVertex2f(pos.x + DIAMOND_SIZE, pos.y);
            
            glTexCoord2f (1.0f, 1.0f); // lower right
            glVertex2f(pos.x + DIAMOND_SIZE, pos.y + DIAMOND_SIZE);
            
            glTexCoord2f (0.0f, 1.0f); // lower left
            glVertex2f(pos.x, pos.y + DIAMOND_SIZE);
            
            glEnd();
        }
    }

    if (m_hasPickedDiamond) {    
        glDisable(GL_TEXTURE_2D);
        glLineWidth(2);
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 1.0f, 1.0f);
        Vector2D pos = getDiamondPosition(m_pickedDiamond);
        glVertex2f(pos.x, pos.y);
        glVertex2f(pos.x + DIAMOND_SIZE, pos.y);
        glVertex2f(pos.x + DIAMOND_SIZE, pos.y + DIAMOND_SIZE);
        glVertex2f(pos.x, pos.y + DIAMOND_SIZE);
        glEnd();
        glColor4f(1, 1, 1, 1);
    }
}

Vector2D BoardRenderer::getDiamondPosition(const DiamondCoords& diamond) const
{
    SpritesArray::const_iterator s = m_sprites.begin();
    for (; m_sprites.end() != s; ++ s) {
        if (s->diamond == diamond) {
            return s->pos;
        }
    }
    
    return Vector2D(diamond.col * DIAMOND_SIZE, diamond.row * DIAMOND_SIZE);
}


void BoardRenderer::onDiamondsFallen(Board* sender, 
                                    const CoordsArray& toCoordsArray, 
                                    const CoordsArray& fromCoordsArray)
{
    assert(false);
    
    assert(m_sprites.empty()); // we're not currently animating anything
    m_sprites.clear();
    for (unsigned i = 0; i < toCoordsArray.size(); ++ i) {
        Sprite s;
        s.diamond = toCoordsArray[i];
        s.pos = getDiamondPosition(fromCoordsArray[i]);
        m_sprites.push_back(s);
    }
        
    m_animationFinished = false;
    glutTimerFunc(100, BoardRenderer::glutTimerHandler, 0);
}

void BoardRenderer::setTimer()
{
    glutTimerFunc(50, BoardRenderer::glutTimerHandler, 0);
}

void BoardRenderer::onTimer()
{
    bool moving = false;
    SpritesArray::iterator s = m_sprites.begin();
    for (; m_sprites.end() != s; ++ s) {
        if (s->pos != s->finalPos) {
            s->pos += s->velocity;
            moving = true;
        }
    }
    
    if (moving) {
        glutPostRedisplay();
        setTimer();
    } else {
        m_sprites.clear();
        m_board.onAnimationFinished(m_currentAnimation);
    } 
}

void BoardRenderer::onDiamondsSwapped(Board* sender, 
                                      const DiamondCoords& d1, const DiamondCoords& d2)
{
    m_hasPickedDiamond = false;
    m_currentAnimation = Board::ANIMATION_SWAPPING;
    setupSwapAnimation(d1, d2);
}

void BoardRenderer::setupSwapAnimation(const DiamondCoords& d1, const DiamondCoords& d2)
{
    assert(m_sprites.empty()); // we're not currently animating anything
    m_sprites.clear();
    
    Sprite s1;
    s1.diamond = d1;
    s1.pos = getDiamondPosition(d2);
    s1.finalPos = getDiamondPosition(d1);
    s1.velocity = (s1.finalPos - s1.pos) / 10;
    
    Sprite s2;
    s2.diamond = d2;
    s2.pos = getDiamondPosition(d1);
    s2.finalPos = getDiamondPosition(d2);
    s2.velocity = (s2.finalPos - s2.pos) / 10;
    
    m_sprites.push_back(s1);
    m_sprites.push_back(s2);
    
  
    setTimer();
}

void BoardRenderer::onPreviousSwapCancelled(Board* sender,
                                            const DiamondCoords& d1, const DiamondCoords& d2)
{
    m_currentAnimation = Board::ANIMATION_SWAPPING_BACK;
    setupSwapAnimation(d1, d2);
}

/*
bool BoardRenderer::doesSprtesContainDiamond(const DiamondCoords& d) const
{
    SpritesArray::const_iterator s = m_sprites.begin();
    for (; m_sprites.end() != s; ++ s) {
        if (s->diamond == d) return true;
    }
    return false;
}
*/


bool BoardRenderer::pickDiamond(unsigned x, unsigned y, DiamondCoords& coord)
{
#if !defined(USE_PICKING_BY_COLOR_ID)
    m_hasPickedDiamond = true;
    coord = DiamondCoords(y / DIAMOND_SIZE, x / DIAMOND_SIZE);
    m_pickedDiamond = coord;
    return true;
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