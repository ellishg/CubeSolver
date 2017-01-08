//
//  CubeDataStructure.cpp
//  Cube
//
//  Created by Ellis Sparky Hoag on 7/2/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

/*
 Generated solutions for 100000 cubes.
 
 <U, D, L, R, F2, B2> Average: 4.09053e-05 Max: 0.000398
 <U, D, L2, R2, F2, B2> Average: 0.000203383 Max: 0.000842
 <U2, D2, L2, R2, F2, B2> Average: 0.00013095 Max: 0.00055
 <> Average: 8.78874e-05 Max: 0.000491
 Full solution: Average: 0.000465136 Max: 0.001557
 */

#include "CubeDataStructure.h"

using namespace std;
using namespace CubeDataStructure;

stack<tuple<int, int>> Solver::get_solution(Cube cube_state)
{
    clock_t t = clock();
    clock_t total_time = clock();
    
    stack<tuple<int, int>> solution;

    queue<tuple<int, int>> edge_orient_solution, edge_perm_corner_orient_solution, permutation_solution, final_goal_solution;

    if (attempt_accomplish_goal(cube_state, 5, final_goal_solution, is_solved))
    {
        while (!final_goal_solution.empty()) {solution.push(final_goal_solution.front()); final_goal_solution.pop();}
        return solution;
    }
    
    FILE * edge_orient_table_file = fopen((tables_path + edge_orient_table_path).c_str(), "rb");
    FILE * edge_perm_corner_orient_table_file = fopen((tables_path + edge_perm_corner_orient_table_path).c_str(), "rb");
    FILE * corner_edge_perm_table_file = fopen((tables_path + corner_edge_perm_table_path).c_str(), "rb");
    FILE * final_table_file = fopen((tables_path + final_table_path).c_str(), "rb");
    
    accomplish_goal(cube_state, edge_orient_solution, edge_orient_final_moves, [](Cube c) -> long {return c.edge_orient;}, edge_orient_table_file);
    
    print_move_list(edge_orient_solution);
    cout << "<U, D, L, R, F2, B2> " << edge_orient_solution.size() << " moves in " << (clock() - t) / (float)CLOCKS_PER_SEC << " seconds.\n";
    t = clock();
    
    accomplish_goal(cube_state, edge_perm_corner_orient_solution, edge_perm_corner_orient_final_moves, get_index_for_edge_perm_corner_orient, edge_perm_corner_orient_table_file, edge_perm_corner_orient_move_restrictions);
    
    print_move_list(edge_perm_corner_orient_solution);
    cout << "<U, D, L2, R2, F2, B2> " << edge_perm_corner_orient_solution.size() << " moves in " << (clock() - t) / (float)CLOCKS_PER_SEC << " seconds.\n";
    t = clock();
    
    accomplish_goal(cube_state, permutation_solution, permutation_final_moves, get_index_for_corner_edge_perm, corner_edge_perm_table_file, permutation_move_restrictions);

    print_move_list(permutation_solution);
    cout << "<U2, D2, L2, R2, F2, B2> " << permutation_solution.size() << " moves in " << (clock() - t) / (float)CLOCKS_PER_SEC << " seconds.\n";
    t = clock();

    accomplish_goal(cube_state, final_goal_solution, NULL, get_index_for_final, final_table_file, final_move_restrictions);
    
    fclose(edge_orient_table_file);
    fclose(edge_perm_corner_orient_table_file);
    fclose(corner_edge_perm_table_file);
    fclose(final_table_file);
    
    print_move_list(final_goal_solution);
    unsigned long final_solution_size = final_goal_solution.size();

    while (!final_goal_solution.empty()) {solution.push(final_goal_solution.front()); final_goal_solution.pop();}
    while (!permutation_solution.empty()) {solution.push(permutation_solution.front()); permutation_solution.pop();}
    while (!edge_perm_corner_orient_solution.empty()) {solution.push(edge_perm_corner_orient_solution.front()); edge_perm_corner_orient_solution.pop();}
    while (!edge_orient_solution.empty()) {solution.push(edge_orient_solution.front()); edge_orient_solution.pop();}
    
    cout << "<> " << final_solution_size << " moves in " << (clock() - t) / (float)CLOCKS_PER_SEC << " seconds.\n";
    cout << solution.size() << " moves in " << (clock() - total_time) / (float)CLOCKS_PER_SEC << " seconds.\n";
    
    return simplify_solution(solution);
}

stack<tuple<int, int>> Solver::simplify_solution(stack<tuple<int, int>> solution)
{
    if (solution.size() < 2) {return solution;}
    
    tuple<int, int> top = solution.top();
    solution.pop();
    tuple<int, int> next = solution.top();
    solution.pop();
    
    int face = get<0>(top);
    
    if (face == get<0>(next))
    {
        int direction = (get<1>(top) + get<1>(next)) % 4;
        
        if (direction == 0)
        {
            //cout << "Removed two moves!\n";
            
            return simplify_solution(solution);
        }
        else
        {
            //cout << "Removed one move!\n";
            
            solution.push(make_tuple(face, direction));
            
            return simplify_solution(solution);
        }
    }
    
    solution.push(next);

    stack<tuple<int, int>> simplified_solution = simplify_solution(solution);

    simplified_solution.push(top);
    
    return simplified_solution;
}

stack<tuple<int, int>> Solver::get_scramble(int length)
{
    stack<tuple<int, int>> scramble;
    
    for (int i = 0; i < length; i++)
    {
        scramble.push(make_tuple(rand() % 6, (rand() % 3) + 1));
    }
    
    return simplify_solution(scramble);
}

void Solver::print_move_list(stack<tuple<int, int>> move_list)
{
    cout << "{ ";
    while (!move_list.empty())
    {
        int face = get<0>(move_list.top());
        int direction = get<1>(move_list.top());
        cout << face_to_char[6 * (direction - 1) + face];
        move_list.pop();
    }
    cout << "}\n";
}

void Solver::print_move_list(queue<tuple<int, int>> move_list)
{
    stack<tuple<int, int>> stack;
    while (!move_list.empty())
    {
        stack.push(move_list.front());
        move_list.pop();
    }
    print_move_list(stack);
}

void Solver::accomplish_goal(Cube & cube_state, queue<tuple<int, int>> & solution, bool (*final_moves)(int, int), function<long (const Cube &)> get_index, FILE * table_file, bool (*move_restrictions)(int, int, int))
{
    depth_first_accomplish_goal(cube_state, move_bound(get_index(cube_state), table_file), solution, final_moves, get_index, table_file, move_restrictions);
}

bool Solver::attempt_accomplish_goal(Cube & cube_state, int max_depth, queue<tuple<int, int>> & solution, bool (*goal)(const Cube &))
{
    for (int i = 0; i < max_depth; i++)
    {
        if (depth_first_accomplish_goal(cube_state, i, solution, NULL, NULL, NULL, standard_move_restrictions, goal))
        {
            return true;
        }
    }
    return false;
}

bool Solver::depth_first_accomplish_goal(Cube & cube_state, int max_depth, queue<tuple<int, int>> & solution, bool (*final_moves)(int, int), function<long (const Cube &)> get_index, FILE * table_file, bool (*move_restrictions)(int, int, int), bool (*goal)(const Cube &), int prev_face)
{
    if (max_depth == 0)
    {
        if (NULL != table_file)
        {
            return move_bound(get_index(cube_state), table_file) == 0;
        }
        else {return goal(cube_state);}
    }
    
    if (NULL != table_file && move_bound(get_index(cube_state), table_file) > max_depth) {return false;}
    
    for (int face = 0; face < 6; face++)
    {
        for (int direction = 1; direction <= 3; direction++)
        {
            if ((max_depth == 1 && final_moves != NULL && !final_moves(face, direction))
                || move_restrictions(face, prev_face, direction)) {continue;}
            
            Cube next_state = cube_state;
            
            next_state.rotate_face(face, direction);
            
            if (depth_first_accomplish_goal(next_state, max_depth - 1, solution, final_moves, get_index, table_file, move_restrictions, goal, face))
            {
                cube_state = next_state;
                solution.push(make_tuple(face, direction));
                return true;
            }
        }
    }
    return false;
}

bool Solver::standard_move_restrictions(int cur_face, int prev_face, int)
{
    /*
     *  returns true if move needs to be skipped
     *  Move ordering of opposite faces.
     *  Front then back
     *  Right then left
     *  Up then down
     */
    
    return (prev_face == FRONT && cur_face == BACK)
        || (prev_face == RIGHT && cur_face == LEFT)
        || (prev_face == UP && cur_face == DOWN)
        || cur_face == prev_face;
}

bool Solver::edge_perm_corner_orient_move_restrictions(int cur_face, int prev_face, int direction)
{
    return standard_move_restrictions(cur_face, prev_face)
        || (direction != HALF_TURN && (cur_face == FRONT || cur_face == BACK));
}

bool Solver::permutation_move_restrictions(int cur_face, int prev_face, int direction)
{
    return edge_perm_corner_orient_move_restrictions(cur_face, prev_face, direction)
        || (direction != HALF_TURN && (cur_face == LEFT || cur_face == RIGHT));
}

bool Solver::final_move_restrictions(int cur_face, int prev_face, int direction)
{
    return direction != HALF_TURN || standard_move_restrictions(cur_face, prev_face);
}

bool Solver::edge_orient_final_moves(int cur_face, int direction)
{
    return direction != HALF_TURN && (cur_face == FRONT || cur_face == BACK);
}

bool Solver::edge_perm_corner_orient_final_moves(int cur_face, int direction)
{
    return direction != HALF_TURN && (cur_face == RIGHT || cur_face == LEFT);
}

bool Solver::permutation_final_moves(int cur_face, int direction)
{
    return direction != HALF_TURN && (cur_face == UP || cur_face == DOWN);
}

int Solver::move_bound(long index, FILE * table_file)
{
    if (table_file == NULL) {return 0;}
    
    uint8_t bound;
    
    fseek(table_file, index, SEEK_SET);
    
    fread((void *)&bound, 1, 1, table_file);
    
    return bound;
}

bool Solver::is_solved(const Cube & cube_state)
{
    return cube_state.corner_permutation == 16434824
        && cube_state.corner_orientation == 0
        && cube_state.edge_permutation == 205163983024656
        && cube_state.edge_orient == 0;
}

void Cube::rotate_face(int face, int direction)
{
    unsigned int * move = face_moves[face];
    
    for (int n = 0; n < direction; n++)
    {
        unsigned int i;
        uint64_t new_ep = 0;
        uint32_t new_cp = 0;
        uint16_t new_co = 0;
        uint16_t new_eo = 0;
        
        for (i = 0; i < 8; i++)
        {
            new_cp |= ((corner_permutation >> move[i] * 3) & 0b111) << (i * 3);
            
            unsigned int orientation = ((corner_orientation >> move[i] * 2) & 0b11);
            
            orientation = (orientation + move[8 + i]) % 3;
            
            new_co |= orientation << (i * 2);
        }
        for (i = 0; i < 12; i++)
        {
            new_ep |= ((edge_permutation >> (move[16 + i] * 4)) & 0b1111) << (i * 4);

            unsigned int orientation = (edge_orient >> move[16 + i]) & 0b1;
           
            orientation = (orientation + move[28 + i]) % 2;
            
            new_eo |= orientation << i;
        }
        
        corner_permutation = new_cp;
        corner_orientation = new_co;
        edge_permutation = new_ep;
        edge_orient = new_eo;
    }
}

void Cube::get_facelet_level(uint32_t (&facelet_level)[6])
{
    for (int i = 0; i < 6; i++) {facelet_level[i] = 0;}
    
    facelet_level[FRONT] |= RED << (4 * 3);
    facelet_level[RIGHT] |= BLUE << (4 * 3);
    facelet_level[BACK] |= ORANGE << (4 * 3);
    facelet_level[LEFT] |= GREEN << (4 * 3);
    facelet_level[UP] |= WHITE << (4 * 3);
    facelet_level[DOWN] |= YELLOW << (4 * 3);
    
    int i = 0;
    
    for (int cubie = 0; cubie < 8; cubie++)
    {
        int corner_index = (corner_permutation >> (cubie * 3)) & 0b111;
        int orientation = (corner_orientation >> (cubie * 2)) & 0b11;
        
        int up_down_color_index = 3 * corner_index + orientation;
        int up_down_color = corner_cubie_to_facelet_color[up_down_color_index];
        int up_down_face_index = facelet_index[i++];
        int up_down_facelet_index = facelet_index[i++];

        int right_left_color_index = 3 * corner_index + ((orientation + ((cubie < 4) ^ (cubie % 2) ? 1 : 2)) % 3);
        int right_left_color = corner_cubie_to_facelet_color[right_left_color_index];
        int right_left_face_index = facelet_index[i++];
        int right_left_facelet_index = facelet_index[i++];
        
        int front_back_color_index = 3 * corner_index + ((orientation + ((cubie < 4) ^ (cubie % 2) ? 2 : 1)) % 3);
        int front_back_color = corner_cubie_to_facelet_color[front_back_color_index];
        int front_back_face_index = facelet_index[i++];
        int front_back_facelet_index = facelet_index[i++];
        
        facelet_level[up_down_face_index] |= up_down_color << (up_down_facelet_index * 3);
        facelet_level[right_left_face_index] |= right_left_color << (right_left_facelet_index * 3);
        facelet_level[front_back_face_index] |= front_back_color << (front_back_facelet_index * 3);
    }
    
    for (int cubie = 0; cubie < 12; cubie++)
    {
        int edge_index = (edge_permutation >> (cubie * 4)) & 0b1111;
        int orientation = (edge_orient >> cubie) & 0b1;
        
        int up_down_color_index = 2 * edge_index + orientation;
        int up_down_color = edge_cubie_to_facelet_color[up_down_color_index];
        int up_down_face_index = facelet_index[i++];
        int up_down_facelet_index = facelet_index[i++];
                
        int right_left_color_index = 2 * edge_index + (orientation ^ 0b1);
        int right_left_color = edge_cubie_to_facelet_color[right_left_color_index];
        int right_left_face_index = facelet_index[i++];
        int right_left_facelet_index = facelet_index[i++];
        
        int front_back_color_index = 2 * edge_index + (up_down_face_index == -1 ? orientation : orientation ^ 0b1);
        int front_back_color = edge_cubie_to_facelet_color[front_back_color_index];
        int front_back_face_index = facelet_index[i++];
        int front_back_facelet_index = facelet_index[i++];
        
        if (up_down_face_index != -1)
        {
            facelet_level[up_down_face_index] |= up_down_color << (up_down_facelet_index * 3);
        }
        if (right_left_face_index != -1)
        {
            facelet_level[right_left_face_index] |= right_left_color << (right_left_facelet_index * 3);
        }
        if (front_back_face_index != -1)
        {
            facelet_level[front_back_face_index] |= front_back_color << (front_back_facelet_index * 3);
        }
    }
}