
#include "pngLoad.h"

#include "Board.h"

Board::Board()
{}


void Board::initRandomly(unsigned size, unsigned diamondTypes)
{
    srand(time(NULL));
    m_diamondMatrix = Matrix(size, size);
    for (unsigned int i = 0; i < m_diamondMatrix.columns(); ++ i) {
        for (unsigned int j = 0; j < m_diamondMatrix.rows(); ++ j) {
            m_diamondMatrix(j, i) = rand() % diamondTypes;
        }
    }
}

unsigned Board::findLines() const
{
    unsigned linesFound = 0;
    for (unsigned d = 0; d < m_diamondTypes; ++ d) {
        //unsigned diamond = m_diamondTextures[t];
        
        // Find horizonal lines
        for (unsigned i = 0; i < m_diamondMatrix.columns(); ++ i) {
            for (unsigned j = 0; j < m_diamondMatrix.rows(); ++ j) {
                unsigned x = i;
                unsigned lineLength = 0;
                while (x < m_diamondMatrix.columns() && m_diamondMatrix(j, x) == d) {
                    lineLength ++;
                    x ++;
                }
                
                if (lineLength >= 3) {
                    linesFound ++;
                    i = x;
                }
            }
        }
        
        // Find vertical lines
        for (unsigned i = 0; i < m_diamondMatrix.columns(); ++ i) {
            for (unsigned j = 0; j < m_diamondMatrix.rows(); ++ j) {
                unsigned y = j;
                unsigned lineLength = 0;
                while (y < m_diamondMatrix.rows() && m_diamondMatrix(y, i) == d) {
                    lineLength ++;
                    y ++;
                }
                
                if (lineLength >= 3) {
                    linesFound ++;
                    j = y;
                }
            }
        }
    }
    
    return linesFound;
}

// REFACTORING: move drawing function to another class


int setupGLTexture(char *image, int width, int height, GLuint texName) 
{
    
    if (image == NULL)
    {
        return 0;
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
    
    return 1; 
}


void Board::LoadTextures()
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
        setupGLTexture(imageRawData, width, height, textureName);
        m_diamondTextures.push_back(textureName);
    }
    
    assert(!m_diamondTextures.empty());
    assert(m_diamondTypes == m_diamondTextures.size());
}

void Board::draw()
{
    if (m_diamondTextures.empty()) LoadTextures();
    
    glEnable (GL_TEXTURE_2D); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    const unsigned int DIAMOND_SIZE = 40;
    for (unsigned int i = 0; i < m_diamondMatrix.columns(); ++ i) {
        for (unsigned int j = 0; j < m_diamondMatrix.rows(); ++ j) {
            
            unsigned int left = i * DIAMOND_SIZE, top = j * DIAMOND_SIZE;
            DiamondIndex d = m_diamondMatrix(j, i);
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