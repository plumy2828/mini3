#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

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
std::array<std::array<int, SIZE>, SIZE> in_board;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> in_board[i][j];
        }
    }
}

/*------------------------------class start-------------------------------------------*/
class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }

    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
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
    void flip_discs(Point center) {
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
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }

public:
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE> in_board) {
        disc_count[EMPTY] = 0;
        disc_count[BLACK] = 0;
        disc_count[WHITE] = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = in_board[i][j];
                if(board[i][j] == BLACK)
                    disc_count[BLACK] ++;
                else if(board[i][j] == WHITE)
                    disc_count[WHITE] ++;
            }
        }
        cur_player = player;
        done = false;
        winner = -1;
    }
    OthelloBoard(const OthelloBoard &co){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = co.board[i][j];
            }
        }
        for(int i = 0; i < co.next_valid_spots.size(); i++){
            Point p = co.next_valid_spots[i];
            next_valid_spots.push_back(p);
        }
        cur_player = co.cur_player;
        disc_count[EMPTY] = disc_count[EMPTY];
        disc_count[BLACK] = disc_count[BLACK];
        disc_count[WHITE] = disc_count[WHITE];
        done = false;
        winner = -1;
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
    int flip_num(Point center) {
        int num = 0;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    num += discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
        return num;
    }
};




/*------------------------------class end-------------------------------------------*/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!set value start!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
Point edge1 = Point(0, 0), edge2 = Point(0, 7), edge3 = Point(7, 0), edge4 = Point(7, 7);
Point X1 = Point(1, 1), X2 = Point(1, 6), X3 = Point(6, 1), X4 = Point(6, 6);
Point x1 = Point(0, 1), x2 = Point(0, 6), x3 = Point(1, 0), x4 = Point(1, 7), x5 = Point(6, 0), x6 = Point(6, 7), x7 = Point(7, 1), x8 = Point(7, 6);
Point side1 = Point(0, 2), side2 = Point(0, 3), side3 = Point(0, 4), side4 = Point(0, 5), side5 = Point(7, 2), side6 = Point(7, 3), side7 = Point(7, 4), side8 = Point(7, 5), side9 = Point(2, 0), side10 = Point(3, 0), side11 = Point(4, 0), side12 = Point(5, 0), side13 = Point(2, 7), side14 = Point(3, 7), side15 = Point(4, 7), side16 = Point(5, 7);
Point great1 = Point(2, 2), great2 = Point(5, 5), great3 = Point(5, 3), great4 = Point(3, 5);
Point bad1 = Point(1, 2), bad2 = Point(1, 3), bad3 = Point(1, 4), bad4 = Point(1, 5), bad5 = Point(6, 2), bad6 = Point(6, 3), bad7 = Point(6, 4), bad8 = Point(6, 5), bad9 = Point(2, 1), bad10 = Point(3, 1), bad11 = Point(4, 1), bad12 = Point(5, 1), bad13 = Point(2, 6), bad14 = Point(3, 6), bad15 = Point(4, 6), bad16 = Point(5, 6);

int set_value(OthelloBoard &game, Point p){
    int value = 0;
    game.set_disc(p, game.cur_player);

    //cal position's point
    if(p == edge1 || p == edge2 || p == edge3 || p == edge4)
        value += 1000;
    else if(p == great1 || p == great2 || p == great3 || p == great4)
        value += 3;
    else if(p == side1 || p == side2 || p == side3 || p == side4 || p == side5 || p == side6 || p == side7 || p == side8 || p == side9 || p == side10 || p == side11 || p == side12 || p == side13 || p == side14 || p == side15 || p == side16)
        value += 20;
    else if(p == X1 || p == X2 || p == X3 || p == X4)
        value -= 500;
    else if(p == x1 || p == x2 || p == x3 || p == x4 || p == x5 || p == x6 || p == x7 || p == x8)
        value -= 50;
    else if(p == bad1 || p == bad2 || p == bad3 || p == bad4 || p == bad5 || p == bad6 || p == bad7 || p == bad8 || p == bad9 || p == bad10 || p == bad11 || p == bad12 || p == bad13 || p == bad14 || p == bad15 || p == bad16)
        value -= 10;
    else
        value += 0;

    //cal the flipped number
    value += game.flip_num(p);

    //remember to reset
    game.set_disc(p, 0);

    return value;
}
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!set value end!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


/*-------------------------------------miniMax start---------------------------------------------------*/
const int MAX = 100000000;
const int MIN = -100000000;
int better[100000];
int better_index = 0;

int minimax(int depth, int next_valid_spots_Index,
            bool maximizingPlayer,
			OthelloBoard &game, int alpha,
			int beta){
	// Terminating condition. i.e leaf node is reached
	if (depth == 3){
        Point p = game.next_valid_spots[next_valid_spots_Index];
        int value = set_value(game, p);
        return value;
	}

	OthelloBoard next = game;
	if(depth != 0){
        Point p = next.next_valid_spots[next_valid_spots_Index];
        next.put_disc(p);
	}

	if (maximizingPlayer){
		int best = MIN;
		// Recur for left and right children
		for (int i = 0; i < next.next_valid_spots.size(); i++){
			int val = minimax(depth + 1, i, false, next, alpha, beta);
			best = max(val, best);

            //better choice
            if(best > alpha && depth == 0){
                better[better_index] = i;
                better_index++;
			}

			alpha = max(alpha, best);
			// Alpha Beta Pruning
			if (beta <= alpha)
				break;
		}
		return best;
	}
	else{
		int best = MAX;
		// Recur for left and right children
		for (int i = 0; i < next.next_valid_spots.size(); i++) {
			int val = minimax(depth + 1, i, true, next, alpha, beta);
			best = min(best, val);
			beta = min(beta, best);

			// Alpha Beta Pruning
			if (beta <= alpha)
				break;
		}
		return best;
	}
}

/*-------------------------------------miniMax end-----------------------------------------------------*/

void read_valid_spots(std::ifstream& fin, OthelloBoard &game) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        game.next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout, OthelloBoard &first) {
    int n_valid_spots = first.next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = first.next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
    int num = minimax(0, 0, true, first, MIN, MAX);

    Point pp = first.next_valid_spots[better[--better_index]];
    fout << pp.x << " " << pp.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    OthelloBoard first(in_board);
    read_valid_spots(fin, first);
    write_valid_spot(fout, first);
    fin.close();
    fout.close();
    return 0;
}
