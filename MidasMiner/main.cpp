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
#include <GLUT/glut.h>




//#include "boost/assert.hpp"


#include "UnitTest++.h"
#include "Board.h"

int kWindowWidth = 320; /* window width */
int kWindowHeight = 320; /* window height */

void display(void) 
{    
    if (!kWindowHeight)
        return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0, kWindowWidth, kWindowHeight, 0, 0, 1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    // Displacement trick for exact pixelization
    // http://basic4gl.wikispaces.com/2D+Drawing+in+OpenGL
    glTranslatef (0.375, 0.375, 0);
    
    static Board *board = NULL;
    if (!board) {
        board = new Board();
        board->initRandomly();
    }
    board->draw();   
    
    glDisable(GL_TEXTURE_2D); /* disable texture mapping */
    glutSwapBuffers();
}

/*
 * Callback function registered to glutReshapeFunc
 */
void reshape(int w, int h)
{
    /* window has been resized */
    kWindowWidth = w;
    kWindowHeight = h;
    
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    /* 
     * gluPerspective fovy aspect near far
     *
     * GLdouble fovy   - angle of the field of view 
     *                   in the yz plane [range 0.0 - 180.0]
     * GLdouble aspect - aspect ratio of frustrum width/height
     * GLdouble near   - distance to the view plane along -z axis
     * GLdouble far    - distance to the clipping plane along -z axis
     *
     * see Chapter 3, Viewing, OpenGL Programming Guide -Red Book.
     * see Chapter 5, Viewing, Interactive Computer Graphics
     */
    gluPerspective(55, (GLfloat) w / (GLfloat) h, 1, 9999);
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
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


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("PNG Texture Example");
    
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    UnitTest::RunAllTests();
    return 0;
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    
    return 0;
}

