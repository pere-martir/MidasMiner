#include "BoardRenderer.h"
#include "pngLoad.h"

BoardRenderer::BoardRenderer()
{
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
                 GL_RGB, GL_UNSIGNED_BYTE, image);
    
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


void BoardRenderer::draw(unsigned windowWidth, unsigned windowHeight, const Board& board)
{
    setupProjectAndModelViewMatrix(windowWidth, windowHeight);
    
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_TEXTURE_2D); 
   // glDisable(GL_LIGHTING);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    const unsigned int DIAMOND_SIZE = 40; // FIXME

    for (unsigned int i = 0; i < board.columns(); ++ i) {
        for (unsigned int j = 0; j < board.rows(); ++ j) {
            //unsigned primitiveIndex = j * board.columns() + i;
            unsigned int left = i * DIAMOND_SIZE, top = j * DIAMOND_SIZE;
            unsigned diamond = board(j, i);
            assert(diamond <= m_diamondTextures.size());
            glBindTexture (GL_TEXTURE_2D, m_diamondTextures[diamond-1]);
            
            glBegin(GL_QUADS);
            
            glTexCoord2f (0.0f, 0.0f); // upper left 
            glVertex2f(left, top);
            //glVertex3f(left, top, primitiveIndex);
            
            glTexCoord2f (1.0f, 0.0f); // upper right
            glVertex2f(left + DIAMOND_SIZE, top);
            //glVertex3f(left + DIAMOND_SIZE, top, primitiveIndex);
            
            glTexCoord2f (1.0f, 1.0f); // lower right
            glVertex2f(left + DIAMOND_SIZE, top + DIAMOND_SIZE);
            //glVertex3f(left + DIAMOND_SIZE, top + DIAMOND_SIZE, primitiveIndex);
            
            glTexCoord2f (0.0f, 1.0f); // lower left
            glVertex2f(left, top + DIAMOND_SIZE);
            //glVertex3f(left, top + DIAMOND_SIZE, primitiveIndex);
            
            glEnd();
        }
    }
}

bool BoardRenderer::pickDiamond(unsigned x, unsigned y, 
                                unsigned& picked_row, unsigned& picked_col) const
{
    picked_row = y / 40;
    picked_col = x / 40;
    return true;
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