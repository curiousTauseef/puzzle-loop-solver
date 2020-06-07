#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <set>
#include <queue>

using namespace std;

class puzzle
{
public:
    size_t cols, rows = 0;   // of lattice
    vector<vector<int>> lat; // lattice

    enum edge_state
    {
        BAN = -1,  // banned
        NOT = 0,   // not linked
        LINKED = 1 // linked
    };
    vector<vector<edge_state>> hrz; // horizon link
    vector<vector<edge_state>> vrt; // vertical link

    vector<vector<bool>> banned_point;

    size_t get_conn(const int &p_r, const int &p_c)
    {
        size_t conn = 0;
        if (point_has_edge_up(p_r, p_c))
            conn++;
        if (point_has_edge_down(p_r, p_c))
            conn++;
        if (point_has_edge_left(p_r, p_c))
            conn++;
        if (point_has_edge_right(p_r, p_c))
            conn++;
        return conn;
    }

    size_t lat_edge(const int &lat_r, const int &lat_c)
    {
        return hrz_has_edge(lat_r, lat_c) +
               hrz_has_edge(lat_r + 1, lat_c) +
               vrt_has_edge(lat_r, lat_c) +
               vrt_has_edge(lat_r, lat_c + 1);
    }

    size_t get_lat_banned_edge(const int &lat_r, const int &lat_c)
    {
        return (hrz[lat_r][lat_c] == BAN ? 1 : 0) +
               (hrz[lat_r + 1][lat_c] == BAN ? 1 : 0) +
               (vrt[lat_r][lat_c] == BAN ? 1 : 0) +
               (vrt[lat_r][lat_c + 1] == BAN ? 1 : 0);
    }

    bool hrz_sat(const int &h_r, const int &h_c)
    {
        if (hrz_has_up_lat(h_r, h_c) &&
            lat[h_r - 1][h_c] >= 0 &&
            lat_edge(h_r - 1, h_c) > lat[h_r - 1][h_c])
            return false;
        if (hrz_has_down_lat(h_r, h_c) &&
            lat[h_r][h_c] >= 0 &&
            lat_edge(h_r, h_c) > lat[h_r][h_c])
            return false;
        return true;
    }

    bool vrt_sat(const int &v_r, const int &v_c)
    {
        if (vrt_has_left_lat(v_r, v_c) &&
            lat[v_r][v_c - 1] >= 0 &&
            lat_edge(v_r, v_c - 1) > lat[v_r][v_c - 1])
            return false;
        if (vrt_has_right_lat(v_r, v_c) &&
            lat[v_r][v_c] >= 0 &&
            lat_edge(v_r, v_c) > lat[v_r][v_c])
            return false;
        return true;
    }

    bool point_can_up(const int &p_r, const int &p_c)
    {
        if (p_r <= 0)
            return false;
        if (banned_point[p_r - 1][p_c])
            return false;
        if (vrt[p_r - 1][p_c] == puzzle::BAN)
            return false;
        return true;
    }

    bool point_can_down(const int &p_r, const int &p_c)
    {
        if (p_r >= rows)
            return false;
        if (banned_point[p_r + 1][p_c])
            return false;
        if (vrt[p_r][p_c] == puzzle::BAN)
            return false;
        return true;
    }

    bool point_can_left(const int &p_r, const int &p_c)
    {
        if (p_c <= 0)
            return false;
        if (banned_point[p_r][p_c - 1])
            return false;
        if (hrz[p_r][p_c - 1] == puzzle::BAN)
            return false;
        return true;
    }

    bool point_can_right(const int &p_r, const int &p_c)
    {
        if (p_c >= cols)
            return false;
        if (banned_point[p_r][p_c + 1])
            return false;
        if (hrz[p_r][p_c] == puzzle::BAN)
            return false;
        return true;
    }

    bool point_has_edge_up(const int &p_r, const int &p_c)
    {
        return point_can_up(p_r, p_c) && vrt_has_edge(p_r - 1, p_c);
    }

    bool point_has_edge_down(const int &p_r, const int &p_c)
    {
        return point_can_down(p_r, p_c) && vrt_has_edge(p_r, p_c);
    }

    bool point_has_edge_left(const int &p_r, const int &p_c)
    {
        return point_can_left(p_r, p_c) && hrz_has_edge(p_r, p_c - 1);
    }

    bool point_has_edge_right(const int &p_r, const int &p_c)
    {
        return point_can_right(p_r, p_c) && hrz_has_edge(p_r, p_c);
    }

    int hrz_has_edge(const int &h_r, const int &h_c)
    {
        return hrz[h_r][h_c] == LINKED;
    }

    int vrt_has_edge(const int &v_r, const int &v_c)
    {
        return vrt[v_r][v_c] == LINKED;
    }

    bool hrz_has_up_lat(const int &h_r, const int &h_c)
    {
        return h_r > 0;
    }

    bool hrz_has_down_lat(const int &h_r, const int &h_c)
    {
        return h_r < rows;
    }

    bool vrt_has_left_lat(const int &v_r, const int &v_c)
    {
        return v_c > 0;
    }

    bool vrt_has_right_lat(const int &v_r, const int &v_c)
    {
        return v_c < cols;
    }

    bool complete_lat(const int &lat_r, const int &lat_c)
    {
        if (lat[lat_r][lat_c] >= 0 &&
            lat[lat_r][lat_c] != lat_edge(lat_r, lat_c))
            return false;
        else
            return true;
    }

    bool is_fin()
    {
        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                if (!complete_lat(row, col))
                    return false;
            }
        }
        for (size_t row = 0; row <= rows; row++)
        {
            for (size_t col = 0; col <= cols; col++)
            {
                if (get_conn(row, col) == 1 || get_conn(row, col) == 3)
                {
                    return false;
                }
            }
        }
        if (is_multiple_loops())
            return false;
        return true;
    }

    bool is_correct() // During solving
    {
        // check lattice
        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                if (lat[row][col] >= 0)
                {
                    if (lat[row][col] < lat_edge(row, col))
                        return false;
                    if (4 - lat[row][col] < get_lat_banned_edge(row, col))
                        return false;
                }
            }
        }
        // check point
        for (size_t row = 0; row <= rows; row++)
        {
            for (size_t col = 0; col <= cols; col++)
            {
                if (get_conn(row, col) == 3)
                {
                    return false;
                }
                // if (get_conn(row, col) == 1)
                // {
                //     size_t ban_edges = (point_can_up(row, col) ? 0 : 1) +
                //                        (point_can_left(row, col) ? 0 : 1) +
                //                        (point_can_down(row, col) ? 0 : 1) +
                //                        (point_can_right(row, col) ? 0 : 1);
                //     if (ban_edges >= 3)
                //         return false;
                // }
            }
        }
        if (is_multiple_loops())
            return false;
        return true;
    }

    bool is_multiple_loops()
    {
        set<pair<int, int>> done;
        size_t loops = 0;
        size_t lines = 0;
        for (size_t row = 0; row <= rows; row++)
        {
            for (size_t col = 0; col <= cols; col++)
            {
                // New point with 2 conn
                // Maybe in a loop
                if (get_conn(row, col) == 2 && done.find({row, col}) == done.end())
                {
                    // Mark one loop or line (BFS)
                    bool is_line = false;
                    queue<pair<int, int>> todo;
                    todo.push({row, col});
                    do
                    {
                        const auto now = todo.front();
                        todo.pop();
                        if (done.find(now) != done.end())
                        {
                            continue;
                        }
                        done.insert(now);
                        if (get_conn(now.first, now.second) == 1) // is a line
                        {
                            is_line = true;
                        }
                        if (point_has_edge_up(now.first, now.second))
                        {
                            if (done.find({now.first - 1, now.second}) == done.end())
                            {
                                todo.push({now.first - 1, now.second});
                            }
                        }
                        if (point_has_edge_down(now.first, now.second))
                        {
                            if (done.find({now.first + 1, now.second}) == done.end())
                            {
                                todo.push({now.first + 1, now.second});
                            }
                        }
                        if (point_has_edge_left(now.first, now.second))
                        {
                            if (done.find({now.first, now.second - 1}) == done.end())
                            {
                                todo.push({now.first, now.second - 1});
                            }
                        }
                        if (point_has_edge_right(now.first, now.second))
                        {
                            if (done.find({now.first, now.second + 1}) == done.end())
                            {
                                todo.push({now.first, now.second + 1});
                            }
                        }
                    } while (!todo.empty());
                    if (is_line)
                    {
                        lines++;
                    }
                    else
                    {
                        loops++;
                    }
                    if (loops > 1)
                        return true;
                    if (loops == 1 && lines > 0)
                        return true;
                }
            }
        }
        return false;
    }

    string to_string()
    {
        const auto link = "█";
        const auto dot = "▪";
        stringstream ss;
        for (size_t row = 0; row <= rows; row++)
        {
            for (size_t col = 0; col <= cols; col++)
            {
                if (get_conn(row, col) > 0)
                    ss << link;
                // else if (banned_point[row][col])
                //     ss << "x";
                else
                    ss << dot;
                if (col < cols)
                {
                    if (hrz_has_edge(row, col))
                        ss << link;
                    else if (hrz[row][col] == BAN)
                        ss << "x";
                    else
                        ss << " ";
                }
            }
            ss << "\n";
            if (row < rows)
            {
                for (size_t col = 0; col <= cols; col++)
                {
                    if (vrt_has_edge(row, col))
                        ss << link;
                    else if (vrt[row][col] == BAN)
                        ss << "x";
                    else
                        ss << " ";
                    if (col < cols)
                    {
                        if (lat[row][col] >= 0)
                            ss << lat[row][col];
                        else
                            ss << " ";
                    }
                }
            }
            ss << "\n";
        }
        return ss.str();
    }
};
