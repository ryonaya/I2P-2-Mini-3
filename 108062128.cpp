#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#define MIN -2100000000
#define MAX 2100000000
using namespace std;

// 1 = debug mode
bool dede = 0;
const int SIZE = 8;
enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};
const std::array<Point, 8> directions{{
    Point(-1, -1),Point(-1, 0),Point(-1, 1),
    Point(0, -1), /*{0, 0}, */ Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
}};

struct Board{
    array<array<int, SIZE>, SIZE> my_board; 
    vector<Point> my_valid_spots;
    array<int, 3> my_disc_count;
    int my_player;

    int& operator()(int i, int j) {
        return my_board[i][j];
    }
    Board& operator=(const Board& r){
        my_board = r.my_board;
        my_valid_spots = r.my_valid_spots;
        my_disc_count = r.my_disc_count;
        my_player = r.my_player;
        return *this;
    }
    int get_next_player(int player) {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) {
        return my_board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        my_board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(my_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, my_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    // cp for current player
    void put_disc(Point p) {
        set_disc(p, my_player);
        my_disc_count[my_player]++;
        my_disc_count[EMPTY]--;
        flip_discs(p);
    }
    vector<Point> get_valid_spots(){
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (my_board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(my_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, my_player)) {
                    for (Point s: discs) {
                        set_disc(s, my_player);
                    }
                    my_disc_count[my_player] += discs.size();
                    my_disc_count[get_next_player(my_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    void count_discs(){
        for(int i=0; i<2; i++)
            my_disc_count[i] = 0;
        for(int i=0; i<SIZE; i++)
        for(int j=0; j<SIZE; j++)
            my_disc_count[ my_board[i][j] ]++;
    }

    int eval(){
        my_player = get_next_player(my_player);
        int state_value = 0;
        if (my_disc_count[my_player] == 0) {
            return -1000;
        }
        for(int i=0; i<SIZE; i++){
            if(my_board[i][0] == my_player) state_value += 4;
            else if(my_board[i][0] == get_next_player(my_player)) state_value -= 3;
            if(my_board[i][7] == my_player) state_value += 4;
            else if(my_board[i][7] == get_next_player(my_player)) state_value -= 3;
            if(my_board[0][i] == my_player) state_value += 4;
            else if(my_board[0][i] == get_next_player(my_player)) state_value -= 3;
            if(my_board[7][i] == my_player) state_value += 4;
            else if(my_board[7][i] == get_next_player(my_player)) state_value -= 3;
        }
        for(int i=1; i<SIZE-1; i++){
            if(my_board[1][i] == my_player) state_value -= 3;
            if(my_board[6][i] == my_player) state_value -= 3;
            if(my_board[i][1] == my_player) state_value -= 3;
            if(my_board[i][6] == my_player) state_value -= 3;
        }

        if(my_disc_count[EMPTY] >= 45){
            state_value *= 2;
            state_value -= my_disc_count[my_player];
        }
        else if(my_disc_count[EMPTY] >= 25){
            state_value += my_disc_count[my_player];
        }
        else{
            state_value += my_disc_count[my_player] * 3;
        }
        return state_value;
    }
};

// ⣿⣿⣟⣽⣿⣿⣿⣿⣟⣵⣿⣿⣿⡿⣳⣫⣾⣿⣿⠟⠻⣿⣿⣿⢻⣿⣿⣿⣿⣷⡽⣿ 
// ⣿⣟⣾⡿⣿⣿⢟⣽⣿⣿⣿⣿⣫⡾⣵⣿⣿⣿⠃⠄⠄⠘⢿⣿⣾⣿⣿⣿⢿⣿⣿⡜ 
// ⡿⣼⡟⣾⣿⢫⣿⣿⣿⣿⡿⣳⣿⣱⣿⣿⣿⡋⠄⠄⠄⠄⠄⠛⠛⠋⠁⠄⠄⣿⢸⣿ 
// ⢳⣟⣼⡿⣳⣿⣿⣿⣿⡿⣹⡿⣃⣿⣿⣿⢳⠁⠄⠄⠄⢀⣀⠄⠄⠄⠄⠄⢀⣿⢿⣿ 
// ⡟⣼⣿⣱⣿⡿⣿⣿⣿⢡⣫⣾⢸⢿⣿⡟⣿⣶⡶⢰⣿⣿⣿⢷⠄⠄⠄⠄⢼⣿⣸⣿ 
// ⣽⣿⢣⣿⡟⣽⣿⣿⠃⣲⣿⣿⣸⣷⡻⡇⣿⣿⢇⣿⣿⣿⣏⣎⣸⣦⣠⡞⣾⢧⣿⣿ 
// ⣿⡏⣿⡿⢰⣿⣿⡏⣼⣿⣿⡏⠙⣿⣿⣤⡿⣿⢸⣿⣿⢟⡞⣰⣿⣿⡟⣹⢯⣿⣿⣿ 
// ⡿⢹⣿⠇⣿⣿⣿⣸⣿⣿⣿⣿⣦⡈⠻⣿⣿⣮⣿⣿⣯⣏⣼⣿⠿⠏⣰⡅⢸⣿⣿⣿ 
// ⡀⣼⣿⢰⣿⣿⣇⣿⣿⡿⠛⠛⠛⠛⠄⣘⣿⣿⣿⣿⣿⣿⣶⣿⠿⠛⢾⡇⢸⣿⣿⣿ 
// ⠄⣿⡟⢸⣿⣿⢻⣿⣿⣷⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡋⠉⣠⣴⣾⣿⡇⣸⣿⣿⡏
// ⠄⣿⡇⢸⣿⣿⢸⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠘⢿⣿⠏⠄⣿⣿⣿⣹ 
// ⠄⢻⡇⢸⣿⣿⠸⣿⣿⣿⣿⣿⣿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣦⣼⠃⠄⢰⣿⣿⢯⣿ 
// ⠄⢸⣿⢸⣿⣿⡄⠙⢿⣿⣿⡿⠁⠄⠄⠄⠄⠉⣿⣿⣿⣿⣿⣿⡏⠄⢀⣾⣿⢯⣿⣿ 
// ⣾⣸⣿⠄⣿⣿⡇⠄⠄⠙⢿⣀⠄⠄⠄⠄⠄⣰⣿⣿⣿⣿⣿⠟⠄⠄⣼⡿⢫⣻⣿⣿ 
// ⣿⣿⣿⠄⢸⣿⣿⠄⠄⠄⠄⠙⠿⣷⣶⣤⣴⣿⠿⠿⠛⠉⠄⠄ ⢸⣿⣿⣿⣿⠃⠄

int player;
Board board;
vector<Point> next_valid_spots;

void read_board(ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board.my_board[i][j];
        }
    }
}
void read_valid_spots(ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}
void write_valid_spot(ofstream& fout, int x, int y) {
    fout << x << " " << y << endl;
    if(dede)fout << "\nI am " << player << endl;
    fout.flush();
}

// ⠄⠄⠄⠄⠄⠄⠄⠈⠉⠁⠈⠉⠉⠙⠿⣿⣿⣿⣿⣿
// ⠄⠄⠄⠄⠄⠄⠄⠄⣀⣀⣀⠄⠄⠄⠄⠄⠹⣿⣿⣿
// ⠄⠄⠄⠄⠄⢐⣲⣿⣿⣯⠭⠉⠙⠲⣄⡀⠄⠈⢿⣿
// ⠐⠄⠄⠰⠒⠚⢩⣉⠼⡟⠙⠛⠿⡟⣤⡳⡀⠄⠄⢻
// ⠄⠄⢀⣀⣀⣢⣶⣿⣦⣭⣤⣭⣵⣶⣿⣿⣏⠄⠄⣿
// ⠄⣼⣿⣿⣿⡉⣿⣀⣽⣸⣿⣿⣿⣿⣿⣿⣿⡆⣀⣿
// ⢠⣿⣿⣿⠿⠟⠛⠻⢿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣼
// ⠄⣿⣿⣿⡆⠄⠄⠄⠄⠳⡈⣿⣿⣿⣿⣿⣿⣿⣿⣿
// ⠄⢹⣿⣿⡇⠄⠄⠄⠄⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
// ⠄⠄⢿⣿⣷⣨⣽⣭⢁⣡⣿⣿⠟⣩⣿⣿⣿⠿⠿⠟
// ⠄⠄⠈⡍⠻⣿⣿⣿⣿⠟⠋⢁⣼⠿⠋⠉⠄⠄⠄⠄
// ⠄⠄⠄⠈⠴⢬⣙⣛⡥⠴⠂⠄⠄⠄⠄⠄⠄⠄⠄⠄


// bd = board, not for BD master

int recur(ofstream& fout, Board bd, int depth, int alpha, int beta){
    if(depth >= (dede ? 3 : 9) || bd.my_disc_count[EMPTY] == 0){
        int k = bd.eval();
        if(dede)fout << "          " << "value = "<< k << endl;
        return k;
    }
    
    bd.my_valid_spots = bd.get_valid_spots();
    
    if(bd.my_player == player){
        int best = MIN;
        for(Point p : bd.my_valid_spots){
            if(dede) switch (depth)
            {
            case 0:
                fout << "ME  ";
                break;
            case 1:
                fout << "ME     ";
                break;
            case 2:
                fout << "ME        ";
                break;
            default:
                break;
            }if(dede)fout << p.x << ", " << p.y << "-------------" << endl;
            if(dede)fout.flush();

            Board tmp = bd;
            tmp.put_disc(p);
            tmp.my_player = tmp.get_next_player(tmp.my_player);
            int val = recur(fout, tmp, depth+1, alpha, beta);

            if(best < val){
                if(depth == 0){
                   if(dede) fout << "\nA change here, " << best << " -> " << val << endl << "So the answer is now : ";
                   if(dede) fout.flush();
                    write_valid_spot(fout, p.x, p.y);
                }
                best = val;
            }
            alpha= max(best, alpha);

            if(dede) switch (depth)
            {
            case 0:
                fout << "    ";
                break;
            case 1:
                fout << "       ";
                break;
            case 2:
                fout << "          ";
                break;
            default:
                break;
            }if(dede)fout << "alph = " << alpha << " beta = " << beta << " best = " << best << endl;
            if(dede)fout.flush();

            if(beta <= alpha){
                if(dede)fout << "          [BREAK]\n";
                break;
            }
        }
        if(depth != 0) return best;
    }
    else {
        int best = MAX;
        for(Point p : bd.my_valid_spots){
            if(dede) switch (depth)
            {
            case 0:
                cout << "OP  ";
                break;
            case 1:
                fout << "OP     ";
                break;
            case 2:
                fout << "OP        ";
                break;
            default:
                break;
            }if(dede)fout << p.x << ", " << p.y << "-------------" << endl;
            if(dede)fout.flush();

            Board tmp = bd;
            tmp.put_disc(p);
            tmp.my_player = tmp.get_next_player(tmp.my_player);
            int val = recur(fout, tmp, depth+1, alpha, beta);

            best = min(best, val);
            beta = min(best, beta);
            if(dede) switch (depth)
            {
            case 0:
                fout << "    ";
                break;
            case 1:
                fout << "       ";
                break;
            case 2:
                fout << "          ";
                break;
            default:
                break;
            }if(dede)fout << "alph = " << alpha << " beta = " << beta << " best = " << best << endl;
            if(dede)fout.flush();

            if(beta <= alpha){
                if(dede) fout << "          [BREAK]\n";
                break;
            }
        }
        if(depth != 0) return best;
    }
    return 1112;
}

int main(int, char** argv) {
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    board.count_discs();
    board.my_player = player;
    board.my_valid_spots = next_valid_spots;

    recur(fout, board, 0, MIN, MAX);

    // fout << "-10000000" << endl;
    fin.close();
    fout.close();
    return 0;
}
