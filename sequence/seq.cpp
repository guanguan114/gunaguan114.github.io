#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>

using namespace std;

// 设置控制台文本颜色的函数
void setColor(int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

// 显示中心对齐的文本
void printCentered(const string& text, int color = 15) {
	setColor(color);
	int width = 80;
	int padding = (width - text.length()) / 2;
	cout << string(padding, ' ') << text << endl;
	setColor(15);
}

int main() {
	srand(time(0));
	char c;
	// 时间间隔列表 - 随难度增加使用更小的值
	int baseIntervals[] = {300, 200, 175, 150, 100, 75, 50, 48, 36, 24};
	char org[] = "1234567890";
	int level = 1;
	int highestLevel = 1;
	int streak = 0;
	
	// 游戏标题
	system("cls");
	printCentered("============================", 13);
	printCentered("   SEQUENCE MEMORY CHALLENGE   ", 13);
	printCentered("============================", 13);
	printCentered("", 15);
	printCentered("Type the numbers you see in the same order", 10);
	printCentered("and within the same time intervals!", 10);
	printCentered("", 15);
	printCentered("Press any key to start...", 14);
	
	getch();
	
	while(1) {
		system("cls");
		printCentered("LEVEL " + to_string(level), 10);
		printCentered("HIGHEST: " + to_string(highestLevel), 14);
		printCentered("", 15);
		printCentered("Get ready...", 11);
		Sleep(1500);
		system("cls");
		
		string h;
		vector<int> v;
		int sequenceLength = min(30, level + 2); // 最长序列为30
		
		// 生成序列
		for(int i = 0; i < sequenceLength; i++) {
			// 难度调整：随等级和连击数减少时间间隔
			int intervalIndex = min(9, (level-1)/3 + min(3, streak/5));
			int baseInterval = baseIntervals[intervalIndex];
			
			// 每个字符的间隔有随机波动
			int interval = baseInterval - (rand() % (baseInterval/3));
			v.push_back(interval);
			
			// 使用所有20个字符
			h.push_back(org[rand() % 10]);
		}
		
		// 显示序列
		for(int i = 0; i < sequenceLength; i++) {
			Sleep(v[i]);
			
			// 根据字符设置不同颜色
			switch(h[i]) {
				case '1': setColor(15); break; // 白色
				case '2': setColor(12); break; // 红色
				case '3': setColor(10); break; // 绿色
				case '4': setColor(14); break; // 黄色
				case '5': setColor(9);  break; // 蓝色
				case '6': setColor(13); break; // 粉色
				case '7': setColor(6);  break; // 棕色
				case '8': setColor(11); break; // 青色
				case '9': setColor(2);  break; // 深绿
				case '0': setColor(5);  break;
			}
			
			printf("%c ", h[i]);
			setColor(15); // 恢复白色
		}
		
		Sleep(500);
		system("cls");
		setColor(10); // 绿色
		printf("Type the sequence: ");
		setColor(15); // 白色
		
		bool flag = 1;
		clock_t st;
		bool success = true;
		
		for(int i = 0; i < sequenceLength; i++) {
			// 根据预期字符设置颜色
			switch(h[i]) {
				case '1': setColor(15); break;
				case '2': setColor(12); break;
				case '3': setColor(10); break;
				case '4': setColor(14); break;
				case '5': setColor(9);  break;
				case '6': setColor(13); break;
				case '7': setColor(6);  break;
				case '8': setColor(11); break;
				case '9': setColor(2);  break;
				case '0': setColor(5);  break;
			}
			
			c = getch(); // 使用getch()不需要按回车
			printf("%c ", c);
			setColor(15); // 恢复白色
			
			if(flag) {
				st = clock();
				flag = 0;
			}
			
			clock_t ed = clock();
			
			// 检查字符和时间
			if(fabs((ed - st - v[i]) / CLOCKS_PER_SEC) > 0.15 || c != h[i]) {
				success = false;
				break;
			}
			
			st = ed; // 更新开始时间
		}
		
		system("cls");
		
		if(success) {
			streak++;
			level++;
			highestLevel = max(highestLevel, level);
			
			setColor(10); // 绿色
			printCentered("GREAT JOB!", 10);
			printCentered("Level up! Current level: " + to_string(level), 10);
			printCentered("Streak: " + to_string(streak), 14);
			setColor(15);
			
			Sleep(2000);
		} else {
			streak = 0;
			
			setColor(12); // 红色
			printCentered("SEQUENCE BROKEN", 12);
			printCentered("Correct sequence: " + h, 14);
			printCentered("Your score: Level " + to_string(level-1), 10);
			printCentered("Highest level: " + to_string(highestLevel), 10);
			setColor(15);
			
			Sleep(3000);
			
			// 询问是否重新开始
			system("cls");
			printCentered("Play again? (Y/N)", 14);
			
			char choice;
			do {
				choice = toupper(getch());
			} while(choice != 'Y' && choice != 'N');
			
			if(choice == 'N') {
				break;
			} else {
				level = 1;
			}
		}
	}
	
	system("cls");
	printCentered("Thanks for playing!", 13);
	printCentered("Highest level achieved: " + to_string(highestLevel), 10);
	printCentered("", 15);
	
	return 0;
}
