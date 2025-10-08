#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;
#define cout YOU_CANNOT_USE_IT
#define mmap_SIZE 100
#define MAP_SIZE 100
enum COLORS {
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY,
	DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
};
void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
#define put(a) setColor((a).color) , printf("%c" , (a).name);
struct infor {
	int color;
	char name;
	bool access;
	int digable;
	infor() {
		
	}
	infor(int a, char b, bool c, int d) {
		color = a;
		name = b;
		access = c;
		digable = d;
	}
};
namespace recog {
	string said;
	void print() {
		setColor(WHITE);
		printf("%s", said.c_str());
	}
}
const infor Grass(GREEN, 'G', 1, 0);
const infor Stone(LIGHTGRAY, 'S', 0, 0);
const infor Wood(BROWN, 'W', 1, 0);
const infor Water(LIGHTBLUE, '#', 1, 0);
const infor Person(WHITE, 'H', 1, 0);
const infor Ice(LIGHTCYAN, 'I', 1, 1);
const infor Flower(YELLOW, 'F', 1, 0);
const infor FloodingWater(LIGHTBLUE, '=', 1, 0);
const infor Iron(WHITE, '+', 0, 1);
const infor NPC(LIGHTRED, 'H', 0, 0);
const infor SIDE(LIGHTBLUE, 'O', 0, 0);
const infor Sand(YELLOW, 'B' , 1 , 0);
const infor House(BROWN, 'H', 1, 0);
bool operator!=(infor i, infor j) {
	return i.name != j.name;
}
bool operator==(infor i, infor j) {
	return !(i != j);
}
infor Mp[MAP_SIZE][MAP_SIZE];
int main() {
	srand(time(0));
	for (auto &i : Mp) {
		for (auto &j : i) {
			if(rand() % 10)j = Grass;
			else if(rand() % 13 <= 6)j = Wood;
			else if(rand() % 13 <= 4)j = Stone;
			else if(rand() % 13 == 0)j = Flower;
			else j = Grass;
		}
	}
	for (auto &i : Mp) {
		for (auto &j : i) {
			if (j == Flower) {
				j.color = (rand() % 3 == 1 ? YELLOW : (rand() & 1 ? GREEN : LIGHTRED));
			}
		}
	}
#ifndef NONWATER
	for (int i = 0; i < 5; i++) {
		int x = rand() % (MAP_SIZE - 10) + 5;
		int y = rand() % (MAP_SIZE - 10) + 5;
		int size = rand() % 5 + 3;
		
		for (int dx = -size; dx <= size; dx++) {
			for (int dy = -size; dy <= size; dy++) {
				if (x + dx > 0 && x + dx < MAP_SIZE - 1 && y + dy > 0 && y + dy < MAP_SIZE - 1) {
					if (dx * dx + dy * dy <= size * size) {
						if (rand() % 104 == 0) {
							Mp[x + dx][y + dy] = Ice;
						} else {
							Mp[x + dx][y + dy] = Water;
						}
					}
				}
			}
		}
	}
	// 生成河流
	for (int i = 0; i < 3; i++) {
		int x = rand() % (MAP_SIZE - 20) + 10;
		int y = rand() % (MAP_SIZE - 20) + 10;
		int length = rand() % 30 + 20;
		int width = rand() % 3 + 1;
		
		for (int l = 0; l < length; l++) {
			// 随机改变河流方向
			if (rand() % 10 == 0) x += rand() % 3 - 1;
			if (rand() % 10 == 0) y += rand() % 3 - 1;
			
			for (int w = -width - (rand() % 4 - 2); w <= width + (rand() % 4 - 2); w++) {
				if (x + l > 0 && x + l < MAP_SIZE - 1 && y + w > 0 && y + w < MAP_SIZE - 1) {
					Mp[x + l][y + w] = FloodingWater;
				}
			}
		}
	}
#endif
#ifndef NONMINE
	// 生成石头矿脉
	for (int i = 0; i < 10; i++) {
		int x = rand() % (mmap_SIZE - 10) + 5;
		int y = rand() % (mmap_SIZE - 10) + 5;
		int size = rand() % 5 + 3;
		
		for (int dx = -size; dx <= size; dx++) {
			for (int dy = -size; dy <= size; dy++) {
				if (x + dx > 0 && x + dx < mmap_SIZE - 1 && y + dy > 0 && y + dy < mmap_SIZE - 1) {
					if (dx * dx + dy * dy <= size * size) {
						if (rand() % 3 == 0) {
							Mp[x + dx][y + dy] = Stone;
							if (rand() % 12 == 0) {
								Mp[x + dx][y + dy] = Iron;
							}
						}
					}
				}
			}
		}
	}
#endif
#ifndef VILLAGE
	// 生成村庄
	int villageX = rand() % (MAP_SIZE - 20) + 10;
	int villageY = rand() % (MAP_SIZE - 20) + 10;
	for (int dx = -2; dx <= 2; dx++) {
		for (int dy = -2; dy <= 2; dy++) {
			if (villageX + dx >= 0 && villageX + dx < MAP_SIZE && villageY + dy >= 0 && villageY + dy < MAP_SIZE) {
				if (abs(dx) == 2 || abs(dy) == 2) {
					Mp[villageX + dx][villageY + dy] = House;
				}
			}
		}
	}
	Mp[villageX][villageY] = NPC;
#endif
	int cnt = 0;
	map<char, int>mp;
	int diglev = 0;
	int x = 50, y = 50;
	while (true) {
		for (int i = x - 10 ; i <= x + 10 ; i ++ ) {
			for (int j = y - 10 ; j <= y + 10 ; j ++) {
				if (i < 0 || i >= mmap_SIZE || j < 0 || j >= mmap_SIZE) {
					put(SIDE);
				} else if (i != x || j != y) {
					put(Mp[i][j]);
				} else {
					put(Person);
				}
			}
			printf("\n");
		}
		int c = _getch();
		int xx = x, yy = y;
		if (c == 'W' || c == 'w')x --;
		else if (c == 'A' || c == 'a')y --;
		else if (c == 'S' || c == 's')x ++;
		else if (c == 'D' || c == 'd')y ++;
		cnt = (++cnt) % 100;
		if (!Mp[x][y].access || x < 0 || x >= mmap_SIZE || y < 0 || y >= mmap_SIZE) {
			x = xx, y = yy;
		}
		if (c == 'G' || c == 'g') {
			if (diglev >= Mp[x][y + 1].digable) {
				if (Mp[x][y + 1] == Stone) {
					mp[Stone.name]++;
					Mp[x][y + 1] = Grass;
				}
				if (Mp[x][y + 1] == Ice) {
					Mp[x][y + 1] = Water;
				}
				if ( Mp[x][y + 1] == Iron) {
					mp[Iron.name] ++;
					Mp[x][y + 1] = Stone;
				}
			}
		}
		if(c == 't' || c == 'T'){
			if(Mp[x][y + 1] == NPC){
				recog::said == "Hello!";
			}
		}
		recog::print();
		system("cls");
	}
	return 0;
}




