//
//  tables.cpp
//  Cube
//
//  Created by Ellis Sparky Hoag on 7/5/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

#include "CubeDataStructure.h"

using namespace std;
using namespace CubeDataStructure;

void Solver::generate_tables()
{
    breadth_first_search([](Cube c) -> long {return c.edge_orient;}, tables_path + "/edge_orient_table.dat", 4096, 7);
    
    breadth_first_search([](Cube c) -> long {return (c.edge_orient != 0) ? INVALID_INDEX : get_index_for_edge_perm_corner_orient(c);}, tables_path + "/edge_perm_corner_orient_table.dat", 32440320, 10, edge_perm_corner_orient_final_moves, edge_perm_corner_orient_move_restrictions);
    
    breadth_first_search([](Cube c) -> long {return (c.edge_orient != 0 || c.corner_orientation != 0) ? INVALID_INDEX : get_index_for_corner_edge_perm(c);}, tables_path + "/corner_edge_perm_table.dat", 2822400, 15, permutation_final_moves, permutation_move_restrictions);

    breadth_first_search(get_index_for_final, tables_path + "/final_table.dat", 7962624, 15, NULL, final_move_restrictions);
}

long Solver::get_index_for_edge_perm_corner_orient(const Cube & cube_state)
{
    long index = 0;
    
    int k = 4;
    
    for (int i = 1; i <= 12 && k > 0; i++)
    {
        int element = (cube_state.edge_permutation >> (i * 4)) & 0b1111;
        
        if (element == 4 || element == 5 || element == 6 || element == 7)
        {
            index += n_choose_k(12 - i, k--);
        }
    }
    
    return 495 * cube_state.corner_orientation + index;
}

long Solver::get_index_for_corner_edge_perm(const Cube & cube_state)
{
    bool edge_combination[8];
    int corner_perm[8];
    
    int j = 0;
    
    for (int i = 0; i < 12; i++)
    {
        if (i == 4) {i = 8;}
        
        int element = (cube_state.edge_permutation >> (i * 4)) & 0b1111;
        
        if (element == 0 || element == 2 || element == 8 || element == 10)
        {
            edge_combination[j] = true;
        }
        else {edge_combination[j] = false;}
        j++;
    }
    
    for (int i = 0; i < 8; i++)
    {
        corner_perm[i] = (cube_state.corner_permutation >> (i * 3)) & 0b111;
    }
    
    return 70 * permutation_to_number(corner_perm, 8) + combination_to_number(edge_combination, 8);
}

long Solver::get_index_for_final(const Cube & cube_state)
{
    const int first_corners[4] = {0, 2, 5, 7};
    const int second_corners[4] = {1, 3, 4, 6};
    const int front_edges[4] = {0, 2, 8, 10};
    const int right_edges[4] = {1, 3, 9, 11};
    
    int first_corner_perm[4];
    int second_corner_perm[4];
    int front_edge_perm[4];
    int right_edge_perm[4];
    int middle_edge_perm[4];
    
    for (int i = 0; i < 4; i++)
    {
        first_corner_perm[i] = (cube_state.corner_permutation >> (first_corners[i] * 3)) & 0b111;
        second_corner_perm[i] = (cube_state.corner_permutation >> (second_corners[i] * 3)) & 0b111;
        front_edge_perm[i] = (cube_state.edge_permutation >> (front_edges[i] * 4)) & 0b1111;
        right_edge_perm[i] = (cube_state.edge_permutation >> (right_edges[i] * 4)) & 0b1111;
        middle_edge_perm[i] = ((cube_state.edge_permutation >> ((i + 4) * 4)) & 0b1111) - 4;
        
        for (int j = 0; j < 4; j++)
        {
            if (first_corner_perm[i] == first_corners[j]) {first_corner_perm[i] = j;}
            if (second_corner_perm[i] == second_corners[j]) {second_corner_perm[i] = j;}
            if (front_edge_perm[i] == front_edges[j]) {front_edge_perm[i] = j;}
            if (right_edge_perm[i] == right_edges[j]) {right_edge_perm[i] = j;}
        }
    }
    return 331776 * permutation_to_number(second_corner_perm, 4)
         + 13824 * permutation_to_number(middle_edge_perm, 4)
         + 576 * permutation_to_number(first_corner_perm, 4)
         + 24 * permutation_to_number(front_edge_perm, 4)
         + permutation_to_number(right_edge_perm, 4);
}

long Solver::n_choose_k(int n, int k)
{
    if (n < k) {return 0;}
    if (n == k || k == 0) {return 1;}
    return n_choose_k(n - 1, k - 1) + n_choose_k(n - 1, k);
}

long Solver::permutation_to_number(int * perm, int size)
{
    if (size <= 1) {return 0;}
    
    int next_perm[size - 1];
    for (int i = 1; i < size; i++)
    {
        next_perm[i - 1] = perm[i];
        if (next_perm[i - 1] > perm[0]) {next_perm[i - 1]--;}
    }
    
    return factorial(size - 1) * perm[0] + permutation_to_number(next_perm, size - 1);
}

long Solver::combination_to_number(bool * perm, int size)
{
    long index = 0;
    int num_ones = 0;
    
    for (int i = size; i > 0; i--)
    {
        if (perm[i - 1])
        {
            index += n_choose_k(size - i, ++num_ones);
        }
    }
    
    //cout << "{" << perm[0];
    //for (int i = 1; i < size; i++) {cout << ", " << perm[i];}
    //cout << "} = " << index << endl;

    return index;
}

void Solver::breadth_first_search(function<long (const Cube &)> get_index, const string path, const long buffer_size, int max_depth, bool (*final_moves)(int, int), bool (*move_restrictions)(int, int, int))
{
    chrono::time_point<chrono::system_clock> start_time = chrono::system_clock::now();
    
    uint8_t * buffer = new uint8_t[buffer_size];
    for (long i = 0; i < buffer_size; i++) {buffer[i] = INVALID_STATE;}
    
    Cube my_cube;
    
    buffer[get_index(my_cube)] = 0;
    
    queue<tuple<Cube, int, int>> cube_state_queue;
    cube_state_queue.push(make_tuple(my_cube, 0, -1));
    
    while (!cube_state_queue.empty())
    {
        tuple<Cube, int, int> current_cube = cube_state_queue.front();
        cube_state_queue.pop();
        
        Cube current_state = get<0>(current_cube);
        int depth = get<1>(current_cube) + 1;
        int prev_face = get<2>(current_cube);
        
        for (int face = 0; face < 6; face++)
        {
            for (int direction = 1; direction <= 3; direction++)
            {
                if ((depth == 1 && final_moves != NULL && !final_moves(face, direction))
                    || move_restrictions(face, prev_face, direction)) {continue;}
                
                Cube next_cube = current_state;
                
                next_cube.rotate_face(face, direction);
                
                long index = get_index(next_cube);
                if (index != INVALID_INDEX)
                {
                    if (buffer[index] == INVALID_STATE)
                    {
                        buffer[index] = depth;
                    }
                    else {continue;}
                }
                if (depth + 1 > max_depth) {continue;}
                
                cube_state_queue.push(make_tuple(next_cube, depth, face));
            }
        }
    }
    
    FILE * my_file = fopen(path.c_str(), "wb");
    fwrite(buffer, 1, buffer_size, my_file);
    fclose(my_file);
    
    delete[] buffer;
    
    cout << "Generated table in " << (chrono::system_clock::now() - start_time).count() / (float)CLOCKS_PER_SEC << " seconds.\n";
}
