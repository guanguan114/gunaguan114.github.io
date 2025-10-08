#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

#define MAP_SIZE 100

enum COLORS {
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY,
	DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
};

void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

struct Info {
	int color;
	char name;
	bool access;
	int digable;
	Info() {}
	Info(int a, char b, bool c, int d) : color(a), name(b), access(c), digable(d) {}
};

namespace recog {
	string said;
	void print() {
		setColor(WHITE);
		printf("%s", said.c_str());
	}
}

const Info Grass(GREEN, 'G', 1, 0);
const Info Stone(LIGHTGRAY, 'S', 0, 0);
const Info Wood(BROWN, 'W', 1, 0);
const Info Water(LIGHTBLUE, '#', 1, 0);
const Info Person(WHITE, '@', 1, 0);
const Info Ice(LIGHTCYAN, 'I', 1, 1);
const Info Flower(YELLOW, 'F', 1, 0);
const Info FloodingWater(LIGHTBLUE, '=', 1, 0);
const Info Iron(WHITE, '+', 0, 1);
const Info NPC(LIGHTRED, 'N', 0, 0);
const Info SIDE(LIGHTBLUE, 'O', 0, 0);
const Info Sand(YELLOW, 'B', 1, 0);
const Info House(BROWN, 'H', 1, 0);

bool operator!=(Info i, Info j) {
	return i.name != j.name;
}

bool operator==(Info i, Info j) {
	return !(i != j);
}

Info Mp[MAP_SIZE][MAP_SIZE];

void generateMap() {

}

int main() {
	generateMap();
	map<char, int> inventory;
	int digLevel = 0;
	int x = 50, y = 50;

	while (true) {
		renderMap(x, y);
		int input = _getch();
		int xx = x, yy = y;

		switch (input) {
			case 'W':
			case 'w':
				x--;
				break;
			case 'A':
			case 'a':
				y--;
				break;
			case 'S':
			case 's':
				x++;
				break;
			case 'D':
			case 'd':
				y++;
				break;
			case 'G':
			case 'g':
				if (digLevel >= Mp[x][y + 1].digable && Mp[x][y + 1].access) {
					if (Mp[x][y + 1] == Stone) {
						inventory[Stone.name]++;
						Mp[x][y + 1] = Grass;
					}
					if (Mp[x][y + 1] == Ice) {
						Mp[x][y + 1] = Water;
					}
					if (Mp[x][y + 1] == Iron) {
						inventory[Iron.name]++;
						Mp[x][y + 1] = Stone;
					}
				}
				break;
			case 'T':
			case 't':
				if (Mp[x][y + 1] == NPC) {
					recog::said = "Hello! Welcome to the village.";
				}
				break;
		}

		if (!Mp[x][y].access || x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
			x = xx, y = yy;
		}

		recog::print();
	}

	return 0;
}




