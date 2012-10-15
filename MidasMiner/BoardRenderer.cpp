#include "BoardRenderer.h"
#include "pngLoad.h"


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
}

void BoardRenderer::draw(const Board& board)
{
    glEnable (GL_TEXTURE_2D); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    const unsigned int DIAMOND_SIZE = 40; // FIXME
    for (unsigned int i = 0; i < board.columns(); ++ i) {
        for (unsigned int j = 0; j < board.rows(); ++ j) {
            
            unsigned int left = i * DIAMOND_SIZE, top = j * DIAMOND_SIZE;
            unsigned d = board(j, i);
            assert(d < m_diamondTextures.size());
            glBindTexture (GL_TEXTURE_2D, m_diamondTextures[d]);
            
            glBegin (GL_QUADS);
            
            glTexCoord2f (0.0f, 0.0f); // upper left 
            glVertex2f (left, top);
            
            glTexCoord2f (1.0f, 0.0f); // upper right
            glVertex2f (left + DIAMOND_SIZE, top);
            
            glTexCoord2f (1.0f, 1.0f); // lower right
            glVertex2f (left + DIAMOND_SIZE, top + DIAMOND_SIZE);
            
            glTexCoord2f (0.0f, 1.0f); // lower left
            glVertex2f (left, top + DIAMOND_SIZE);
            
            glEnd ();
        }
    }
}