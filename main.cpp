#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>

using namespace std;

class puzzle
{
public:
    size_t cols, rows = 0;   // of lattice
    vector<vector<int>> lat; // lattice

    enum edge_state
    {
        BAN = -1,   // banned
        NOT = 0,    // not linked
        LINKED = 1, // linked
        MUST = 2    // must linked
    };
    vector<vector<edge_state>> hrz; // horizon link
    vector<vector<edge_state>> vrt; // vertical link

    vector<vector<bool>> banned_point;

    size_t get_conn(const int &p_r, const int &p_c)
    {
        size_t conn = 0;
        if (point_can_up(p_r, p_c) && vrt_has_edge(p_r - 1, p_c))
            conn++;
        if (point_can_down(p_r, p_c) && vrt_has_edge(p_r, p_c))
            conn++;
        if (point_can_left(p_r, p_c) && hrz_has_edge(p_r, p_c - 1))
            conn++;
        if (point_can_right(p_r, p_c) && hrz_has_edge(p_r, p_c))
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

    int hrz_has_edge(const int &h_r, const int &h_c)
    {
        return hrz[h_r][h_c] == LINKED || hrz[h_r][h_c] == MUST;
    }

    int vrt_has_edge(const int &v_r, const int &v_c)
    {
        return vrt[v_r][v_c] == LINKED || vrt[v_r][v_c] == MUST;
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

    bool is_fin()
    {
        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                if (lat[row][col] >= 0 &&
                    lat[row][col] != lat_edge(row, col))
                    return false;
            }
        }
        return true;
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
                else
                    ss << dot;
                if (col < cols)
                {
                    if (hrz_has_edge(row, col))
                        ss << link;
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

void heuristic(puzzle &p);

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
    heuristic(p);
    DFS(p);
    cout << "Solutions: " << puzzle_results.size() << endl;
    return 0;
}

void heuristic(puzzle &p)
{
    // ban around zero
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

void DFS(puzzle &p)
{
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
    // Link overlap
    if (p.vrt_has_edge(v_r, v_c))
        return;
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
    // Go next point
    go_next(p, start_r, start_c, dst_p_r, dst_p_c);
}

void go_hrz(puzzle p, const int &start_r, const int &start_c,
            const int &h_r, const int &h_c,
            const int &dst_p_r, const int &dst_p_c)
{
    // Link overlap
    if (p.hrz_has_edge(h_r, h_c))
        return;
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
    // Go next point
    go_next(p, start_r, start_c, dst_p_r, dst_p_c);
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
