#include <GLUT/glut.h>
#include "MidasMiner.h"

void GLUTEventHandler::init(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("PNG Texture Example");
    
    m_board.initRandomly(4, 5, m_randNumGenerator);
}

void GLUTEventHandler::handleDisplay() 
{
    if (!m_renderer) {
        m_renderer = new BoardRenderer(m_board);
    }

    m_renderer->draw(WINDOW_WIDTH, WINDOW_HEIGHT); 
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
        
        // refactoring to a controller
        DiamondCoords picked;
        if (m_renderer->pickDiamond(x, y, picked)) {
            glutPostRedisplay();
            if (m_pickedDiamonds.size() < 2) {
                printf("\npicked (%d,%d)", picked.row, picked.col);
                m_pickedDiamonds.push_back(picked);
            } 
            if (m_pickedDiamonds.size() == 2) {
                if (m_board.swap(m_pickedDiamonds[0], m_pickedDiamonds[1])) {
                    m_board.collapse();
                }
                    
                m_pickedDiamonds.clear();
            }
        }

    }
}