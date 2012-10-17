#include "BoardRenderer.h"
#include "pngLoad.h"


BoardRenderer* BoardRenderer::s_singleton = NULL;

BoardRenderer::BoardRenderer(Board& board) : m_board(board), m_boardPos(335, 110), m_hasPickedDiamond(false)
{
    assert(NULL == BoardRenderer::s_singleton);
    BoardRenderer::s_singleton = this;
    m_board.setDelegate(this);

    
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

// There is no need to use complicated OpenGL color-ID picking.
bool BoardRenderer::pickDiamond(unsigned x, unsigned y, DiamondCoords& coord)
{
    if (anyAnimationInProgress()) return false; 
    
    int row = (y - m_boardPos.y) / DIAMOND_SIZE;
    int col = (x - m_boardPos.x) / DIAMOND_SIZE;
    if (row >= 0 && row < m_board.rows() && col >= 0 && col < m_board.columns()) {
        coord = DiamondCoords(row, col);
        m_pickedDiamond = coord;
        m_hasPickedDiamond = true;
    }
    
    return m_hasPickedDiamond;
}

// Setup the origin at the upper-left corner if originAtLeftUpperCorner = true (default)
// Setup the origin at the lower-left corner if originAtLeftUpperCorner = false,
// it's only used when drawing the text.
void BoardRenderer::setupProjectAndModelViewMatrix(unsigned windowWidth, 
                                                   unsigned windowHeight, 
                                                   bool originAtLeftUpperCorner)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (originAtLeftUpperCorner)
        glOrtho(0, windowWidth, windowHeight, 0, 0, 1);
    else
        glOrtho(0, windowWidth, 0, windowHeight, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Displacement trick for exact pixelization
    // http://basic4gl.wikispaces.com/2D+Drawing+in+OpenGL
    glTranslatef(0.375, 0.375, 0);    
    
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
}


void BoardRenderer::draw(unsigned windowWidth, unsigned windowHeight,
                         unsigned remainingTimePercentage)
{
    setupProjectAndModelViewMatrix(windowWidth, windowHeight);
    
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    drawBackground();
    drawTimeBar(remainingTimePercentage);
    glTranslatef(m_boardPos.x, m_boardPos.y, 0);
    drawDiamonds();
    
    if (0 < remainingTimePercentage) {
        drawCrossOnRecentlyRemovedDiamonds();
        drawPickedSquare();
    } else {
        drawGameOver();
    }
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

void BoardRenderer::drawTimeBar(unsigned remainingTimePercentage)
{
    glDisable(GL_BLEND);
    const unsigned LEFT = 50;
    const unsigned TOP = 180;
    const unsigned MAX_WIDTH = 130;
    const unsigned HEIGHT = 10;
    const unsigned VERTICAL_CENTER = HEIGHT / 2;
    unsigned width = MAX_WIDTH * float(remainingTimePercentage) / 100;
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
    glDisable(GL_TEXTURE_2D);
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

void BoardRenderer::drawGameOver()
{
    //
    // Gray-out the diamonds
    //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.4f);
    unsigned boardSize = m_board.size() * DIAMOND_SIZE;
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(boardSize, 0);
    glVertex2f(boardSize, boardSize);
    glVertex2f(0, boardSize);
    glEnd();
    
    glPushMatrix();
    // Reverse the model view matrix because glutStrokeCharacter assumes the origin
    // is at the lower-left.
    setupProjectAndModelViewMatrix(m_windowWidth, m_windowHeight, false);
    
    glColor4f(0, 1, 0, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    
    glPushMatrix();
    glTranslatef(m_boardPos.x, m_windowHeight - m_boardPos.y - 100, 0);
    glScalef(0.4f, 0.4f, 0.4f);    
    glLineWidth(3);
    const char* GAMEOVER = "GAME OVER";
    const char* PRESS_ESC = "PRESS ESC TO LEAVE THE GAME";
    for (unsigned i = 0; i < strlen(GAMEOVER); ++ i)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, GAMEOVER[i]);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(m_boardPos.x - 10, m_windowHeight - m_boardPos.y - 200, 0);
    glScalef(0.15f, 0.15f, 0.15f);    
    glLineWidth(2);
    for (unsigned i = 0; i < strlen(PRESS_ESC); ++ i)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, PRESS_ESC[i]);    
    glPopMatrix();
    
    glPushMatrix();
    glColor4f(1, 1, 1, 1);
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
    assert(!anyAnimationInProgress());
    m_currentAnimation = Board::ANIMATION_SWAPPING;
    setupSwapAnimation(d1, d2);
}


void BoardRenderer::onPreviousSwapCancelled(Board* sender,
                                            const DiamondCoords& d1, const DiamondCoords& d2)
{
    assert(!anyAnimationInProgress());
    m_currentAnimation = Board::ANIMATION_SWAPPING_BACK;
    setupSwapAnimation(d1, d2);
}

void BoardRenderer::setupSwapAnimation(const DiamondCoords& d1, const DiamondCoords& d2)
{
    assert(!anyAnimationInProgress());
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
    assert(!anyAnimationInProgress());
    
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
    assert(!anyAnimationInProgress());
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
