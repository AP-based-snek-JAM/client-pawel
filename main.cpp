#include <clocale>
#include <curses.h>
#include <functional>
#include <ncurses.h>
#include <ratio>
#include <string>
#include <tuple>
#include <vector>
#include <unistd.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <random>


using namespace std;

const int width = 20;
const int height = 20;

int hitCount = 0;
int headPos_x = width/2;
int headPos_y = height/2;
int foodPos_x = 5;
int foodPos_y = 5;
int snekLength = 2;
bool foodPresent = false;

int renderDelay = 100000;
int keyboardDelay = 10000;

bool gameOver = false;

bool done = false;
int ch;
int dir;

class Tile {
    public:
	int ttl;

	Tile(): ttl(0){}
};

void update_field(WINDOW* snekWin, vector<vector<Tile>> &field){
    field[headPos_y-1][headPos_x-1].ttl = snekLength;

    
    for(vector<Tile> &line: field){
	for(Tile &tile: line){
	    tile.ttl -= (tile.ttl == 0) ? 0 : 1;
	}
    }
}

void update_food(WINDOW* snekWin){
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> dist6(1,width-2);
    //cout << dist6(rng) << " " << dist6(rng) << endl;

    if(headPos_x == foodPos_x && headPos_y == foodPos_y){
	snekLength += 1;
	foodPresent = false;
    }
    
    if(!foodPresent){
	foodPos_x = dist6(rng);
	foodPos_y = dist6(rng);
	foodPresent = true;
    }
}

void update_snake(WINDOW* snekWin, int dir, vector<vector<Tile>> field){
	switch (dir) {
	    case 68:
		headPos_x--;
		break;

	    case 67:
		headPos_x++;
		break;

	    case 65:
		headPos_y--;
		break;

	    case 66:
		headPos_y++;
		break;
	}
	
	werase(snekWin);

	box(snekWin, 0, 0);

	mvwprintw(snekWin, 0, 1, "Snek");
	

	int t_x = 0;
	int t_y = 0;
	for(vector<Tile> line: field){
	    for(Tile tile: line){
		//mvwprintw(snekWin, t_y+1, t_x+1, "%s", to_string(tile.ttl).c_str());

		wattron(snekWin, COLOR_PAIR(2));
		mvwprintw(snekWin, t_y+1, t_x+1, "%s", (tile.ttl > 0) ? to_string(tile.ttl).c_str() : " ");
		wattroff(snekWin, COLOR_PAIR(2));
		if((headPos_x-1 == t_x && headPos_y-1 == t_y) && (0 < tile.ttl && tile.ttl < snekLength-1)){
		    hitCount++;
		    if(hitCount > 0){
			gameOver = true;
		    }
		    //hitCount = tile.ttl;
		}

		t_x++;
		//wrefresh(snekWin);
	    }
	    t_y++;
	    t_x = 0;
	}
	
	if(headPos_x > width-2 || headPos_x < 1 || headPos_y > height-2 || headPos_y < 1){
	    gameOver = true;
	}
	
	//mvwaddwstr(snekWin, headPos_y, headPos_x, L"🗿");
	

	wattron(snekWin, COLOR_PAIR(1));
	mvwprintw(snekWin, foodPos_y, foodPos_x, "F");
	wattroff(snekWin, COLOR_PAIR(1));

	wattron(snekWin, COLOR_PAIR(2));
	mvwaddwstr(snekWin, headPos_y, headPos_x, L"S");
	wattroff(snekWin, COLOR_PAIR(2));

	wrefresh(snekWin);
}

void update_debug(WINDOW* debugWin, vector<string> values){
    werase(debugWin);
    wresize(debugWin, values.size()+2, 30);

    box(debugWin, 0, 0);
    mvwprintw(debugWin, 0, 1, "Debug");

    int currentLine = 1;
    for(const string s: values){
	mvwprintw(debugWin, currentLine, 1, "%s", s.c_str());
	currentLine++;
    }

    wrefresh(debugWin);
}

int main(int argc, char **argv)
{
    vector<vector<Tile>> field;
    field.resize(height-2);
    for(int i=0;i<height-2;i++){
	field[i].resize(width-2);
    }
    setlocale(LC_ALL, "");
    initscr();

    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    curs_set(0);
    nodelay(stdscr, true);

    // creating a snekWindow;
    // with height = 15 and width = 10
    // also with start x axis 10 and start y axis = 20
    WINDOW *snekWin = newwin(width, height, 0, 0);
    WINDOW *debugWin = newwin(6, 30, 0, width);
    //nodelay(snekWin, true);
    noecho();
    //timeout(-1);
    refresh();


    // refreshing the snekWindow
    wrefresh(snekWin);


    auto start_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();
    auto diff = chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

    while (!done) {
	ch = getch();
	
	switch(ch){
	    case 67:
		dir = 67;
		break;
	    case 68:
		dir = 68;
		break;
	    case 65:
		dir = 65;
		break;
	    case 66:
		dir = 66;
		break;
	}

	if(gameOver){
	    mvwprintw(snekWin, height/2, width/2-5, "GAME OVER!");
	    mvwprintw(snekWin, height/2+1, width/2-5, "SCORE: %d", snekLength-2);
	    wrefresh(snekWin);

	    if(ch == 10 || ch == 32){
		gameOver = false;
		snekLength = 2;
		hitCount = 0;
		headPos_x = width/2;
		headPos_y = height/2;


		for(vector<Tile> &line: field){
		    for(Tile &tile: line){
			tile.ttl = 0;
		    }
		}
		//cout << ch << endl;
	    }

	    continue;
	}
	
	current_time = std::chrono::high_resolution_clock::now();
	diff = chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

	if (diff > 200) {	
	    update_snake(snekWin, dir, field);
	    update_field(snekWin, field);
	    update_food(snekWin);
	    start_time = std::chrono::high_resolution_clock::now();
	}

	int totalTtl = 0;

	for(vector<Tile> line: field){
	    for(Tile tile: line){
		totalTtl += tile.ttl;
	    }
	}

	update_debug(debugWin, {"diff: "+to_string(diff), "f_xy: ("+to_string(foodPos_x)+";"+to_string(foodPos_y)+")", "h_xy: ("+to_string(headPos_x)+";"+to_string(headPos_y)+")", "ch: "+to_string(dir), "ttl: "+to_string(totalTtl), "len: "+to_string(snekLength), "hit: "+to_string(hitCount)});
    }

    endwin();
    return 0;
}
