#include <GLUT/glut.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

static char state[5] = "1230";


const unsigned pencolor[][3] = {
    {109, 110, 101}, // 0. background & label
    {205, 193, 180}, // 1. bracket 1
    {238, 228, 218}, // 2. bracket 2
    {255, 97, 0} // 3. hint
};

char text_toshow[256] = "                 3-puzzle Game";
void * font = GLUT_BITMAP_HELVETICA_18;
int color = 8;
void show_text()
{
    // There are 7 bitmap fonts available in GLUT. They are
    // GLUT_BITMAP_8_BY_13
    // GLUT_BITMAP_9_BY_15
    // GLUT_BITMAP_TIMES_ROMAN_10
    // GLUT_BITMAP_TIMES_ROMAN_24
    // GLUT_BITMAP_HELVETICA_10
    // GLUT_BITMAP_HELVETICA_12
    // GLUT_BITMAP_HELVETICA_18
    
    // draw hints
    glColor3ub(pencolor[3][0],
               pencolor[3][1],
               pencolor[3][2]);
    glRasterPos2f(-1, 0.85);
    for (char* c = text_toshow; *c; ++c)
        glutBitmapCharacter(font, *c);
    
    if (state[0] != '0') {
        glColor3ub(pencolor[0][0], pencolor[0][1], pencolor[0][2]);
        glRasterPos2f(-0.5, 0.3);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, state[0]);
    }
    
    if (state[1] != '0') {
        glColor3ub(pencolor[0][0], pencolor[0][1], pencolor[0][2]);
        glRasterPos2f(0.5, 0.3);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, state[1]);
    }
    
    if (state[2] != '0') {
        glColor3ub(pencolor[0][0], pencolor[0][1], pencolor[0][2]);
        glRasterPos2f(-0.5, -0.6);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, state[2]);
    }
    
    if (state[3] != '0') {
        glColor3ub(pencolor[0][0], pencolor[0][1], pencolor[0][2]);
        glRasterPos2f(0.5, -0.6);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, state[3]);
    }
}

void display(void) {
    glClearColor(0.9804, 0.9725, 0.9373, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POLYGON);
    glColor3ub(pencolor[1][0], pencolor[1][1], pencolor[1][2]);
    glVertex2f(-1, 0.818);
    glVertex2f(0, 0.818);
    glVertex2f(0, -0.091);
    glVertex2f(-1, -0.091);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3ub(pencolor[2][0], pencolor[2][1], pencolor[2][2]);
    glVertex2f(1, 0.818);
    glVertex2f(1, -0.091);
    glVertex2f(0, -0.091);
    glVertex2f(0, 0.818);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3ub(pencolor[2][0], pencolor[2][1], pencolor[2][2]);
    glVertex2f(-1, -0.091);
    glVertex2f(0, -0.091);
    glVertex2f(0, -1);
    glVertex2f(-1, -1);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3ub(pencolor[1][0], pencolor[1][1], pencolor[1][2]);
    glVertex2f(1, -1);
    glVertex2f(0, -1);
    glVertex2f(0, -0.091);
    glVertex2f(1, -0.091);
    glEnd();
    
    show_text();
    glFlush();
}

char control[20], temp[10];
void controller(void) {
    scanf("%s", control);
    //printf("control=[%s]\n", control);
    if (!strcmp(control, "BEGIN")) {
        font = GLUT_BITMAP_HELVETICA_18;
        strcpy(text_toshow, "              Input a lucky number");
    } else if (!strcmp(control, "DRAW")) {
        scanf("%s", state);
    } else if (!strcmp(control, "STEP")) {
        font = GLUT_BITMAP_HELVETICA_12;
        color = 6;
        scanf("%s", temp);
        strcpy(text_toshow, "?: hint E: end A: restart 1~3: make a move  STEP: ");
        strcat(text_toshow, temp);
    } else if (!strcmp(control, "ILLEGAL")) {
        font = GLUT_BITMAP_HELVETICA_18;
        strcpy(text_toshow, "                    Illegal Move!");
    } else if (!strcmp(control, "SUCCESS")) {
        font = GLUT_BITMAP_HELVETICA_18;
        strcpy(text_toshow, "         Success! A: restart E: end");
    } else if (!strcmp(control, "END")) {
        exit(0);
    }
    //printf("state = %s, text_toshow = [%s]\n", state, text_toshow);
    display();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(300, 330);
    glutCreateWindow("3-puzzle Game");
    glutDisplayFunc(&display);
    glutIdleFunc(&controller);
    glutMainLoop();
    return 0;
}
