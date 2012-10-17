#ifndef MidasMiner_GLUTEventHandler_h
#define MidasMiner_GLUTEventHandler_h


#include "Board.h"
#include "BoardRenderer.h"
#include "RandomNumberGenerator.h"


class GLUTEventHandler
{
public:
    GLUTEventHandler() : m_renderer(NULL), m_windowWidth(0), m_windowHeight(0) 
    {
        assert(!GLUTEventHandler::s_singleton);
        GLUTEventHandler::s_singleton = this;
    }
    
    ~GLUTEventHandler() { delete m_renderer; }
    
    static GLUTEventHandler* getSingleton() 
    { 
        assert(s_singleton);
        return s_singleton; 
    }
    
    void init(int argc, char** argv);
    void handleKeyboard(unsigned char key, int x, int y);
    void handleMouse(int button, int state, int x, int y);
    void handleDisplay();
    void handleReshape(int width, int height) {
        m_windowWidth = width;
        m_windowHeight = height;
        glViewport(0, 0, width, height);
        glutPostRedisplay();
    }
    void handleIdle() {}  
    
    static void glutTimerHandler(int value) 
    {
        assert(COUNTDOWN_TIMER_ID == value);
        GLUTEventHandler::getSingleton()->countdown();
    }

    
private:
    static GLUTEventHandler *s_singleton;
    unsigned m_windowWidth, m_windowHeight;
    Board m_board;
    BoardRenderer *m_renderer; 
    StandardLibaryRandomNumberGenerator m_randNumGenerator;
    
    std::vector<DiamondCoords> m_pickedDiamonds;
          
    static const unsigned COUNTDOWN_TIMER_ID = 1;
    static const unsigned COUNTDOWN_TIMER_INTERVAL = 100; // ms
     
    void setCountdownTimer();
    void countdown();
    bool gameOver() { return m_remainingTime <= 0; }
    int m_totalTime; // ms
    int m_remainingTime; // ms
};


#endif
