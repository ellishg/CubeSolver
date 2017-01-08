//
//  CubeDataStructure.h
//  Cube
//
//  Created by Ellis Sparky Hoag on 7/2/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

#ifndef CubeDataStructure_h
#define CubeDataStructure_h

#include <iostream>
#include <queue>
#include <stack>
#include <tuple>
#include <functional>

#include "definitions.h"

namespace CubeDataStructure
{
    struct Cube;
    class Solver;
    
    struct Cube
    {
    public:
        uint64_t edge_permutation;

        uint32_t corner_permutation;
        
        uint16_t corner_orientation;
        
        uint16_t edge_orient;
        
    public:
        
        Cube() : corner_permutation(16434824), corner_orientation(0), edge_permutation(205163983024656), edge_orient(0) {}
        
        void rotate_face(int face, int direction);
        
        void get_facelet_level(uint32_t (&facelet_level)[6]);
        
        friend std::ostream & operator<<(std::ostream & out, const Cube & c)
        {            
            std::cout << "Corner Permutation: (";
            
            unsigned int i;
            
            for (i = 0; i < 8; i++)
            {
                std::cout << ((c.corner_permutation >> (i * 3)) & 0b111) + 1;
                if (i < 7)
                {
                    std::cout << ", ";
                }
            }
            
            std::cout << ")\nCorner Orientation: (";
            
            for (i = 0; i < 8; i++)
            {
                std::cout << ((c.corner_orientation >> (i * 2)) & 0b11);
                if (i < 7)
                {
                    std::cout << ", ";
                }
            }
            
            std::cout << ")\nEdge Permutation: (";
            
            for (i = 0; i < 12; i++)
            {
                std::cout << ((c.edge_permutation >> (i * 4)) & 0b1111) + 1;
                if (i < 11)
                {
                    std::cout << ", ";
                }
            }
            
            std::cout <<")\nEdge Orientation: (";
            
            for (i = 0; i < 12; i++)
            {
                std::cout << ((c.edge_orient >> i) & 0b1);
                if (i < 11)
                {
                    std::cout << ", ";
                }
            }
            
            return out << ")\n";
        }
    };
    
    class Solver
    {
    public:
        
        Solver(const std::string _tables_path) : tables_path(_tables_path) {}

        std::stack<std::tuple<int, int>> get_solution(Cube cube_state);
        
        std::stack<std::tuple<int, int>> simplify_solution(std::stack<std::tuple<int, int>> solution);
        
        std::stack<std::tuple<int, int>> get_scramble(int length);
        
        void print_move_list(std::stack<std::tuple<int, int>> move_list);
        void print_move_list(std::queue<std::tuple<int, int>> move_list);

        void generate_tables();

    private:
        
        void accomplish_goal(Cube & cube_state, std::queue<std::tuple<int, int>> & solution, bool (*final_moves)(int, int), std::function<long (const Cube &)> get_index, FILE * table_file, bool (*move_restrictions)(int, int, int) = standard_move_restrictions);

        bool attempt_accomplish_goal(Cube & cube_state, int max_depth, std::queue<std::tuple<int, int>> & solution, bool (*goal)(const Cube &));

        bool depth_first_accomplish_goal(Cube & cube_state, int max_depth, std::queue<std::tuple<int, int>> & solution, bool (*final_moves)(int, int), std::function<long (const Cube &)> get_index, FILE * table_file, bool (*move_restrictions)(int, int, int), bool (*goal)(const Cube &) = NULL, int prev_face = -1);
        
        void breadth_first_search(std::function<long (const Cube &)> get_index, const std::string path, const long buffer_size, int max_depth, bool (*final_moves)(int, int) = NULL, bool (*move_restrictions)(int, int, int) = standard_move_restrictions);
        
        static bool is_solved(const Cube & cube_state);
        
        static bool standard_move_restrictions(int cur_face, int prev_face, int = 0);
        
        static bool edge_perm_corner_orient_move_restrictions(int cur_face, int prev_face, int direction);
        
        static bool permutation_move_restrictions(int cur_face, int prev_face, int direction);
        
        static bool final_move_restrictions(int cur_face, int prev_face, int direction);
        
        static bool edge_orient_final_moves(int cur_face, int direction);
        
        static bool edge_perm_corner_orient_final_moves(int cur_face, int direction);
        
        static bool permutation_final_moves(int cur_face, int direction);
        
        static int move_bound(long index, FILE * table_file);
        
        static long get_index_for_edge_perm_corner_orient(const Cube & cube_state);
        
        static long n_choose_k(int n, int k);
        
        static long factorial(int n) {return n > 1 ? n * factorial(n - 1) : 1;}
        
        static long get_index_for_final(const Cube & cube_state);
        
        static long permutation_to_number(int * perm, int size);
        
        static long combination_to_number(bool * perm, int size);
        
        static long get_index_for_corner_edge_perm(const Cube & cube_state);
        
        const std::string tables_path;

        const std::string edge_orient_table_path = "/edge_orient_table.dat";
        const std::string edge_perm_corner_orient_table_path = "/edge_perm_corner_orient_table.dat";
        const std::string corner_edge_perm_table_path = "/corner_edge_perm_table.dat";
        const std::string final_table_path = "/final_table.dat";
    };
}

#endif /* CubeDataStructure_h */
