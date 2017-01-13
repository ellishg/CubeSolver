//
//  main.cpp
//  RenderTest
//
//  Created by Ellis Sparky Hoag on 7/3/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "CubeDataStructure.h"

using namespace std;
using namespace CubeDataStructure;

Cube my_cube;
Solver my_solver("/Users/ellis/GitHub/CubeSolver/CubeDataStructure/Tables");

float rot_x, rot_y;

SDL_Window * window;
SDL_Event event;
SDL_GLContext context;

const int window_size = 500;

void render();
void render_face(uint32_t color_list);
void events();
void scramble_cube();
bool init();
void quit();

int main(int argc, const char * argv[])
{
    srand((unsigned)time(NULL));
    
    if (!init()) {quit();}
        
    rot_x = 30;
    rot_y = -45;
    
    while (true)
    {
        render();
        events();
    }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    glLoadIdentity();
    
    glTranslatef(0.f, 0.f, -10.f);
    
    glRotatef(rot_x, 1, 0, 0);
    glRotatef(rot_y, 0, 1, 0);
    
    uint32_t facelet_level[6];
    
    my_cube.get_facelet_level(facelet_level);
    
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        render_face(facelet_level[4]);
    glPopMatrix();
    
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        render_face(facelet_level[5]);
    glPopMatrix();
    
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
            render_face(facelet_level[i]);
        glPopMatrix();
        glRotatef(90, 0, 1, 0);
    }
    
    SDL_GL_SwapWindow(window);
}

void render_face(uint32_t color_list)
{
    
    const float face_separation = 0.1f;
    
    glTranslatef(-2.f - face_separation, 2.f + face_separation, 3.f + face_separation);
    
    for (int i = 0; i < 9; i++)
    {
        uint8_t color = (color_list >> (i * 3)) & 0b111;
        
        switch (color) {
            case RED:
                glColor3f(1.f, 0.f, 0.f);
                break;
            case BLUE:
                glColor3f(0.f, 0.f, 1.f);
                break;
            case ORANGE:
                glColor3f(1.f, 0.5f, 0.f);
                break;
            case GREEN:
                glColor3f(0.f, 1.f, 0.f);
                break;
            case WHITE:
                glColor3f(1.f, 1.f, 1.f);
                break;
            case YELLOW:
                glColor3f(1.f, 1.f, 0.f);
                break;
            default:
                cout << "Invalid color!\n";
                quit();
                break;
        }
        
        glBegin(GL_QUADS);
            glVertex3f(-1.f, 1.f, 0.f);
            glVertex3f(-1.f, -1.f, 0.f);
            glVertex3f(1.f, -1.f, 0.f);
            glVertex3f(1.f, 1.f, 0.f);
        glEnd();
        
        glTranslatef(2.f + face_separation, 0.f, 0.f);
        
        if (i % 3 == 2)
        {
            glTranslatef(-3.f * (2.f + face_separation), -2.f - face_separation, 0.f);
        }
    }
}

void events()
{
    const float rot_increment = 10.f;
    
    stack<tuple<int, int>> solution;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit();
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit();
                        break;
                    case SDLK_u:
                        my_cube.rotate_face(UP, CLOCKWISE);
                        cout << "U";
                        break;
                    case SDLK_f:
                        my_cube.rotate_face(FRONT, CLOCKWISE);
                        cout << "F";
                        break;
                    case SDLK_r:
                        my_cube.rotate_face(RIGHT, CLOCKWISE);
                        cout << "R";
                        break;
                    case SDLK_b:
                        my_cube.rotate_face(BACK, CLOCKWISE);
                        cout << "B";
                        break;
                    case SDLK_l:
                        my_cube.rotate_face(LEFT, CLOCKWISE);
                        cout << "L";
                        break;
                    case SDLK_d:
                        my_cube.rotate_face(DOWN, CLOCKWISE);
                        cout << "D";
                        break;
                    case SDLK_UP:
                        rot_x += rot_increment;
                        break;
                    case SDLK_DOWN:
                        rot_x -= rot_increment;
                        break;
                    case SDLK_LEFT:
                        rot_y += rot_increment;
                        break;
                    case SDLK_RIGHT:
                        rot_y -= rot_increment;
                        break;
                    case SDLK_s:
                        solution = my_solver.get_solution(my_cube);
                        cout << "\nSolution = ";
                        my_solver.print_move_list(solution);
                        break;
                    case SDLK_m:
                        scramble_cube();
                        break;
                    case SDLK_p:
                        cout << my_cube;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void scramble_cube()
{
    stack<tuple<int, int>> scramble = my_solver.get_scramble(100);
    
    cout << "\nScramble = ";
    my_solver.print_move_list(scramble);

    while (!scramble.empty())
    {
        int face = get<0>(scramble.top());
        int direction = get<1>(scramble.top());
        
        scramble.pop();
        
        my_cube.rotate_face(face, direction);
    }
}

bool init()
{
    if(SDL_Init(SDL_INIT_VIDEO))   {
        cout << "Error initializing video.\n";
        return false;
    }
    
    window = SDL_CreateWindow("Rubik's Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_size, window_size, SDL_WINDOW_OPENGL);
    
    if(window == NULL)   {
        cout << "Error creating window.\n";
        return false;
    }
    
    context = SDL_GL_CreateContext(window);
    
    if (context == NULL) {
        cout << "Error creating context.\n";
        return false;
    }
    
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    
    glFrustum(-1.f, 1.f, -1.f, 1.f, 1.5f, 15.f);
    
    glClearColor(0.f, 0.f, 0.f, 1.f);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_CULL_FACE);
    
    glMatrixMode(GL_MODELVIEW);

    return true;
}

void quit()
{
    cout << endl;
    
    SDL_DestroyWindow(window);
    
    SDL_GL_DeleteContext(context);
    
    SDL_Quit();
    
    exit(0);
}
