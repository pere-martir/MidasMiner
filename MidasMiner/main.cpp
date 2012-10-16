/*
 * file: pngDrvr.c
 * author: mnorton
 * 
 * description:
 * This program is the example driver code for the pngLoad.c 
 * library. The program uses a modified version of Example 9-1
 * source code, from Chapter 9, Texture Mapping, from
 * The OpenGL Programming Guide v1.2 (aka the Red Book).
 * This source code also implements components of code found
 * Nate Miller's OpenGL Texture Mapping: An Example.
 *
 * This example loads a sample 256 x 256 pixel PNG file and
 * displays it in an OpenGL application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "UnitTest++.h"
#include "MidasMiner.h"

MidasMiner g_midasMinder;

void display(void) 
{    
    g_midasMinder.glutDisplayHandler();
}

void keyboard(unsigned char key, int x, int y)
{
    g_midasMinder.glutKeyboardHandler(key, x, y);
}

void mouse(int button, int state, int x, int y)
{
    g_midasMinder.glutMouseHanlder(button, state, x, y);
}

int main(int argc, char** argv)
{
#if 0
    g_midasMinder.init(argc, argv);
    glutDisplayFunc(display);
    //glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMouseFunc(mouse);
    glutMainLoop();
#else
    UnitTest::RunAllTests();
#endif
    
    return 0;
}

