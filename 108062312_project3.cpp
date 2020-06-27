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
        disc_count[EMPTY] = co.disc_count[EMPTY];
        disc_count[BLACK] = co.disc_count[BLACK];
        disc_count[WHITE] = co.disc_count[WHITE];
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
int mapp[8][8]{
    500, -25, 10, 5, 5, 10, -25, 500,
    -25, -45, 1, 1, 1, 1, -45, -25,
    10, 1, 3, 2, 2, 3, 1, 10,
    5, 1, 2, 1, 1, 2, 1, 5,
    5, 1, 2, 1, 1, 2, 1, 5,
    10, 1, 3, 2, 2, 3, 1, 10,
    -25, -45, 1, 1, 1, 1, -45, -25,
    500, -25, 10, 5, 5, 10, -25, 500
};

int set_value(OthelloBoard &in, Point p){
    OthelloBoard game = in;
    int value = 0;
    game.put_disc(p);

    //cal position's point
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(game.board[i][j] == player)
                value += mapp[i][j];
            else if(game.board[i][j] == (3-player))
                value -= mapp[i][j];
        }
    }
    return value;
}

int set_value2(OthelloBoard &in){
    int value = 0;
    //cal position's point
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(in.board[i][j] == player)
                value += mapp[i][j];
            else if(in.board[i][j] == (3-player))
                value -= mapp[i][j];
        }
    }
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

	if (maximizingPlayer){//depth = 0 or 2
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
		if(next.next_valid_spots.size() == 0){
            int val = set_value2(next);
            best = max(val, best);
            //better choice
            if(best > alpha && depth == 0){
                better[better_index] = 0;
                better_index++;
			}

			alpha = max(alpha, best);
		}
		return best;
	}
	else{// depth = 1
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
		if(next.next_valid_spots.size() == 0){
            int val = set_value2(next);
            best = min(val, best);
			beta = min(beta, best);
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
    /*int index = (rand() % n_valid_spots);
    Point p = first.next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();*/
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
