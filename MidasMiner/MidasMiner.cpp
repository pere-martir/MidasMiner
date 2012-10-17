#include <GLUT/glut.h>
#include "MidasMiner.h"

void GLUTEventHandler::init(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Midas Miner");
    
#if 0
    m_board.initRandomly(4, 5, m_randNumGenerator);
#else
    /*
    Matrix m;
    m.initWithElements(4, 4, 
                       3, 3, 1, 2,
                       1, 2, 3, 1, 
                       2, 1, 2, 2,
                       1, 2, 1, 2);
    m_board.initWithMatrix(m);
    Matrix future;
    future.initWithElements(1, 4, 
                       3, 4, 5, 1);
    m_board.setFutureMatrix(future);
    */
    
    
    Matrix m;
    m.initWithElements(4, 4, 
                       2, 1, 2, 1,
                       1, 2, 3, 2, 
                       2, 1, 3, 1,
                       1, 3, 1, 2);
     m_board.initWithMatrix(m);
     Matrix future;
     future.initWithElements(3, 4, 
                             1, 2, 5, 1,
                             2, 1, 4, 2,
                             1, 2, 4, 1);
     m_board.setFutureMatrix(future);
    
#endif
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
                DiamondCoords d1 = m_pickedDiamonds[0];
                DiamondCoords d2 = m_pickedDiamonds[1];
                m_pickedDiamonds.clear();
                m_board.swap(d1, d2);                    
            }
        }

    }
}