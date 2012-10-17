#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "UnitTest++.h"
#include "MidasMiner.h"

// GLUT callbacks are native C function pointers. Neither Boost.Bind nor mem_fn can
// convert class member functions to them, so we have to use a small C delegate 
// function to call our own C++ handler. The global object is not a perfect design
// of singleton but it's simple and enough for this program.
static GLUTEventHandler g_eventHandler;

void display(void) 
{    
    //glViewport(0, 0, 320, 320);
    g_eventHandler.handleDisplay();
}

void reshape(int width, int height)
{
    g_eventHandler.handleReshape(width, height);
}

void keyboard(unsigned char key, int x, int y)
{
    g_eventHandler.handleKeyboard(key, x, y);
}

void mouse(int button, int state, int x, int y)
{
    g_eventHandler.handleMouse(button, state, x, y);
}

void idle()
{
    g_eventHandler.handleIdle();
}

int main(int argc, char** argv)
{
#if 1
    g_eventHandler.init(argc, argv);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);
    glutMainLoop();
#else
    UnitTest::RunAllTests();
#endif
    
    return 0;
}

