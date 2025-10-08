#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

#define cout YOU_CANNOT_USE_IT
#define MAP_WIDTH 200
#define MAP_HEIGHT 100
#define VIEW_WIDTH 40
#define VIEW_HEIGHT 20
#define GAME_SPEED 50
#define INVENTORY_SIZE 15

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
	bool canFlow;
	int flowDelay;
	string itemName;
	int durability; // 耐久度，挖掘所需时间

	Block() {}
	Block(int a, char b, bool c, bool d, bool e = true, bool f = false,
	      int g = 0, string in = "", int dur = 1) :
		color(a), name(b), solid(c), diggable(d), placeable(e),
		canFlow(f), flowDelay(g), itemName(in), durability(dur) {}

	bool operator==(const Block& other) const {
		return name == other.name && color == other.color;
	}

	bool operator!=(const Block& other) const {
		return !(*this == other);
	}
};

// 物品结构体
struct Item {
	string name;
	int amount;
	Block blockType;

	Item(string n, int a, Block b) : name(n), amount(a), blockType(b) {}
};

// 任务结构体
struct Quest {
	string title;
	string description;
	bool completed;
	string requiredItem;
	int requiredAmount;
	string rewardItem;
	int rewardAmount;

	Quest(string t, string d, string ri, int ra, string rwi = "", int rwa = 0) :
		title(t), description(d), completed(false), requiredItem(ri), requiredAmount(ra),
		rewardItem(rwi), rewardAmount(rwa) {}
};

bool needRedraw = true;

namespace Dialog {
	string message;
	vector<string> npcDialogs;
	int currentDialog = 0;

	void show() {
		setColor(WHITE);
		printf("%s", message.c_str());
	}

	void showNpcDialog() {
		if (currentDialog < npcDialogs.size()) {
			message = "NPC: " + npcDialogs[currentDialog];
			needRedraw = true;
		}
	}

	void nextDialog() {
		currentDialog = (currentDialog + 1) % npcDialogs.size();
		showNpcDialog();
	}
}

// 方块类型定义
const Block AIR(BLACK, ' ', false, false, true);
const Block GRASS(GREEN, 'G', true, true, true, false, 0, "草方块", 1);
const Block DIRT(BROWN, 'D', true, true, true, false, 0, "泥土", 1);
const Block STONE(LIGHTGRAY, 'S', true, true, true, false, 0, "石头", 3);
const Block WATER(LIGHTBLUE, '@', false, false, true, true, 3);
const Block PLAYER(WHITE, 'P', true, false, false);
const Block IRON(WHITE, '+', true, true, true, false, 0, "铁矿石", 5);
const Block WOOD(BROWN, 'H', true, true, true, false, 0, "木头", 2);
const Block NPC(LIGHTRED, 'N', true, false, false);
const Block HOUSE(BROWN, '[', true, false, false);
const Block BORDER(LIGHTBLUE, 'X', true, false, false);
const Block LEAVES(LIGHTGREEN, '%', true, true, true, false, 0, "树叶", 1);
const Block SAND(YELLOW, 'E', true, true, true, false, 0, "沙子", 1);
const Block GOLD(YELLOW, '$', true, true, true, false, 0, "金矿石", 7);
const Block DIAMOND(LIGHTCYAN, 'H', true, true, true, false, 0, "钻石", 10);
const Block GLASS(WHITE, 'Q', true, true, true, false, 0, "玻璃", 1);
const Block BRICK(RED, 'B', true, true, true, false, 0, "砖块", 2);
const Block FLOWER(RED, 'F', false, true, true, false, 0, "花", 1);
const Block LAVA(LIGHTRED, 'L', false, false, true, true, 5, "", 0);
const Block CACTUS(GREEN, '#', true, true, true, false, 0, "仙人掌", 1);
const Block COAL(DARKGRAY, '&', true, true, true, false, 0, "煤炭", 2);

Block world[MAP_WIDTH][MAP_HEIGHT];
int heightMap[MAP_WIDTH];

// 玩家数据
vector<Item> inventory;
vector<Quest> quests;
Quest currentQuest("", "", "", 0);
bool hasQuest = false;
int miningProgress = 0;
Block currentMiningBlock = AIR;
int miningX = 0, miningY = 0;

// 初始化NPC对话
void initNpcDialogs() {
	Dialog::npcDialogs = {
		"欢迎来到方块世界！",
		"这里有很多资源可以收集。",
		"完成任务可以获得稀有物品！",
		"小心熔岩，它会破坏你的物品！",
		"沙漠中有特殊的植物和矿物。"
	};
}

// 初始化任务
void initQuests() {
	quests.push_back(Quest("初级矿工", "收集20个石头", "石头", 20, "铁矿石", 3));
	quests.push_back(Quest("木材收集", "收集10个木头", "木头", 10, "煤炭", 5));
	quests.push_back(Quest("高级矿工", "收集3个钻石", "钻石", 3, "金矿石", 5));
	quests.push_back(Quest("建筑大师", "收集15个砖块", "砖块", 15, "玻璃", 10));
}

// 检查任务完成情况
void checkQuestCompletion() {
	if (!hasQuest) return;

	for (auto& item : inventory) {
		if (item.name == currentQuest.requiredItem && item.amount >= currentQuest.requiredAmount) {
			currentQuest.completed = true;
			break;
		}
	}
}

// 添加物品到物品栏
void addToInventory(string itemName) {
	for (auto& item : inventory) {
		if (item.name == itemName) {
			item.amount++;
			return;
		}
	}

	if (inventory.size() < INVENTORY_SIZE) {
		if (itemName == "草方块") inventory.push_back(Item(itemName, 1, GRASS));
		else if (itemName == "泥土") inventory.push_back(Item(itemName, 1, DIRT));
		else if (itemName == "石头") inventory.push_back(Item(itemName, 1, STONE));
		else if (itemName == "铁矿石") inventory.push_back(Item(itemName, 1, IRON));
		else if (itemName == "木头") inventory.push_back(Item(itemName, 1, WOOD));
		else if (itemName == "树叶") inventory.push_back(Item(itemName, 1, LEAVES));
		else if (itemName == "沙子") inventory.push_back(Item(itemName, 1, SAND));
		else if (itemName == "金矿石") inventory.push_back(Item(itemName, 1, GOLD));
		else if (itemName == "钻石") inventory.push_back(Item(itemName, 1, DIAMOND));
		else if (itemName == "玻璃") inventory.push_back(Item(itemName, 1, GLASS));
		else if (itemName == "砖块") inventory.push_back(Item(itemName, 1, BRICK));
		else if (itemName == "花") inventory.push_back(Item(itemName, 1, FLOWER));
		else if (itemName == "仙人掌") inventory.push_back(Item(itemName, 1, CACTUS));
		else if (itemName == "煤炭") inventory.push_back(Item(itemName, 1, COAL));
	}
}

// 生成生物群系
void generateBiomes() {
	// 生成沙漠区域
	int desertStart = rand() % (MAP_WIDTH - 50);
	int desertEnd = desertStart + 30 + rand() % 40;

	for (int x = desertStart; x < desertEnd && x < MAP_WIDTH; x++) {
		for (int y = max(0, heightMap[x] - 3); y < MAP_HEIGHT; y++) {
			if (y == heightMap[x]) {
				world[x][y] = SAND;
			} else if (y > heightMap[x] && y < heightMap[x] + 3) {
				world[x][y] = SAND;

				// 生成仙人掌
				if (rand() % 50 == 0 && x > 0 && x < MAP_WIDTH - 1) {
					int cactusHeight = 1 + rand() % 3;
					for (int cy = y; cy > y - cactusHeight && cy >= 0; cy--) {
						world[x][cy] = CACTUS;
					}
				}
			}
		}

		// 沙漠中的水池改为小绿洲
		if (rand() % 100 == 0) {
			int poolX = x + rand() % 10;
			int poolY = heightMap[poolX] + 1;
			int size = 2 + rand() % 3;

			for (int dx = -size; dx <= size; dx++) {
				for (int dy = 0; dy <= size; dy++) {
					if (poolX + dx >= 0 && poolX + dx < MAP_WIDTH &&
					    poolY + dy >= 0 && poolY + dy < MAP_HEIGHT) {
						world[poolX + dx][poolY + dy] = WATER;

						// 在水边生成花
						if (dy == 0 && rand() % 2 == 0) {
							if (poolX + dx + 1 < MAP_WIDTH && world[poolX + dx + 1][poolY].name == ' ') {
								world[poolX + dx + 1][poolY] = FLOWER;
							}
							if (poolX + dx - 1 >= 0 && world[poolX + dx - 1][poolY].name == ' ') {
								world[poolX + dx - 1][poolY] = FLOWER;
							}
						}
					}
				}
			}
		}
	}

	// 生成矿脉
	for (int i = 0; i < 10; i++) {
		int veinX = rand() % MAP_WIDTH;
		int veinY = heightMap[veinX] + 5 + rand() % (MAP_HEIGHT - heightMap[veinX] - 5);
		int size = 3 + rand() % 5;
		Block veinType;

		switch (rand() % 4) {
			case 0:
				veinType = IRON;
				break;
			case 1:
				veinType = COAL;
				break;
			case 2:
				veinType = GOLD;
				break;
			case 3:
				veinType = DIAMOND;
				break;
		}

		for (int dx = -size; dx <= size; dx++) {
			for (int dy = -size; dy <= size; dy++) {
				if (veinX + dx >= 0 && veinX + dx < MAP_WIDTH &&
				    veinY + dy >= 0 && veinY + dy < MAP_HEIGHT) {
					if (dx * dx + dy * dy <= size * size && world[veinX + dx][veinY + dy] == STONE) {
						world[veinX + dx][veinY + dy] = veinType;
					}
				}
			}
		}
	}

	// 生成熔岩池
	for (int i = 0; i < 3; i++) {
		int lavaX = rand() % (MAP_WIDTH - 10) + 5;
		int lavaY = heightMap[lavaX] + 3 + rand() % 10;
		int size = 2 + rand() % 3;

		for (int dx = -size; dx <= size; dx++) {
			for (int dy = -size; dy <= size; dy++) {
				if (lavaX + dx >= 0 && lavaX + dx < MAP_WIDTH &&
				    lavaY + dy >= 0 && lavaY + dy < MAP_HEIGHT) {
					if (dx * dx + dy * dy <= size * size &&
					    (world[lavaX + dx][lavaY + dy] == STONE ||
					     world[lavaX + dx][lavaY + dy] == DIRT)) {
						world[lavaX + dx][lavaY + dy] = LAVA;
					}
				}
			}
		}
	}
}

// 生成高度图
void generateHeightMap() {
	heightMap[0] = MAP_HEIGHT - 5;
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

// 生成世界
void generateWorld() {
	generateHeightMap();

	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			if (y < heightMap[x]) {
				world[x][y] = AIR;
			} else if (y <= heightMap[x]) {
				world[x][y] = GRASS;
				if (rand() % 20 == 0) {
					int treeHeight = rand() % 3 + 3;
					int treeBaseY = y;
							
					int treeTopY = y - treeHeight;
					for (int ty = treeBaseY; ty >= treeTopY; ty--) {
						if (ty >= 0) {
							world[x][ty] = WOOD;
						}
					}

					for (int ly = treeTopY; ly >= treeTopY - 2; ly--) {
						if (ly < 0) continue;

						int radius = 1;
						if (ly == treeTopY - 1) radius = 2;

						for (int dx = -radius; dx <= radius; dx++) {
							for (int dy = -1; dy <= 1; dy++) {
//								if (abs(dx) + abs(dy) <= radius) {
									int leafX = x + dx;
									int leafY = ly + dy;

									if (leafX >= 0 && leafX < MAP_WIDTH &&
									    leafY >= 0 && leafY < MAP_HEIGHT &&
									    world[leafX][leafY] == AIR) {
										world[leafX][leafY] = LEAVES;
									}
//								}
							}
						}
					}

					if (treeTopY - 3 >= 0 && world[x][treeTopY - 3] == AIR) {
						world[x][treeTopY - 3] = LEAVES;
					}
				}
			} else if (y <= heightMap[x] + 4) {
				world[x][y] = DIRT;
			} else {
				world[x][y] = STONE;
			}
		}
	}

	// 生成特殊生物群系
	generateBiomes();

	// 添加NPC房屋
	int houseX = MAP_WIDTH / 2;
	int houseY = heightMap[houseX] - 1;

	// 用砖块建造房屋
	for (int dx = -3; dx <= 3; dx++) {
		for (int dy = -4; dy <= 0; dy++) {
			if (houseX + dx >= 0 && houseX + dx < MAP_WIDTH &&
			    houseY + dy >= 0 && houseY + dy < MAP_HEIGHT) {
				if (dx == 0 && dy == 0) {
					world[houseX + dx][houseY + dy] = NPC;
				} else if (abs(dx) == 3 || dy == -4) {
					world[houseX + dx][houseY + dy] = BRICK;
				} else if (dy == -3 && abs(dx) <= 1) {
					world[houseX + dx][houseY + dy] = GLASS; // 窗户
				} else {
					world[houseX + dx][houseY + dy] = AIR;
				}
			}
		}
	}
	world[houseX - 3][houseY] = AIR; // 门
	world[houseX - 3][houseY - 1] = AIR;
}

// 更新熔岩流动
void updateLavaFlow() {
	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = MAP_HEIGHT - 1; y >= 0; y--) {
			if (world[x][y].name == 'L' && world[x][y].canFlow) {
				if (world[x][y].flowDelay > 0) {
					world[x][y].flowDelay--;
					continue;
				}

				world[x][y].flowDelay = 5; // 熔岩流动比水慢

				// 向下流动
				if (y + 1 < MAP_HEIGHT && world[x][y + 1].name == ' ') {
					world[x][y + 1] = LAVA;
					world[x][y] = AIR;
					needRedraw = true;
					continue;
				}

				// 熔岩可以烧毁某些方块
				for (int dx = -1; dx <= 1; dx++) {
					for (int dy = -1; dy <= 1; dy++) {
						if (x + dx >= 0 && x + dx < MAP_WIDTH && y + dy >= 0 && y + dy < MAP_HEIGHT) {
							if (world[x + dx][y + dy].name == 'H' || // 木头
							    world[x + dx][y + dy].name == 'F' || // 花
							    world[x + dx][y + dy].name == '%') { // 树叶
								world[x + dx][y + dy] = AIR;
								needRedraw = true;
							}
						}
					}
				}

				// 水平扩散
				if (x > 0 && world[x - 1][y].name == ' ' &&
				    (y == MAP_HEIGHT - 1 || world[x - 1][y + 1].solid)) {
					world[x - 1][y] = LAVA;
					world[x][y] = AIR;
					needRedraw = true;
					continue;
				}

				if (x < MAP_WIDTH - 1 && world[x + 1][y].name == ' ' &&
				    (y == MAP_HEIGHT - 1 || world[x + 1][y + 1].solid)) {
					world[x + 1][y] = LAVA;
					world[x][y] = AIR;
					needRedraw = true;
				}
			}
		}
	}
}

// 水流动更新
void updateWaterFlow() {
	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = MAP_HEIGHT - 1; y >= 0; y--) {
			if (world[x][y].name == '#' && world[x][y].canFlow) {
				if (world[x][y].flowDelay > 0) {
					world[x][y].flowDelay--;
					continue;
				}

				world[x][y].flowDelay = 3;

				// 向下流动
				if (y + 1 < MAP_HEIGHT && world[x][y + 1].name == ' ') {
					world[x][y + 1] = WATER;
					world[x][y] = AIR;
					needRedraw = true;
					continue;
				}

				// 向左下方流动
				if (x > 0 && y + 1 < MAP_HEIGHT &&
				    world[x - 1][y + 1].name == ' ' && world[x - 1][y].name == ' ') {
					world[x - 1][y] = WATER;
					world[x][y] = AIR;
					needRedraw = true;
					continue;
				}

				// 向右下方流动
				if (x < MAP_WIDTH - 1 && y + 1 < MAP_HEIGHT &&
				    world[x + 1][y + 1].name == ' ' && world[x + 1][y].name == ' ') {
					world[x + 1][y] = WATER;
					world[x][y] = AIR;
					needRedraw = true;
					continue;
				}

				// 向左扩散
				if (x > 0 && world[x - 1][y].name == ' ' &&
				    (y == MAP_HEIGHT - 1 || world[x - 1][y + 1].solid)) {
					world[x - 1][y] = WATER;
					world[x][y] = AIR;
					needRedraw = true;
					continue;
				}

				// 向右扩散
				if (x < MAP_WIDTH - 1 && world[x + 1][y].name == ' ' &&
				    (y == MAP_HEIGHT - 1 || world[x + 1][y + 1].solid)) {
					world[x + 1][y] = WATER;
					world[x][y] = AIR;
					needRedraw = true;
				}
			}
		}
	}
}

// 渲染游戏画面
void renderGame(int px, int py) {
	if (!needRedraw) return;

	system("cls");

	int startX = max(0, px - VIEW_WIDTH / 2);
	int endX = min(MAP_WIDTH, px + VIEW_WIDTH / 2);
	int startY = max(0, py - VIEW_HEIGHT / 2);
	int endY = min(MAP_HEIGHT, py + VIEW_HEIGHT / 2);

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

	setColor(WHITE);
	printf("坐标: (%d, %d)\n", px, py);
	printf("移动: WASD  跳跃: 空格  挖掘: Shift+方向键(按住)\n");
	printf("放置: Ctrl+方向键  切换方块: B  对话: T\n");
	printf("接受任务: E  提交任务: R  下一个对话: N\n");

	// 显示物品栏
	printf("\n物品栏(%d/%d):\n", inventory.size(), INVENTORY_SIZE);
	for (size_t i = 0; i < inventory.size(); i++) {
		printf("%d. %s x%d\n", i + 1, inventory[i].name.c_str(), inventory[i].amount);
	}

	// 显示当前任务
	if (hasQuest) {
		printf("\n当前任务: %s\n", currentQuest.title.c_str());
		printf("任务描述: %s\n", currentQuest.description.c_str());
		printf("奖励: %d x %s\n", currentQuest.rewardAmount, currentQuest.rewardItem.c_str());
		printf("进度: ");

		int collected = 0;
		for (auto& item : inventory) {
			if (item.name == currentQuest.requiredItem) {
				collected = item.amount;
				break;
			}
		}

		printf("%d/%d %s", min(collected, currentQuest.requiredAmount),
		       currentQuest.requiredAmount, currentQuest.requiredItem.c_str());

		if (currentQuest.completed) {
			printf(" (已完成)");
		}
		printf("\n");
	}

	// 显示挖掘进度
	if (miningProgress > 0) {
		printf("\n挖掘进度: [");
		int progress = (miningProgress * 10) / currentMiningBlock.durability;
		for (int i = 0; i < 10; i++) {
			if (i < progress) printf("#");
			else printf(" ");
		}
		printf("] %d%%\n", (miningProgress * 100) / currentMiningBlock.durability);
	}

	Dialog::show();
	needRedraw = false;
}

int main() {
	srand(time(0));
	generateWorld();
	initNpcDialogs();
	initQuests();

	int playerX = MAP_WIDTH / 2;
	int playerY = heightMap[playerX] - 1;

	if (world[playerX][playerY].solid) {
		while (playerY >= 0 && world[playerX][playerY].solid) {
			playerY--;
		}
		if (playerY < 0) {
			playerY = heightMap[playerX] - 1;
			world[playerX][playerY] = AIR;
		}
	}

	bool isJumping = false;
	int jumpHeight = 0;
	const int MAX_JUMP = 3;
	bool onGround = (playerY + 1 < MAP_HEIGHT) && world[playerX][playerY + 1].solid;

	vector<Block> placeableBlocks = {DIRT, STONE, GRASS, WOOD, LEAVES, SAND, BRICK, GLASS, CACTUS};
	size_t currentBlock = 0;

	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);

	int waterUpdateCounter = 0;
	int lavaUpdateCounter = 0;

	while (true) {
		Sleep(GAME_SPEED);

		// 更新水流动
		if (waterUpdateCounter++ >= 5) {
			updateWaterFlow();
			waterUpdateCounter = 0;
		}

		// 更新熔岩流动
		if (lavaUpdateCounter++ >= 10) {
			updateLavaFlow();
			lavaUpdateCounter = 0;
		}

		if (_kbhit()) {
			int input = _getch();
			int newX = playerX;
			int newY = playerY;
			bool moved = false;

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

			if (input == ' ' && onGround) {
				isJumping = true;
				jumpHeight = 0;
				onGround = false;
				moved = true;
			}

			// 挖掘方块
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
				int digX = playerX, digY = playerY;
				if (input == 'w' || input == 'W') digY--;
				else if (input == 'a' || input == 'A') digX--;
				else if (input == 's' || input == 'S') digY++;
				else if (input == 'd' || input == 'D') digX++;

				if (digX >= 0 && digX < MAP_WIDTH && digY >= 0 && digY < MAP_HEIGHT) {
					if (world[digX][digY].diggable) {
						// 开始或继续挖掘
						if (world[digX][digY] != currentMiningBlock ||
						    digX != miningX || digY != miningY) {
							miningProgress = 0;
							currentMiningBlock = world[digX][digY];
							miningX = digX;
							miningY = digY;
						}

						miningProgress++;
						if (miningProgress >= currentMiningBlock.durability) {
							// 挖掘完成
							if (!world[digX][digY].itemName.empty()) {
								addToInventory(world[digX][digY].itemName);
							}
							world[digX][digY] = AIR;
							miningProgress = 0;
							checkQuestCompletion();
						}
						needRedraw = true;
					}
				}
			} else {
				miningProgress = 0; // 松开Shift键重置挖掘进度
			}

			// 放置方块
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				int placeX = playerX, placeY = playerY;
				if (input == 'w' || input == 'W') placeY--;
				else if (input == 'a' || input == 'A') placeX--;
				else if (input == 's' || input == 'S') placeY++;
				else if (input == 'd' || input == 'D') placeX++;

				if (input == 'b' || input == 'B') {
					currentBlock = (currentBlock + 1) % placeableBlocks.size();
					Dialog::message = "当前方块: " + placeableBlocks[currentBlock].itemName;
					needRedraw = true;
				} else if (placeX >= 0 && placeX < MAP_WIDTH &&
				           placeY >= 0 && placeY < MAP_HEIGHT &&
				           world[placeX][placeY] == AIR) {
					// 检查物品栏中是否有该方块
					bool hasItem = false;
					for (auto& item : inventory) {
						if (item.blockType == placeableBlocks[currentBlock]) {
							hasItem = true;
							item.amount--;
							if (item.amount <= 0) {
								inventory.erase(remove_if(inventory.begin(), inventory.end(),
								[&](const Item & i) {
									return i.name == item.name && i.amount <= 0;
								}),
								inventory.end());
							}
							break;
						}
					}

					if (hasItem) {
						world[placeX][placeY] = placeableBlocks[currentBlock];
						needRedraw = true;
					} else {
						Dialog::message = "物品栏中没有这种方块！";
						needRedraw = true;
					}
				}
			}

			// 对话交互
			if (input == 't' || input == 'T') {
				if (abs(playerX - (MAP_WIDTH / 2)) < 3 &&
				    abs(playerY - (heightMap[MAP_WIDTH / 2] - 1)) < 2) {
					Dialog::showNpcDialog();
				} else {
					Dialog::message = "附近没有NPC可以对话";
				}
				needRedraw = true;
			}

			// 下一个对话
			if (input == 'n' || input == 'N') {
				Dialog::nextDialog();
			}

			// 接受任务
			if (input == 'e' || input == 'E') {
				if (abs(playerX - (MAP_WIDTH / 2)) < 3 &&
				    abs(playerY - (heightMap[MAP_WIDTH / 2] - 1)) < 2) {
					if (!hasQuest) {
						currentQuest = quests[rand() % quests.size()];
						hasQuest = true;
						Dialog::message = "已接受任务: " + currentQuest.title + "\n" +
						                  "任务描述: " + currentQuest.description + "\n" +
						                  "奖励: " + to_string(currentQuest.rewardAmount) + " x " + currentQuest.rewardItem;
					} else {
						Dialog::message = "你已经有一个任务了";
					}
					needRedraw = true;
				}
			}

			// 提交任务
			if (input == 'r' || input == 'R') {
				if (abs(playerX - (MAP_WIDTH / 2)) < 3 &&
				    abs(playerY - (heightMap[MAP_WIDTH / 2] - 1)) < 2) {
					if (hasQuest && currentQuest.completed) {
						// 移除任务物品
						for (auto it = inventory.begin(); it != inventory.end(); ) {
							if (it->name == currentQuest.requiredItem) {
								it->amount -= currentQuest.requiredAmount;
								if (it->amount <= 0) {
									it = inventory.erase(it);
									continue;
								}
							}
							++it;
						}

						// 添加奖励物品
						if (!currentQuest.rewardItem.empty()) {
							for (int i = 0; i < currentQuest.rewardAmount; i++) {
								addToInventory(currentQuest.rewardItem);
							}
						}

						Dialog::message = "任务完成！获得奖励: " +
						                  to_string(currentQuest.rewardAmount) + " x " + currentQuest.rewardItem;
						hasQuest = false;
					} else if (hasQuest) {
						Dialog::message = "你还没有完成任务";
					} else {
						Dialog::message = "你没有任务可以提交";
					}
					needRedraw = true;
				}
			}

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

		if (!isJumping &&
		    (playerY + 1 < MAP_HEIGHT && !world[playerX][playerY + 1].solid)) {
			playerY++;
			onGround = false;
			needRedraw = true;
		} else if (!isJumping) {
			if (!onGround) needRedraw = true;
			onGround = true;
		}

		playerX = max(0, min(MAP_WIDTH - 1, playerX));
		playerY = max(0, min(MAP_HEIGHT - 1, playerY));

		renderGame(playerX, playerY);
	}

	return 0;
}
