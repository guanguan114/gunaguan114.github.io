#include<bits/stdc++.h>
#include<windows.h>
using namespace std;

void setcolor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

enum COLORS {
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY, DARKGREEN,
	DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW
};

class corns {
	public:
		bool bought;
		int cost, doub;
		double earn;
		int color;
		string name;
		corns(int a, double b, int d, string q, int co) {
			bought = 0;
			cost = a;
			earn = b;
			doub = d;
			name = q;
			color = co;
		}
};

int main() {
#define Length 4
	corns a[] = {{0, 0.12, 2, "corn",  YELLOW}, {1, 0.53, 2, "apple", RED},
		{3, 0.98, 2, "pear", LIGHTGREEN},
		{12, 1.25, 2, "orange", LIGHTMAGENTA},
		{72, 3.03, 3, "coconut", LIGHTBLUE},
		{128, 4.47, 3, "cane", LIGHTGREEN},
		{347, 7.23, 4, "rice", LIGHTGRAY},
		{625, 11.92, 4, "tea", GREEN},
		{1032, 11.92, 4, "cotton", LIGHTRED},
	};
	int day = 1, nig = 1;
	double money = 0;

	cout << "Welcome to the Farm Game!" << endl;
	cout << "Type 'help' to see available commands.\n" << endl;

	while (1) {
		// 每日收益计算
		for (auto i : a) {
			if (i.bought) money += i.earn * nig;
		}

		cout << "Day ";
		setcolor(YELLOW);
		cout << day;
		setcolor(YELLOW);
		cout << ":\n";

		string h;
		bool flag = 1;
		while (1) {
			setcolor(YELLOW);
			cout << "Money : $";
			setcolor(GREEN);
			cout << fixed << setprecision(2) << money;
			setcolor(YELLOW);
			cout << " w    Nigger : ";
			setcolor(LIGHTCYAN);
			cout << nig << endl;
			setcolor(YELLOW);

			cout << "> ";
			cin >> h;

			if (h == "skip") break;
			else if (h == "shop" && flag) {
				cout << "\nToday's nigger:\n";
				double nig1 = (double)(rand() % 50) / 100 * money + 1;
				double nig2 = (double)(rand() % 50) / 100 * money + 1;
				double nig3 = (double)(rand() % 50) / 100 * money + 1;
				cout << "Nigger1 : $" << nig1 << " w \n" << "Nigger2 : $" << nig2 << " w \n" << "Nigger3 : $" << nig3 << " w \n";
				int i;
				cin >> i;
				if (i)cout << "success!\n";
				if (i) {
					money -= (i == 1 ? nig1 : (i == 2 ? nig2 : nig3));
					nig ++;
				}
				flag = 0;
			} else if (h == "buy") {
				cout << "\nAvailable crops:" << endl;
				for (auto i : a) {
					setcolor(i.color);
					cout << i.name << " : $" << i.cost << " w   [" << (i.bought ? ("LevelUp") : "Selling") << "]";
					if (i.bought) cout << "  Earns: $" << fixed << setprecision(2) << i.earn << "/day";
					cout << endl;
				}
				setcolor(CYAN);

				cout << "\nEnter crop name to buy: ";
				cin >> h;
				if (h == "level") {
					cin >> h;
					bool found = false;
					for (auto& i : a) {
						if (i.name == h) {
							found = true;
							if (i.bought) {
								if (money < i.cost) {
									setcolor(RED);
									cout << "Not enough money! You need $"
									     << fixed << setprecision(2) << i.cost - money << " more." << endl;
									setcolor(CYAN);
								} else {
									i.earn *= i.doub;
									i.cost *= (i.doub + 1);
									setcolor(GREEN);
									cout << "Success!!" << endl;
									setcolor(CYAN);
								}
							} else  {
								setcolor(RED);
								cout << "You didn't bought " << h << "!" << endl;
								setcolor(CYAN);
							}
							break;
						}
					}
					if (!found) {
						setcolor(RED);
						cout << "Crop not found!" << endl;
						setcolor(CYAN);
					}
				}
				bool found = false;
				for (auto& i : a) {
					if (i.name == h) {
						found = true;
						if (i.bought) {
							cout << "You already own this crop!" << endl;
						} else if (money >= i.cost) {
							money -= i.cost;
							i.bought = true;
							setcolor(GREEN);
							cout << "Successfully bought " << h << "!" << endl;
							setcolor(CYAN);
							cout << "You now earn $" << fixed << setprecision(2)
							     << i.earn << " per day from it." << endl;
						} else {
							setcolor(RED);
							cout << "Not enough money! You need $"
							     << fixed << setprecision(2) << i.cost - money << " more." << endl;
							setcolor(CYAN);
						}
						break;
					}
				}
				if (!found) {
					setcolor(RED);
					cout << "Crop not found!" << endl;
					setcolor(CYAN);
				}
			} else if (h == "help") {
				cout << "\nAvailable commands:" << endl;
				cout << "  buy [crop]   - Buy a crop" << endl;
				cout << "  skip         - Skip to next day" << endl;
				cout << "  help         - Show this help message" << endl;
				cout << "  status       - Show farm status" << endl;
			} else if (h == "status") {
				cout << "\n--- Farm Status ---" << endl;
				cout << "Day: " << day << endl;
				cout << "Money: $" << fixed << setprecision(2) << money << " w" << endl;
				cout << "Niggers: " << nig << endl;

				cout << "\nYour crops:" << endl;
				bool hasCrops = false;
				for (auto i : a) {
					if (i.bought) {
						hasCrops = true;
						setcolor(i.color);
						cout << "- " << i.name << " (Earns $" << fixed << setprecision(2)
						     << i.earn << "/day)" << endl;
					}
				}
				setcolor(CYAN);

				if (!hasCrops) {
					cout << "You don't own any crops yet!" << endl;
				}
			} else {
				setcolor(RED);
				cout << "Unknown command! Type 'help' for available commands." << endl;
				setcolor(CYAN);
			}
		}

		day++;
		system("cls");
	}

	return 0;
}
