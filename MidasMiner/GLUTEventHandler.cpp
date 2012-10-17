#include <GLUT/glut.h>
#include "GLUTEventHandler.h"

GLUTEventHandler *GLUTEventHandler::s_singleton = NULL;

void GLUTEventHandler::init(int argc, char** argv)
{
    m_board.initRandomly(8, 5, m_randNumGenerator);
 
    // Small non-random boards. They are only used for testing.
#if 0
    // Eliminate a horizonal line
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
#endif
    
#if 0
    // Eliminate a vertical line
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
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
    // Initialize the window to be the same size of BackGround.png.
    // FIXME: read the size from BoardRenderer.
    glutInitWindowSize(755, 600);
    glutCreateWindow("Midas Miner");
    m_renderer = new BoardRenderer(m_board);
}

void GLUTEventHandler::handleDisplay() 
{
    if (m_remainingTime == m_totalTime) setCountdownTimer();
    int remainingPercentage = 100 * float(m_remainingTime) / m_totalTime;
    m_renderer->draw(m_windowWidth, m_windowHeight, remainingPercentage); 
    glutSwapBuffers();
}

void GLUTEventHandler::handleKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: /* ESC key */
        case 'Q':
        case 'q':
            exit(0);
            break;
        default:
            break;
    }
}

void GLUTEventHandler::handleMouse(int button, int state, int x, int y)
{
    if (m_remainingTime == 0) return;
    
    if (GLUT_LEFT_BUTTON == button && GLUT_UP == state) {
        
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


void GLUTEventHandler::countdown()
{
    if (m_remainingTime > 0) {
        m_remainingTime -= COUNTDOWN_TIMER_INTERVAL;
        if (m_remainingTime < 0) m_remainingTime = 0;
        glutPostRedisplay();
        setCountdownTimer();
    }
}

void GLUTEventHandler::setCountdownTimer()
{
    glutTimerFunc(100, GLUTEventHandler::glutTimerHandler, COUNTDOWN_TIMER_ID);
}
