#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

#define cout YOU_CANNOT_USE_IT
#define MAP_WIDTH 100
#define MAP_HEIGHT 60
#define VIEW_WIDTH 40
#define VIEW_HEIGHT 20
#define GAME_SPEED 50

enum COLORS {
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY,
	DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
};

void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

#define put(a) setColor((a).color), printf("%c", (a).name);

struct Block {
	int color;
	char name;
	bool solid;
	bool diggable;
	bool placeable;

	Block() {}
	Block(int a, char b, bool c, bool d, bool e = true) :
		color(a), name(b), solid(c), diggable(d), placeable(e) {}

	bool operator==(const Block& other) const {
		return name == other.name && color == other.color;
	}

	bool operator!=(const Block& other) const {
		return !(*this == other);
	}
};

namespace Dialog {
	string message;
	void show() {
		setColor(WHITE);
		printf("%s", message.c_str());
	}
}

// 方块类型定义
const Block AIR(BLACK, ' ', false, false, true);
const Block GRASS(GREEN, 'G', true, true, true);
const Block LEAF(GREEN, 'O', true, true, true);
const Block DIRT(BROWN, 'D', true, true, true);
const Block STONE(LIGHTGRAY, 'S', true, true, true);
const Block WATER(LIGHTBLUE, '#', false, false, true);
const Block PLAYER(WHITE, 'P', true, false, false);
const Block IRON(WHITE, '+', true, true, true);
const Block WOOD(BROWN, 'H', true, true, true);
const Block NPC(LIGHTRED, 'N', true, false, false);
const Block HOUSE(BROWN, '[', true, false, false);
const Block BORDER(LIGHTBLUE, 'X', true, false, false);

Block world[MAP_WIDTH][MAP_HEIGHT];
int heightMap[MAP_WIDTH];
bool needRedraw = true;

// 生成高度图（地面在底部）
void generateHeightMap() {
	heightMap[0] = MAP_HEIGHT - 5; // 地面从底部开始
	for (int x = 1; x < MAP_WIDTH; x++) {
		heightMap[x] = heightMap[x - 1] + (rand() % 3 - 1);
		heightMap[x] = max(MAP_HEIGHT - 10, min(MAP_HEIGHT - 1, heightMap[x]));
	}

	for (int i = 0; i < 5; i++) {
		for (int x = 1; x < MAP_WIDTH - 1; x++) {
			heightMap[x] = (heightMap[x - 1] + heightMap[x] + heightMap[x + 1]) / 3;
		}
	}
}

// 生成世界（上下颠倒）
void generateWorld() {
	generateHeightMap();

	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			// y越大越靠近底部（地面）
			if (y < heightMap[x]) {
				world[x][y] = AIR; // 上方是空气
			} else if (y <= heightMap[x]) {
				world[x][y] = GRASS; // 地表是草地
				if (rand() % 20 == 0) {

					for (int dx = -2; dx <= 2; dx++) {
						for (int dy = -2; dy <= 0; dy++) {
							if (x + dx >= 0 && x + dx < MAP_WIDTH && y - 3 + dy >= 0) {
								world[x + dx][y - 3 + dy] = LEAF;
							}
						}
					}
					world[x - 1][y - 1] = WOOD;
					world[x - 1][y - 2] = WOOD;
				}
			} else if (y <= heightMap[x] + 4) {
				world[x][y] = DIRT; // 地下浅层是泥土
			} else {
				world[x][y] = (rand() % 20 == 0) ? IRON : STONE; // 深层是石头和铁矿
			}
		}
	}


//	// 添加洞穴（现在是向上的洞穴）
//	for (int i = 0; i < 10; i++) {
//		int caveX = rand() % (MAP_WIDTH - 10) + 5;
//		int caveY = rand() % (MAP_HEIGHT - heightMap[caveX]) + heightMap[caveX];
//		int size = rand() % 5 + 3;
//
//		for (int dx = -size; dx <= size; dx++) {
//			for (int dy = -size; dy <= size; dy++) {
//				if (caveX+dx >= 0 && caveX+dx < MAP_WIDTH &&
//					caveY+dy >= 0 && caveY+dy < MAP_HEIGHT) {
//					if (dx*dx + dy*dy <= size*size) {
//  						world[caveX+dx][caveY+dy] = AIR;
//					}
//				}
//			}
//		}
//	}

	// 添加水池（现在是从地面向下）
	for (int i = 0; i < 5; i++) {
		int poolX = rand() % (MAP_WIDTH - 10) + 5;
		int poolY = heightMap[poolX] + 1;
		int width = rand() % 5 + 3;
		int depth = rand() % 3 + 2;

		for (int dx = -width; dx <= width; dx++) {
			for (int dy = 0; dy <= depth; dy++) {
				if (poolX + dx >= 0 && poolX + dx < MAP_WIDTH &&
				    poolY + dy >= 0 && poolY + dy < MAP_HEIGHT) {
					world[poolX + dx][poolY + dy] = WATER;
				}
			}
		}
	}
	// 添加NPC房屋（现在在地面上）
	int houseX = MAP_WIDTH / 2;
	int houseY = heightMap[houseX] - 1;
	for (int dx = -3; dx <= 3; dx++) {
		for (int dy = -4; dy <= 0; dy++) {
			if (houseX + dx >= 0 && houseX + dx < MAP_WIDTH &&
			    houseY + dy >= 0 && houseY + dy < MAP_HEIGHT) {
				if (dx == 0 && dy == 0) {
					world[houseX + dx][houseY + dy] = NPC;
				} else if (abs(dx) == 3 || dy == -4) {
					world[houseX + dx][houseY + dy] = HOUSE;
				} else {
					world[houseX + dx][houseY + dy] = AIR;
				}
			}
		}
	}
	world[houseX - 3][houseY] = AIR;
}

// 渲染游戏画面
void renderGame(int px, int py) {
	if (!needRedraw) return;

	system("cls");

	// 计算视野范围
	int startX = max(0, px - VIEW_WIDTH / 2);
	int endX = min(MAP_WIDTH, px + VIEW_WIDTH / 2);
	int startY = max(0, py - VIEW_HEIGHT / 2);
	int endY = min(MAP_HEIGHT, py + VIEW_HEIGHT / 2);

	// 从顶部到底部渲染
	for (int y = startY; y < endY; y++) {
		for (int x = startX; x < endX; x++) {
			if (x == px && y == py) {
				put(PLAYER);
			} else if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
				put(BORDER);
			} else {
				put(world[x][y]);
			}
		}
		printf("\n");
	}

	// 显示状态信息
	setColor(WHITE);
	printf("坐标: (%d, %d)\n", px, py);
	printf("移动: WASD\n");
	printf("跳跃: 空格(向上跳)\n");
	printf("挖掘: Shift+W(上)/A(左)/S(下)/D(右)\n");
	printf("放置: Ctrl+W(上)/A(左)/S(下)/D(右)\n");
	printf("切换方块: B\n");
	Dialog::show();

	needRedraw = false;
}

int main() {
	srand(time(0));
	generateWorld();

	// 设置玩家初始位置在地面上方
	int playerX = MAP_WIDTH / 2;
	int playerY = heightMap[playerX] - 1;

	// 确保玩家不会生成在固体方块中
	if (world[playerX][playerY].solid) {
		// 向上寻找第一个非固体位置
		while (playerY >= 0 && world[playerX][playerY].solid) {
			playerY--;
		}
		if (playerY < 0) {
			// 如果找不到合适位置，强制清空
			playerY = heightMap[playerX] - 1;
			world[playerX][playerY] = AIR;
		}
	}

	bool isJumping = false;
	int jumpHeight = 0;
	const int MAX_JUMP = 3;
	bool onGround = (playerY + 1 < MAP_HEIGHT) && world[playerX][playerY + 1].solid;

	// 可放置的方块列表
	vector<Block> placeableBlocks = {DIRT, STONE, GRASS, WOOD};
	size_t currentBlock = 0;

	// 隐藏光标
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);

	while (true) {
		// 控制游戏速度
		Sleep(GAME_SPEED);

		// 处理输入
		if (_kbhit()) {
			int input = _getch();
			int newX = playerX;
			int newY = playerY;
			bool moved = false;

			// 处理移动输入
			if (input == 'a' || input == 'A') {
				newX--;
				moved = true;
			}
			if (input == 'd' || input == 'D') {
				newX++;
				moved = true;
			}
			if (input == 'w' || input == 'W') {
				newY--;
				moved = true;
			}
			if (input == 's' || input == 'S') {
				newY++;
				moved = true;
			}

			// 处理跳跃(向上)
			if (input == ' ' && onGround) {
				isJumping = true;
				jumpHeight = 0;
				onGround = false;
				moved = true;
			}

			// 处理挖掘 (Shift+方向键)
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
				int digX = playerX, digY = playerY;
				if (input == 'w' || input == 'W') digY--; // 挖上方
				else if (input == 'a' || input == 'A') digX--; // 挖左方
				else if (input == 's' || input == 'S') digY++; // 挖下方
				else if (input == 'd' || input == 'D') digX++; // 挖右方

				if (digX >= 0 && digX < MAP_WIDTH && digY >= 0 && digY < MAP_HEIGHT) {
					if (world[digX][digY].diggable) {
						world[digX][digY] = AIR;
						needRedraw = true;
					}
				}
			}

			// 处理放置方块 (Ctrl+方向键)
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				int placeX = playerX, placeY = playerY;
				if (input == 'w' || input == 'W') placeY--; // 上方放置
				else if (input == 'a' || input == 'A') placeX--; // 左方放置
				else if (input == 's' || input == 'S') placeY++; // 下方放置
				else if (input == 'd' || input == 'D') placeX++; // 右方放置

				if (placeX >= 0 && placeX < MAP_WIDTH && placeY >= 0 && placeY < MAP_HEIGHT) {
					if (input == 'b' || input == 'B') {
						currentBlock = (currentBlock + 1) % placeableBlocks.size();
						Dialog::message = "当前方块: " + string(1, placeableBlocks[currentBlock].name);
						needRedraw = true;
					} else if (world[placeX][placeY] == AIR) {
						world[placeX][placeY] = placeableBlocks[currentBlock];
						needRedraw = true;
					}
				}
			}

			// 处理对话
			if (input == 't' || input == 'T') {
				if (abs(playerX - (MAP_WIDTH / 2)) < 3 &&
				    abs(playerY - (heightMap[MAP_WIDTH / 2] - 1)) < 2) {
					Dialog::message = "NPC: 你好，冒险者!";
				} else {
					Dialog::message = "";
				}
				needRedraw = true;
			}

			// 碰撞检测
			if (newX >= 0 && newX < MAP_WIDTH &&
			    newY >= 0 && newY < MAP_HEIGHT) {
				if (!world[newX][newY].solid) {
					if (newX != playerX || newY != playerY) {
						moved = true;
					}
					playerX = newX;
					playerY = newY;
				}
			}

			if (moved) needRedraw = true;
		}

		// 跳跃物理(向上)
		if (isJumping) {
			if (jumpHeight < MAX_JUMP &&
			    (playerY - 1 >= 0 && !world[playerX][playerY - 1].solid)) {
				playerY--;
				jumpHeight++;
				needRedraw = true;
			} else {
				isJumping = false;
			}
		}

		// 重力系统(向下)
		if (!isJumping &&
		    (playerY + 1 < MAP_HEIGHT && !world[playerX][playerY + 1].solid)) {
			playerY++;
			onGround = false;
			needRedraw = true;
		} else if (!isJumping) {
			if (!onGround) needRedraw = true;
			onGround = true;
		}

		// 确保玩家不会穿墙
		playerX = max(0, min(MAP_WIDTH - 1, playerX));
		playerY = max(0, min(MAP_HEIGHT - 1, playerY));

		// 渲染游戏
		renderGame(playerX, playerY);
	}

	return 0;
}
