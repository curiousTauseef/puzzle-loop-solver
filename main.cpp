#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>

#include "puzzle.h"

using namespace std;

bool heuristic(puzzle &p);

void ban_edge_around_zero(puzzle &p);
void ban_edge_around_one(puzzle &p);
void ban_edge_around_two(puzzle &p);
void ban_edge_around_three(puzzle &p);
void ban_edge_around_point(puzzle &p);
void ban_point(puzzle &p);

void link_around_one(puzzle &p);
void link_around_two(puzzle &p);
void link_around_three(puzzle &p);
void link_around_point(puzzle &p);

void DFS(puzzle &p);
void go_vrt(puzzle p,
            const int &start_r, const int &start_c,
            const int &v_r, const int &v_c,
            const int &dst_p_r, const int &dst_p_c);
void go_hrz(puzzle p, const int &start_r, const int &start_c,
            const int &h_r, const int &h_c,
            const int &dst_p_r, const int &dst_p_c);
void go_next(puzzle &p,
             const int &start_r, const int &start_c,
             const int &dst_p_r, const int &dst_p_c);
void vrt_go_next(puzzle &p,
                 const int &start_r, const int &start_c,
                 const int &v_r, const int &v_c,
                 const int &dst_p_r, const int &dst_p_c);
void hrz_go_next(puzzle &p,
                 const int &start_r, const int &start_c,
                 const int &h_r, const int &h_c,
                 const int &dst_p_r, const int &dst_p_c);

unordered_set<string> puzzle_results;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "No puzzle file\n";
        return -1;
    }

    puzzle p;
    // read puzzle file
    ifstream puzzle_file(argv[1]);
    // read cols rows
    puzzle_file >> p.cols >> p.rows;
    // read puzzle
    for (size_t row = 0; row < p.rows; row++)
    {
        string line;
        puzzle_file >> line;
        vector<int> tmp_row;
        for (size_t col = 0; col < p.cols; col++)
        {
            const char c = line[col];
            switch (c)
            {
            case '0':
            case '1':
            case '2':
            case '3':
                tmp_row.push_back(c - '0');
                break;
            default:
                tmp_row.push_back(-1);
                break;
            }
        }
        p.lat.push_back(tmp_row);
    }
    // init connect state table
    for (size_t row = 0; row < p.rows + 1; row++)
    {
        vector<puzzle::edge_state> tmp_row;
        for (size_t col = 0; col < p.cols; col++)
        {
            tmp_row.push_back(puzzle::NOT);
        }
        p.hrz.push_back(tmp_row);
    }
    for (size_t row = 0; row < p.rows; row++)
    {
        vector<puzzle::edge_state> tmp_row;
        for (size_t col = 0; col < p.cols + 1; col++)
        {
            tmp_row.push_back(puzzle::NOT);
        }
        p.vrt.push_back(tmp_row);
    }
    // init banned point
    for (size_t row = 0; row <= p.rows; row++)
    {
        vector<bool> tmp_row;
        for (size_t col = 0; col <= p.cols; col++)
        {
            tmp_row.push_back(false);
        }
        p.banned_point.push_back(tmp_row);
    }
    // solve puzzle
    ban_edge_around_zero(p);
    heuristic(p);
    DFS(p);
    cout << "Solutions: " << puzzle_results.size() << endl;
    return 0;
}

bool heuristic(puzzle &p)
{
    while (true)
    {
        const string last_step = p.to_string();
        ban_edge_around_one(p);
        ban_edge_around_two(p);
        ban_edge_around_three(p);
        ban_edge_around_point(p);
        ban_point(p);
        link_around_point(p);
        link_around_three(p);
        link_around_two(p);
        link_around_one(p);
        const string curr_step = p.to_string();
        if (!p.is_correct())
            return false;
        if (last_step.compare(curr_step) == 0)
        {
            cout << curr_step;
            return true;
        }
    }
}

void ban_edge_around_zero(puzzle &p)
{
    /**
     * . b .
     * b 0 b
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 0)
            {
                p.hrz[row][col] = puzzle::BAN;
                p.hrz[row + 1][col] = puzzle::BAN;
                p.vrt[row][col] = puzzle::BAN;
                p.vrt[row][col + 1] = puzzle::BAN;
            }
        }
    }
}

void ban_edge_around_one(puzzle &p)
{
    /**
     *   x
     * x . b .
     *   b 1
     *   .   .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 1)
            {
                if (!p.point_can_up(row, col) &&
                    !p.point_can_left(row, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                if (!p.point_can_up(row, col + 1) &&
                    !p.point_can_right(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                if (!p.point_can_down(row + 1, col) &&
                    !p.point_can_left(row + 1, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                if (!p.point_can_down(row + 1, col + 1) &&
                    !p.point_can_right(row + 1, col + 1))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
            }
        }
    }
    /**
     * . b .
     * b 1 |
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 1)
            {
                if (p.hrz_has_edge(row, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row + 1, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
            }
        }
    }
}

void ban_edge_around_two(puzzle &p)
{
    /**
     * . - .
     * b 2 |
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 2)
            {
                if (p.hrz_has_edge(row, col) &&
                    p.vrt_has_edge(row, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row, col) &&
                         p.hrz_has_edge(row + 1, col))
                {
                    p.vrt[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row + 1, col) &&
                         p.vrt_has_edge(row, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row + 1, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.hrz[row + 1][col] = puzzle::BAN;
                }
            }
        }
    }
}

void ban_edge_around_three(puzzle &p)
{
    /**
     * . - .
     * | 3 |
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3)
            {
                if (p.hrz_has_edge(row, col) &&
                    p.hrz_has_edge(row + 1, col) &&
                    p.vrt_has_edge(row, col))
                {
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row, col) &&
                         p.hrz_has_edge(row + 1, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.vrt[row][col] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1) &&
                         p.hrz_has_edge(row, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1) &&
                         p.hrz_has_edge(row + 1, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                }
            }
        }
    }
}

void ban_edge_around_point(puzzle &p)
{
    /**
     *   x
     * x . x
     *   b
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (!p.point_can_up(row, col) &&
                !p.point_can_down(row, col) &&
                !p.point_can_left(row, col) &&
                p.point_can_right(row, col))
            {
                p.hrz[row][col] = puzzle::BAN;
            }
            else if (!p.point_can_up(row, col) &&
                     !p.point_can_down(row, col) &&
                     p.point_can_left(row, col) &&
                     !p.point_can_right(row, col))
            {
                p.hrz[row][col - 1] = puzzle::BAN;
            }
            else if (!p.point_can_up(row, col) &&
                     p.point_can_down(row, col) &&
                     !p.point_can_left(row, col) &&
                     !p.point_can_right(row, col))
            {
                p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_can_up(row, col) &&
                     !p.point_can_down(row, col) &&
                     !p.point_can_left(row, col) &&
                     !p.point_can_right(row, col))
            {
                p.vrt[row - 1][col] = puzzle::BAN;
            }
        }
    }
    /**
     *   b
     * - . b
     *   |
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (p.point_has_edge_up(row, col) &&
                p.point_has_edge_down(row, col))
            {
                if (p.point_can_left(row, col))
                    p.hrz[row][col - 1] = puzzle::BAN;
                if (p.point_can_right(row, col))
                    p.hrz[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_left(row, col) &&
                     p.point_has_edge_right(row, col))
            {
                if (p.point_can_up(row, col))
                    p.vrt[row - 1][col] = puzzle::BAN;
                if (p.point_can_down(row, col))
                    p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_up(row, col) &&
                     p.point_has_edge_left(row, col))
            {
                if (p.point_can_right(row, col))
                    p.hrz[row][col] = puzzle::BAN;
                if (p.point_can_down(row, col))
                    p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_up(row, col) &&
                     p.point_has_edge_right(row, col))
            {
                if (p.point_can_left(row, col))
                    p.hrz[row][col - 1] = puzzle::BAN;
                if (p.point_can_down(row, col))
                    p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_down(row, col) &&
                     p.point_has_edge_left(row, col))
            {
                if (p.point_can_up(row, col))
                    p.vrt[row - 1][col] = puzzle::BAN;
                if (p.point_can_right(row, col))
                    p.hrz[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_down(row, col) &&
                     p.point_has_edge_right(row, col))
            {
                if (p.point_can_up(row, col))
                    p.vrt[row - 1][col] = puzzle::BAN;
                if (p.point_can_left(row, col))
                    p.hrz[row][col - 1] = puzzle::BAN;
            }
        }
    }
}

void ban_point(puzzle &p)
{
    /**
     *   x
     * x b x
     *   x
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (!p.point_can_up(row, col) &&
                !p.point_can_down(row, col) &&
                !p.point_can_left(row, col) &&
                !p.point_can_right(row, col))
            {
                p.banned_point[row][col] = true;
            }
        }
    }
}

void link_around_one(puzzle &p)
{
    /**
     * . x .
     * x 1 x
     * . l .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 1 &&
                !p.complete_lat(row, col) &&
                p.get_lat_banned_edge(row, col) == 3)
            {
                if (p.hrz[row][col] != puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] != puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                }
                else if (p.vrt[row][col] != puzzle::BAN)
                {
                    p.vrt[row][col] = puzzle::LINKED;
                }
                else if (p.vrt[row][col + 1] != puzzle::BAN)
                {
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
            }
        }
    }
}

void link_around_two(puzzle &p)
{
    /**
     * . l .
     * x 2 x
     * . l .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 2 &&
                !p.complete_lat(row, col) &&
                p.get_lat_banned_edge(row, col) == 2)
            {
                if (p.hrz[row][col] == puzzle::BAN &&
                    p.hrz[row + 1][col] == puzzle::BAN)
                {
                    p.vrt[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.vrt[row][col] == puzzle::BAN &&
                         p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.hrz[row + 1][col] = puzzle::LINKED;
                }
                else if (p.hrz[row][col] == puzzle::BAN &&
                         p.vrt[row][col] == puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.hrz[row][col] == puzzle::BAN &&
                         p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] == puzzle::BAN &&
                         p.vrt[row][col] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] == puzzle::BAN &&
                         p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
            }
        }
    }
}

void link_around_three(puzzle &p)
{
    /**
     * . l .
     * x 3 l
     * . l .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3 &&
                !p.complete_lat(row, col) &&
                p.get_lat_banned_edge(row, col) == 1)
            {
                if (p.hrz[row][col] == puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.vrt[row][col] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
            }
        }
    }
    /**
     *   x
     * x . l .
     *   l 3
     *   .   .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3)
            {
                if (!p.point_can_up(row, col) &&
                    !p.point_can_left(row, col))
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                if (!p.point_can_up(row, col + 1) &&
                    !p.point_can_right(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                if (!p.point_can_down(row + 1, col) &&
                    !p.point_can_left(row + 1, col))
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                if (!p.point_can_down(row + 1, col + 1) &&
                    !p.point_can_right(row + 1, col + 1))
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
            }
        }
    }
}

void link_around_point(puzzle &p)
{
    /**
     *   x
     * l . x
     *   |
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (p.get_conn(row, col) == 1)
            {
                if (p.point_can_up(row, col) &&
                    p.point_can_down(row, col) &&
                    !p.point_can_left(row, col) &&
                    !p.point_can_right(row, col))
                {
                    p.vrt[row - 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                else if (p.point_can_up(row, col) &&
                         !p.point_can_down(row, col) &&
                         p.point_can_left(row, col) &&
                         !p.point_can_right(row, col))
                {
                    p.vrt[row - 1][col] = puzzle::LINKED;
                    p.hrz[row][col - 1] = puzzle::LINKED;
                }
                else if (p.point_can_up(row, col) &&
                         !p.point_can_down(row, col) &&
                         !p.point_can_left(row, col) &&
                         p.point_can_right(row, col))
                {
                    p.vrt[row - 1][col] = puzzle::LINKED;
                    p.hrz[row][col] = puzzle::LINKED;
                }
                else if (!p.point_can_up(row, col) &&
                         p.point_can_down(row, col) &&
                         p.point_can_left(row, col) &&
                         !p.point_can_right(row, col))
                {
                    p.vrt[row][col] = puzzle::LINKED;
                    p.hrz[row][col - 1] = puzzle::LINKED;
                }
                else if (!p.point_can_up(row, col) &&
                         p.point_can_down(row, col) &&
                         !p.point_can_left(row, col) &&
                         p.point_can_right(row, col))
                {
                    p.vrt[row][col] = puzzle::LINKED;
                    p.hrz[row][col] = puzzle::LINKED;
                }
                else if (!p.point_can_up(row, col) &&
                         !p.point_can_down(row, col) &&
                         p.point_can_left(row, col) &&
                         p.point_can_right(row, col))
                {
                    p.hrz[row][col - 1] = puzzle::LINKED;
                    p.hrz[row][col] = puzzle::LINKED;
                }
            }
        }
    }
}

void DFS(puzzle &p)
{
    // Start with one line
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 1; col <= p.cols; col++)
        {
            if (!p.banned_point[row][col] && p.get_conn(row, col) == 1)
            {
                go_next(p, row, col, row, col);
                return;
            }
        }
    }
    // No line on this map
    // For every points find all solutions
    for (size_t row = 0; row <= p.rows; row++)
    {
        // Optimized
        for (size_t col = 1; col <= p.cols; col += 2)
        {
            go_next(p, row, col, row, col);
            // set point banned
            p.banned_point[row][col - 1] = true;
            p.banned_point[row][col] = true;
            if (col + 1 == p.cols)
                p.banned_point[row][col + 1] = true;
        }
    }
}

void go_vrt(puzzle p,
            const int &start_r, const int &start_c,
            const int &v_r, const int &v_c,
            const int &dst_p_r, const int &dst_p_c)
{
    // Draw line
    p.vrt[v_r][v_c] = puzzle::LINKED;
    // Solved
    if (dst_p_r == start_r && dst_p_c == start_c)
    {
        // Do final check
        if (p.is_fin())
        {
            // Print solution
            const auto result = p.to_string();
            if (puzzle_results.find(result) == puzzle_results.end())
            {
                puzzle_results.insert(result);
                cout << p.to_string();
            }
        }
        return;
    }
    // Check connectivity
    if (p.get_conn(dst_p_r, dst_p_c) > 2)
        return;
    // Check lattice
    if (!p.vrt_sat(v_r, v_c))
        return;
    // Do heuristic
    if (heuristic(p) == false)
        return;
    // Go next point
    vrt_go_next(p, start_r, start_c, v_r, v_c, dst_p_r, dst_p_c);
}

void go_hrz(puzzle p, const int &start_r, const int &start_c,
            const int &h_r, const int &h_c,
            const int &dst_p_r, const int &dst_p_c)
{
    // Draw line
    p.hrz[h_r][h_c] = puzzle::LINKED;
    // Solved
    if (dst_p_r == start_r && dst_p_c == start_c)
    {
        // Do final check
        if (p.is_fin())
        {
            // Print solution
            const auto result = p.to_string();
            if (puzzle_results.find(result) == puzzle_results.end())
            {
                puzzle_results.insert(result);
                cout << p.to_string();
            }
        }
        return;
    }
    // Check connectivity
    if (p.get_conn(dst_p_r, dst_p_c) > 2)
        return;
    // Check lattice
    if (!p.hrz_sat(h_r, h_c))
        return;
    // Do heuristic
    if (heuristic(p) == false)
        return;
    // Go next point
    hrz_go_next(p, start_r, start_c, h_r, h_c, dst_p_r, dst_p_c);
}

void go_next(puzzle &p,
             const int &start_r, const int &start_c,
             const int &dst_p_r, const int &dst_p_c)
{
    if (p.point_can_up(dst_p_r, dst_p_c))
    {
        go_vrt(p,
               start_r, start_c,
               dst_p_r - 1, dst_p_c,
               dst_p_r - 1, dst_p_c);
    }
    if (p.point_can_down(dst_p_r, dst_p_c))
    {
        go_vrt(p,
               start_r, start_c,
               dst_p_r, dst_p_c,
               dst_p_r + 1, dst_p_c);
    }
    if (p.point_can_left(dst_p_r, dst_p_c))
    {
        go_hrz(p,
               start_r, start_c,
               dst_p_r, dst_p_c - 1,
               dst_p_r, dst_p_c - 1);
    }
    if (p.point_can_right(dst_p_r, dst_p_c))
    {
        go_hrz(p,
               start_r, start_c,
               dst_p_r, dst_p_c,
               dst_p_r, dst_p_c + 1);
    }
}

void vrt_go_next(puzzle &p,
                 const int &start_r, const int &start_c,
                 const int &v_r, const int &v_c,
                 const int &dst_p_r, const int &dst_p_c)
{
    if (p.get_conn(dst_p_r, dst_p_c) == 2)
    {
        // Solved
        if (dst_p_r == start_r && dst_p_c == start_c)
        {
            // Do final check
            if (p.is_fin())
            {
                // Print solution
                const auto result = p.to_string();
                if (puzzle_results.find(result) == puzzle_results.end())
                {
                    puzzle_results.insert(result);
                    cout << p.to_string();
                }
            }
            return;
        }
        if (p.point_can_up(dst_p_r, dst_p_c) &&
            p.vrt_has_edge(dst_p_r - 1, dst_p_c) &&
            dst_p_r - 1 != v_r)
        {
            vrt_go_next(p,
                        start_r, start_c,
                        dst_p_r - 1, dst_p_c,
                        dst_p_r - 1, dst_p_c);
        }
        else if (p.point_can_down(dst_p_r, dst_p_c) &&
                 p.vrt_has_edge(dst_p_r, dst_p_c) &&
                 dst_p_r != v_r)
        {
            vrt_go_next(p,
                        start_r, start_c,
                        dst_p_r, dst_p_c,
                        dst_p_r + 1, dst_p_c);
        }
        else if (p.point_can_left(dst_p_r, dst_p_c) &&
                 p.hrz_has_edge(dst_p_r, dst_p_c - 1))
        {
            hrz_go_next(p,
                        start_r, start_c,
                        dst_p_r, dst_p_c - 1,
                        dst_p_r, dst_p_c - 1);
        }
        else if (p.point_can_right(dst_p_r, dst_p_c) &&
                 p.hrz_has_edge(dst_p_r, dst_p_c))
        {
            hrz_go_next(p,
                        start_r, start_c,
                        dst_p_r, dst_p_c,
                        dst_p_r, dst_p_c + 1);
        }
    }
    else
    {
        if (p.point_can_up(dst_p_r, dst_p_c) &&
            dst_p_r - 1 != v_r)
        {
            go_vrt(p,
                   start_r, start_c,
                   dst_p_r - 1, dst_p_c,
                   dst_p_r - 1, dst_p_c);
        }
        if (p.point_can_down(dst_p_r, dst_p_c) &&
            dst_p_r != v_r)
        {
            go_vrt(p,
                   start_r, start_c,
                   dst_p_r, dst_p_c,
                   dst_p_r + 1, dst_p_c);
        }
        if (p.point_can_left(dst_p_r, dst_p_c))
        {
            go_hrz(p,
                   start_r, start_c,
                   dst_p_r, dst_p_c - 1,
                   dst_p_r, dst_p_c - 1);
        }
        if (p.point_can_right(dst_p_r, dst_p_c))
        {
            go_hrz(p,
                   start_r, start_c,
                   dst_p_r, dst_p_c,
                   dst_p_r, dst_p_c + 1);
        }
    }
}

void hrz_go_next(puzzle &p,
                 const int &start_r, const int &start_c,
                 const int &h_r, const int &h_c,
                 const int &dst_p_r, const int &dst_p_c)
{
    if (p.get_conn(dst_p_r, dst_p_c) == 2)
    {
        // Solved
        if (dst_p_r == start_r && dst_p_c == start_c)
        {
            // Do final check
            if (p.is_fin())
            {
                // Print solution
                const auto result = p.to_string();
                if (puzzle_results.find(result) == puzzle_results.end())
                {
                    puzzle_results.insert(result);
                    cout << p.to_string();
                }
            }
            return;
        }
        if (p.point_can_up(dst_p_r, dst_p_c) &&
            p.vrt_has_edge(dst_p_r - 1, dst_p_c))
        {
            vrt_go_next(p,
                        start_r, start_c,
                        dst_p_r - 1, dst_p_c,
                        dst_p_r - 1, dst_p_c);
        }
        else if (p.point_can_down(dst_p_r, dst_p_c) &&
                 p.vrt_has_edge(dst_p_r, dst_p_c))
        {
            vrt_go_next(p,
                        start_r, start_c,
                        dst_p_r, dst_p_c,
                        dst_p_r + 1, dst_p_c);
        }
        else if (p.point_can_left(dst_p_r, dst_p_c) &&
                 p.hrz_has_edge(dst_p_r, dst_p_c - 1) &&
                 dst_p_c - 1 != h_c)
        {
            hrz_go_next(p,
                        start_r, start_c,
                        dst_p_r, dst_p_c - 1,
                        dst_p_r, dst_p_c - 1);
        }
        else if (p.point_can_right(dst_p_r, dst_p_c) &&
                 p.hrz_has_edge(dst_p_r, dst_p_c) &&
                 dst_p_c != h_c)
        {
            hrz_go_next(p,
                        start_r, start_c,
                        dst_p_r, dst_p_c,
                        dst_p_r, dst_p_c + 1);
        }
    }
    else
    {
        if (p.point_can_up(dst_p_r, dst_p_c))
        {
            go_vrt(p,
                   start_r, start_c,
                   dst_p_r - 1, dst_p_c,
                   dst_p_r - 1, dst_p_c);
        }
        if (p.point_can_down(dst_p_r, dst_p_c))
        {
            go_vrt(p,
                   start_r, start_c,
                   dst_p_r, dst_p_c,
                   dst_p_r + 1, dst_p_c);
        }
        if (p.point_can_left(dst_p_r, dst_p_c) &&
            dst_p_c - 1 != h_c)
        {
            go_hrz(p,
                   start_r, start_c,
                   dst_p_r, dst_p_c - 1,
                   dst_p_r, dst_p_c - 1);
        }
        if (p.point_can_right(dst_p_r, dst_p_c) &&
            dst_p_c != h_c)
        {
            go_hrz(p,
                   start_r, start_c,
                   dst_p_r, dst_p_c,
                   dst_p_r, dst_p_c + 1);
        }
    }
}
