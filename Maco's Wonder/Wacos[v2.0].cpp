#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

// ========== 工具函数 ==========
void SetPos(int x, int y) {
	COORD pos = { (short)y, (short)x };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void HideCursor() {
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void Color(int a) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);
}

void ClearScreen() {
	system("cls");
}

// ========== 游戏定义 ==========
#define mmap_SIZE 100
#define VIEW_SIZE 21  // 视野大小(奇数)

// 颜色定义 (前景色 | 背景色 << 4)
enum COLORS {
	BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, 
	MAGENTA = 5, BROWN = 6, LIGHTGRAY = 7, DARKGRAY = 8,
	LIGHTBLUE = 9, LIGHTGREEN = 10, LIGHTCYAN = 11, 
	LIGHTRED = 12, LIGHTMAGENTA = 13, YELLOW = 14, WHITE = 15,
	
	// 带背景色的组合
	GREEN_BG = (GREEN << 4),
	BLUE_BG = (BLUE << 4),
	CYAN_BG = (CYAN << 4)
};

struct Block {
	int color;
	char name;
	bool walkable;  // 可否行走
	bool diggable;   // 可否挖掘
	int hardness;    // 挖掘难度
	
	Block() = default;
	Block(int c, char n, bool w, bool d, int h = 0) 
	: color(c), name(n), walkable(w), diggable(d), hardness(h) {}
};

// 方块定义
const Block GRASS(GREEN_BG | LIGHTGREEN, 'G', true, false);
const Block STONE(LIGHTGRAY, 'S', false, true, 3);
const Block WOOD(BROWN, 'W', true, false);
const Block WATER(BLUE_BG | LIGHTBLUE, '#', false, false);
const Block PLAYER(WHITE, 'H', true, false);
const Block ICE(CYAN_BG | LIGHTCYAN, 'I', true, true, 1);
const Block FLOWER(GREEN_BG | YELLOW, 'F', true, false);
const Block BEDROCK(BLACK, 'B', false, false);
const Block SAND(GREEN_BG | YELLOW, '~', true, false);
const Block TALL_GRASS(GREEN_BG | DARKGRAY, '|', true, false);
const Block RIVER(BLUE_BG | LIGHTBLUE, '=', false, false);
const Block OCEAN(BLUE_BG | BLUE, 'O', false, false);

Block mmap[mmap_SIZE][mmap_SIZE];
int playerX = mmap_SIZE/2, playerY = mmap_SIZE/2;
int digLevel = 1;
map<char, int> inventory;

// ========== 游戏函数 ==========
void Initmmap() {
	// 生成基础地形 - 草地
	for (int i = 0; i < mmap_SIZE; i++) {
		for (int j = 0; j < mmap_SIZE; j++) {
			if (i == 0 || j == 0 || i == mmap_SIZE-1 || j == mmap_SIZE-1) {
				mmap[i][j] = BEDROCK;
			} else {
				// 基础草地
				mmap[i][j] = GRASS;
				
				// 随机添加草地上的元素
				int r = rand() % 100;
				if (r < 5) {  // 5%几率生成高草
					mmap[i][j] = TALL_GRASS;
				} else if (r < 7) {  // 2%几率生成花
					mmap[i][j] = FLOWER;
					mmap[i][j].color = GREEN_BG | (rand() % 3 == 0 ? YELLOW : (rand() % 2 ? LIGHTRED : LIGHTMAGENTA));
				} else if (r < 10) {  // 3%几率生成沙子
					mmap[i][j] = SAND;
				}
			}
		}
	}
	
	// 生成石头矿脉
	for (int i = 0; i < 10; i++) {
		int x = rand() % (mmap_SIZE-10) + 5;
		int y = rand() % (mmap_SIZE-10) + 5;
		int size = rand() % 5 + 3;
		
		for (int dx = -size; dx <= size; dx++) {
			for (int dy = -size; dy <= size; dy++) {
				if (x+dx > 0 && x+dx < mmap_SIZE-1 && y+dy > 0 && y+dy < mmap_SIZE-1) {
					if (dx*dx + dy*dy <= size*size) {
						if (rand() % 3 == 0) {
							mmap[x+dx][y+dy] = STONE;
						}
					}
				}
			}
		}
	}
	
	// 生成树木
	for (int i = 0; i < 20; i++) {
		int x = rand() % (mmap_SIZE-10) + 5;
		int y = rand() % (mmap_SIZE-10) + 5;
		
		if (mmap[x][y].name == 'G') {
			mmap[x][y] = WOOD;
			// 周围生成高草
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (x+dx > 0 && x+dx < mmap_SIZE-1 && y+dy > 0 && y+dy < mmap_SIZE-1) {
						if (mmap[x+dx][y+dy].name == 'G') {
							mmap[x+dx][y+dy] = TALL_GRASS;
						}
					}
				}
			}
		}
	}
	
	// 生成湖泊
	for (int i = 0; i < 5; i++) {
		int x = rand() % (mmap_SIZE-10) + 5;
		int y = rand() % (mmap_SIZE-10) + 5;
		int size = rand() % 5 + 3;
		
		for (int dx = -size; dx <= size; dx++) {
			for (int dy = -size; dy <= size; dy++) {
				if (x+dx > 0 && x+dx < mmap_SIZE-1 && y+dy > 0 && y+dy < mmap_SIZE-1) {
					if (dx*dx + dy*dy <= size*size) {
						if (rand() % 4 == 0) {
							mmap[x+dx][y+dy] = ICE;
						} else {
							mmap[x+dx][y+dy] = WATER;
						}
					}
				}
			}
		}
	}
	
	// 生成河流
	for (int i = 0; i < 3; i++) {
		int x = rand() % (mmap_SIZE-20) + 10;
		int y = rand() % (mmap_SIZE-20) + 10;
		int length = rand() % 30 + 20;
		int width = rand() % 3 + 1;
		
		for (int l = 0; l < length; l++) {
			// 随机改变河流方向
			if (rand() % 10 == 0) x += rand() % 3 - 1;
			if (rand() % 10 == 0) y += rand() % 3 - 1;
			
			for (int w = -width; w <= width; w++) {
				if (x+l > 0 && x+l < mmap_SIZE-1 && y+w > 0 && y+w < mmap_SIZE-1) {
					mmap[x+l][y+w] = RIVER;
				}
			}
		}
	}
	
	// 生成海洋边缘
	for (int i = 0; i < mmap_SIZE; i++) {
		for (int j = 0; j < mmap_SIZE; j++) {
			if (i < 5 || i > mmap_SIZE-6 || j < 5 || j > mmap_SIZE-6) {
				if (mmap[i][j].name != 'B') {
					mmap[i][j] = OCEAN;
				}
			}
		}
	}
}

void DrawGame() {
	ClearScreen();
	int halfView = VIEW_SIZE / 2;
	
	// 绘制地图
	for (int i = playerX - halfView; i <= playerX + halfView; i++) {
		for (int j = playerY - halfView; j <= playerY + halfView; j++) {
			if (i >= 0 && i < mmap_SIZE && j >= 0 && j < mmap_SIZE) {
				if (i == playerX && j == playerY) {
					Color(PLAYER.color);
					putchar(PLAYER.name);
				} else {
					Color(mmap[i][j].color);
					putchar(mmap[i][j].name);
				}
			} else {
				Color(BLACK);
				putchar(' ');
			}
//			putchar(' ');
		}
		putchar('\n');
	}
	
	// 绘制状态栏
	Color(WHITE);
	SetPos(VIEW_SIZE + 1, 0);
	printf("位置: (%d, %d)", playerX, playerY);
	SetPos(VIEW_SIZE + 2, 0);
	printf("背包: ");
	for (auto& item : inventory) {
		printf("%c:%d ", item.first, item.second);
	}
	SetPos(VIEW_SIZE + 3, 0);
	printf("挖掘等级: %d", digLevel);
	SetPos(VIEW_SIZE + 4, 0);
	printf("当前方块: %c", mmap[playerX][playerY].name);
	SetPos(VIEW_SIZE + 5, 0);
	printf("控制: WASD移动, G挖掘(面前), U使用物品");
	SetPos(VIEW_SIZE + 6, 0);
	printf("提示: 挖掘石头(S)可提升挖掘等级");
}

void ProcessInput() {
	if (_kbhit()) {
		int c = _getch();
		int newX = playerX, newY = playerY;
		int digX = playerX, digY = playerY;
		
		switch (tolower(c)) {
			case 'w': newX--; break;
			case 'a': newY--; break;
			case 's': newX++; break;
			case 'd': newY++; break;
			case 'g':  // 挖掘面前方块
			// 确定挖掘方向
			if (newX != playerX || newY != playerY) {
				digX = newX;
				digY = newY;
			} else {
				// 默认挖掘下方
				digX = playerX + 1;
				digY = playerY;
			}
			
			// 执行挖掘
			if (digX >= 0 && digX < mmap_SIZE && digY >= 0 && digY < mmap_SIZE) {
				if (mmap[digX][digY].diggable && digLevel >= mmap[digX][digY].hardness) {
					inventory[mmap[digX][digY].name]++;
					mmap[digX][digY] = GRASS; // 挖掉后变成草地
				}
			}
			break;
			case 'u':  // 使用物品
			if (!inventory.empty()) {
				char item = inventory.begin()->first;
				if (inventory[item] > 0) {
					inventory[item]--;
					// 物品效果
					if (item == 'S') digLevel++; // 石头可以提升挖掘等级
				}
			}
			break;
			case 'q':  // 退出游戏
			exit(0);
			break;
		}
		
		// 移动检测
		if (newX != playerX || newY != playerY) {
			if (newX >= 0 && newX < mmap_SIZE && newY >= 0 && newY < mmap_SIZE) {
				if (mmap[newX][newY].walkable) {
					playerX = newX;
					playerY = newY;
				}
			}
		}
		
		// 每次输入后重绘
		DrawGame();
	}
}

// ========== 主函数 ==========
int main() {
	// 初始化
	system("mode con cols=60 lines=30");
	HideCursor();
	srand(time(0));
	Initmmap();
	
	// 游戏开始提示
	Color(YELLOW);
	ClearScreen();
	SetPos(5, 10);
	printf("=== 方块世界 ===");
	SetPos(7, 10);
	printf("按任意键开始游戏");
	_getch();
	
	// 游戏主循环
	DrawGame();
	while (true) {
		ProcessInput();
		Sleep(50);  // 控制游戏速度
	}
	
	return 0;
}
