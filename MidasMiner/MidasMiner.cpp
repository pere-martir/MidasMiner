#include <GLUT/glut.h>
#include "MidasMiner.h"

void GLUTEventHandler::init(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("PNG Texture Example");
    
    m_board.initRandomly(8, 5, m_randNumGenerator);
}

void GLUTEventHandler::handleDisplay() 
{
    if (!m_renderer) m_renderer = new BoardRenderer();
    m_renderer->draw(WINDOW_WIDTH, WINDOW_HEIGHT, m_board); 
    glutSwapBuffers();
}

void GLUTEventHandler::handleKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:    /* ESC key */
        case 'Q':   /* Q|q keys */
        case 'q':
            exit(0);
            break;
        default:
            break;
    }
}

void GLUTEventHandler::handleMouse(int button, int state, int x, int y)
{
    if (GLUT_LEFT_BUTTON == button && GLUT_UP == state) {
        
#if USE_FBO_FOR_PICKING
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
#else
        unsigned picked_row = 0, picked_col = 0;
        if (m_renderer->pickDiamond(x, y, picked_row, picked_col)) {
            printf("\npicked (%d,%d)", picked_row, picked_col);
        }
#endif
    }
}