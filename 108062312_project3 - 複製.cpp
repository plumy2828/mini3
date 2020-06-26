#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cassert>


using namespace std;
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
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

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

void read_board() {
    cin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cin >> board[i][j];
        }
    }
}

void read_valid_spots() {
    int n_valid_spots;
    cin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        cin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}
/*---------------------------------------begin-----------------------------------------------------------*/
const std::array<Point, 8> directions{{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1), /*{0, 0}, */Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
}};
enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
int cur_player = 1;
bool is_spot_valid(Point center);
int get_disc(Point p);
bool is_disc_at(Point p, int disc);
int get_next_player(int player);
bool is_spot_on_board(Point p);
void flip_discs(Point center);
void set_disc(Point p, int disc);

bool is_spot_valid(Point center){
    if (get_disc(center) != EMPTY)
        return false;
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, get_next_player(cur_player)))
            continue;
        p = p + dir;
        while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
            if (is_disc_at(p, cur_player))
                return true;
            p = p + dir;
        }
    }
    return false;
}

int get_disc(Point p){
    return board[p.x][p.y];
}
bool is_disc_at(Point p, int disc){
    if (!is_spot_on_board(p))
        return false;
    if (get_disc(p) != disc)
        return false;
    return true;
}
int get_next_player(int player){
    return 3 - player;
}
bool is_spot_on_board(Point p){
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
void flip_discs(Point center){
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, get_next_player(cur_player)))
            continue;
        std::vector<Point> discs({p});
        p = p + dir;
        while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
            if (is_disc_at(p, cur_player)) {
                for (Point s: discs) {
                    set_disc(s, cur_player);
                }
                break;
            }
            discs.push_back(p);
            p = p + dir;
        }
    }
}

void set_disc(Point p, int disc){
    board[p.x][p.y] = disc;
}


//debug
void print_for_debug(){
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << board[i][j] << " ";
        }
        cout << "\n";
    }
}
/*---------------------------------------end-----------------------------------------------------------*/


Point edge[4] = {Point(0, 0), Point(0, 7), Point(7, 0), Point(7, 7)};
Point X[4] = {Point(1, 1), Point(1, 6), Point(6, 1), Point(6, 6)};
Point x[8] = {Point(0, 1), Point(0, 6), Point(1, 0), Point(1, 7), Point(6, 0), Point(6, 7), Point(7, 1), Point(7, 6)};
void write_valid_spot() {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    /* do once security output */
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    cout << p.x << " " << p.y << std::endl;
    flip_discs(p);
    print_for_debug();
    /* better output*/
    /*while(true){

        Point p = next_valid_spots[index];
        fout << p.x << " " << p.y << std::endl;
        fout.flush();
    }*/
}

int main(int, char** argv) {
    read_board();
    read_valid_spots();
    cout << "\n";
    write_valid_spot();
    return 0;
}
