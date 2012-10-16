#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "UnitTest++.h"
#include "MidasMiner.h"

GLUTEventHandler g_eventHandler;

void display(void) 
{    
    g_eventHandler.handleDisplay();
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
    //glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);
    glutMainLoop();
#else
    UnitTest::RunAllTests();
#endif
    
    return 0;
}

