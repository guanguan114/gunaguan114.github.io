#include<bits/stdc++.h>
#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<time.h>
#define KEY_DOWN(VK_NONAME)((GetAsyncKeyState(VK_NONAME)&0x8000)?1:0)
using namespace std;
int m[10001][21];
struct node {int xx,yy,Zhong,rr,mr;} B[100001];
int K,X,Y,tX,tY,tY2,tT,Er,T,Yb,Zb,Win,Blood,Score,Kb;
void Color(int a)
{
	if(a==0) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
	if(a==1) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
	if(a==2) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	if(a==3) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
	if(a==4) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
	if(a==5) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
	if(a==6) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE);
	if(a==7) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN);
	if(a==8) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
	if(a==9) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_RED);
	if(a==10) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE|BACKGROUND_INTENSITY|BACKGROUND_BLUE);
}
void SetPos(int x,int y)
{
	COORD pos; pos.X=y*2-1,pos.Y=x+1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}
void Zha(int x,int y)
{
	Zb++;
	int k1=rand()%2+2,k2=rand()%3+2,r=rand()%6;
	if(r==0) B[Zb].xx=x-K,B[Zb].yy=y,B[Zb].Zhong=1,B[Zb].rr=0,B[Zb].mr=k1*2;
	if(r==1) B[Zb].xx=x-K,B[Zb].yy=y,B[Zb].Zhong=2;
	if(r==2) B[Zb].xx=x-K,B[Zb].yy=y,B[Zb].Zhong=3,B[Zb].rr=k2,B[Zb].mr=k1;
	if(r==3) B[Zb].xx=x-K,B[Zb].yy=y,B[Zb].Zhong=4;
	if(r==4) B[Zb].xx=x-K,B[Zb].yy=y,B[Zb].Zhong=5,B[Zb].rr=1,B[Zb].mr=k2*2;
	if(r==5) B[Zb].xx=x-K,B[Zb].yy=y,B[Zb].Zhong=rand()%2+6,B[Zb].rr=k1+1,B[Zb].mr=k2+3;
}
void Cout(int i,int j,int a)
{
	int R=rand()%500,jk=0;
	if(i-K>=0&&i-K<=1&&j>=2&&j<=6) return;
	if(a!=4)
	{
		if(m[i][j]==0&&(a==0||a==3)) SetPos(i-K,j),Color(7),cout<<"■";
		if(m[i][j]==1&&a==5) SetPos(i-K,j),Color(0),cout<<"  ";//空气 
		if(m[i][j]==2) SetPos(i-K,j),Color(8),cout<<"■";
		if(m[i][j]>=3&&m[i][j]<6) SetPos(i-K,j),Color(5),cout<<"█",m[i][j]++;
		if(m[i][j]==6&&a!=5) SetPos(i-K,j),cout<<"  ",m[i][j]=1;
		if(m[i][j]==7) SetPos(i-K,j),Color(9),cout<<"~ ";
		if(m[i][j]==8) SetPos(i-K,j),Color(10),cout<<"~ ";
		if(m[i][j]==9) SetPos(i-K,j),Color(3),cout<<"◆";
		if((m[i][j]==1||m[i][j]==8)&&m[i-1][j]>=10&&m[i-1][j]<=30&&a!=5) {int M=m[i][j];m[i][j]=m[i-1][j],m[i-1][j]=M;if(M==1) SetPos(i-K-1,j),cout<<"  ";}//炸弹下落 
		if(m[i][j]==1&&m[i-1][j]==9&&a!=5) {m[i][j]=9,m[i-1][j]=1,SetPos(i-K-1,j),Color(0),cout<<"  ";jk=1;}//宝石下落
		if(m[i][j]>=10&&m[i][j]<=30) {m[i][j]++;if(m[i][j]%3==0) Color(4);else Color(5);SetPos(i-K,j),cout<<"●";}//炸弹计时
		if(m[i][j]==1&&m[i-1][j]==7&&a!=5) {m[i][j]=7,m[i-1][j]=1,SetPos(i-K-1,j),Color(0),cout<<"  ";jk=1;}//岩浆下落
		if(m[i][j]==0&&m[i-1][j]==7&&R<=2) m[i][j]=m[i-1][j],m[i-1][j]=1,SetPos(i-K-1,j),cout<<"  ";//岩浆融解 
		if(m[i][j]==7&&R>2&&R<=5) {int r=rand()%3-1; if(m[i][j+r]==1) m[i][j+r]=m[i][j];if(m[i][j+r]==8) m[i][j+r]=2;}//岩浆扩散  
		if(R<50&&R>=20&&m[i+1][j+1]==1&&m[i+1][j]==8&&m[i][j]==8) m[i+1][j+1]=8,m[i][j]=1,SetPos(i-K,j),Color(0),cout<<"  ";
		if(R<80&&R>=50&&m[i+1][j-1]==1&&m[i+1][j]==8&&m[i][j]==8) m[i+1][j-1]=8,m[i][j]=1,SetPos(i-K,j),Color(0),cout<<"  ";//岩浆平面 
		if(m[i][j]==1&&m[i-1][j]==8&&a!=5) {m[i][j]=8,m[i-1][j]=1,SetPos(i-K-1,j),Color(0),cout<<"  ";jk=1;}//水下落
		if(m[i+1][j+1]==1&&m[i+1][j]==8&&m[i][j]==8) m[i+1][j+1]=8,m[i][j]=1,SetPos(i-K,j),Color(0),cout<<"  ";
		if(m[i+1][j-1]==1&&m[i+1][j]==8&&m[i][j]==8) m[i+1][j-1]=8,m[i][j]=1,SetPos(i-K,j),Color(0),cout<<"  ";//水平面 
		if((m[i+1][j]==8&&m[i][j]==7)||(m[i+1][j]==7&&m[i][j]==8)) m[i][j]=2,m[i-1][j]=1,SetPos(i-K-1,j),Color(0),cout<<"  ";//变石头
		if(m[i][j]==0&&m[i-1][j]==8&&R>5&&R<=7) m[i][j]=m[i-1][j],m[i-1][j]=1,SetPos(i-K-1,j),cout<<"  ";//水融解 
		if(m[i][j]==8&&R>7&&R<=9) {int r=rand()%3-1; if(m[i][j+r]==1) m[i][j+r]=m[i][j];if(m[i][j+r]==7) m[i][j+r]=2;}//水扩散  
		if(m[i][j]==31) SetPos(i-K,j),cout<<"  ",Zha(i,j);
	}
	else if(a!=5)//界外 
	{
		if(m[i][j]>=3&&m[i][j]<6) m[i][j]++; if(m[i][j]==6) m[i][j]=1;
		if((m[i][j]==1||(m[i][j]>=3&&m[i][j]<=6))&&m[i-1][j]>=10&&m[i-1][j]<=30) m[i][j]=m[i-1][j],m[i-1][j]=1;//炸弹下落 
		if(m[i][j]>=10&&m[i][j]<=30) m[i][j]++;//炸弹计时
		if(m[i][j]==1&&m[i-1][j]==9) m[i][j]=9,m[i-1][j]=1;//宝石下落
		if(m[i][j]==1&&m[i-1][j]==7) m[i][j]=7,m[i-1][j]=1;//岩浆下落
		if(m[i][j]==1&&m[i-1][j]==8) m[i][j]=8,m[i-1][j]=1;//水下落
	}if(jk==1) jk=0,Cout(i,j,5);
	Color(0);
}
void Map(int a);
void Wo()
{
	SetPos(X-K,Y),cout<<"  "; //清除上一次残影
	if(m[X][Y]==9) Blood++,Score+=5,m[X][Y]=1,system("color 2F"),Sleep(50),system("color 0F"),Map(3);
	if(tX!=0&&(m[X-1][Y]==1||m[X-1][Y]==7||m[X-1][Y]==8||m[X-1][Y]==9||(m[X-1][Y]>=3&&m[X-1][Y]<=6))) tX--,X-=2;  //上跳 
	if(m[X+1][Y]==1||(m[X+1][Y]>=3&&m[X+1][Y]<=6)||m[X+1][Y]==7||m[X+1][Y]==8||m[X+1][Y]==9) X++; //掉落 
	if(m[X+1][Y]==0||m[X+1][Y]==2||(m[X+1][Y]>=3&&m[X+1][Y]<=6)) tX=Er=0; //跳跃次数清零 
	if(m[X-1][Y]==0||m[X-1][Y]==2||(m[X-1][Y]>=10&&m[X-1][Y]<=31)) tX=Er=0; //跳跃次数清零 
	if(X<=K) tX=Er=0,X=K; if(X>=K+28) X=K+28,Kb=1; //高度上下限
	if(m[X][Y]==7) m[X-1][Y]=m[X-2][Y]=1,tX+=8;
	if(m[X][Y]!=1&&m[X][Y]!=8&&m[X][Y]!=9) Blood--,m[X][Y]=1,system("color 4F"),Sleep(50),system("color 0F"),Map(3);
	if(Blood<=0) Win=-1;
	SetPos(X-K,Y),Color(1),cout<<"●";
}
void Map(int a)
{
	SetPos(0,2);cout<<"生命 ："<<Blood<<"  ";
	SetPos(1,2);cout<<"得分 ："<<Score<<"  ";
	if(a==3) system("cls");
	for(int i=K+28;i>=K;i--)for(int j=1;j<=20;j++) Cout(i,j,a);
	for(int i=K+33;i>K+28;i--)for(int j=1;j<=20;j++) Cout(i,j,4);
	if(a!=3) Wo();
}
void CircleBomb(int x,int y,int s,int ms)
{
	if(s==ms) return;
	for(int i=x-s;i<=x+s;i++)
		for(int j=y-s;j<=y+s;j++)
		{
			float k=(i-x)*(i-x)+(j-y)*(j-y)-s*s;
			if(k<=s&&k>=-s&&j>0&&j<=20&&m[i+K][j]!=2&&m[i+K][j]!=9) m[i+K][j]=3;
		}Zb++;
	B[Zb].xx=x,B[Zb].yy=y,B[Zb].Zhong=1,B[Zb].rr=s+1,B[Zb].mr=ms;
	Sleep(30);
}
void LineBomb(int i,int j)
{
	for(int k=0;j+k<=20;k++) if(m[i+K][j+k]!=2&&m[i+K][j+k]!=9) m[i+K][j+k]=3;
	for(int k=0;j+k<=20;k++) if(m[i+K+1][j+k]!=2&&m[i+K+1][j+k]!=9) m[i+K+1][j+k]=3;
	for(int k=0;j-k>0;k++) if(m[i+K][j-k]!=2&&m[i+K][j-k]!=9) m[i+K][j-k]=3;
	for(int k=0;j-k>0;k++) if(m[i+K+1][j-k]!=2&&m[i+K+1][j-k]!=9) m[i+K+1][j-k]=3;
}
void ZuanBomb(int i,int j)
{
	int k;
	for(k=0;m[i+K+k][j]!=2&&k<=7;k++) if(m[i+K+k][j]!=9) m[i+K+k][j]=3;
	int k1=rand()%2+2;Zb++;
	B[Zb].xx=i+k,B[Zb].yy=j,B[Zb].Zhong=1,B[Zb].rr=0,B[Zb].mr=k1*3/2;
}
void TrigleBomb(int i,int j,int r,int mr)
{
	if(r==mr) return;
	if(r==1) m[i+K][j]=3,i++;
	for(int k=max(0,j-r);k<=min(20,j+r);k++) if(m[i+K][k]!=2&&m[i+K][k]!=9) m[i+K][k]=3;
	Zb++;B[Zb].xx=i+1,B[Zb].yy=j,B[Zb].Zhong=5,B[Zb].rr=r+1,B[Zb].mr=mr;
}
void GunBomb(int i,int j,int r,int mr)
{
	if(mr<=0) return;Zb++;
	B[Zb].xx=i,B[Zb].yy=j+2,B[Zb].Zhong=6,B[Zb].rr=r,B[Zb].mr=mr-1;Zb++;
	B[Zb].xx=i,B[Zb].yy=j,B[Zb].Zhong=1,B[Zb].rr=r-1,B[Zb].mr=r;
}
void GunBomb2(int i,int j,int r,int mr)
{
	if(mr<=0) return;Zb++;
	B[Zb].xx=i,B[Zb].yy=j-2,B[Zb].Zhong=7,B[Zb].rr=r,B[Zb].mr=mr-1;Zb++;
	B[Zb].xx=i,B[Zb].yy=j,B[Zb].Zhong=1,B[Zb].rr=r-1,B[Zb].mr=r;
}
void ThreeBomb(int x,int y,int s,int ms)
{
	for(int i=1;i<=3;i++)
	{
		int Xx=rand()%(2*s+1)-s,Yy=rand()%(2*s+1)-s; Zb++;
		B[Zb].xx=Xx+x,B[Zb].yy=Yy+y,B[Zb].Zhong=1,B[Zb].rr=0,B[Zb].mr=ms+rand()%3-1;
	}
}
void Sheng(int a)
{
	K++;
	for(int i=1;i<=20;i++)
	{
		int R=rand()%max(80-(K/10),30);int Rr=rand()%200;
		if(R<=3) m[K+28][i]=2;
		else if(R<=6) m[K+28][i]=7;
		else if(R<=9) m[K+28][i]=8;
		if(Rr==0) m[K+28][i]=9;
	}
	if(a!=1) system("cls"),Map(0);
}
void Jiao()
{
	SetPos(0,2);Color(1),cout<<"●",Color(7),cout<<"   ■ ",Color(8),cout<<"   ■ ",Color(4),cout<<"   ●    ",Color(9),cout<<"~ ",Color(0),cout<<"   ",Color(10),cout<<"~ ",Color(3),cout<<"   ◆";
	SetPos(2,2);Color(0),cout<<"你  泥土  石块  炸弹  岩浆  水  宝石";
	SetPos(4,1),cout<<"  ↑           空格放炸弹..."; 
	SetPos(5,1),cout<<"←  →移动 ";
	SetPos(6,1),cout<<"  ↓           可二段跳。";
	SetPos(8,1),cout<<"炸弹种类随机，计时3秒爆炸。";
	SetPos(10,1),cout<<"(一共有7种炸弹，有几率组合一起爆炸)";
	SetPos(12,1),cout<<"岩浆和水有几率向左右扩散...";
	SetPos(14,1),cout<<"它们相融会产生石块。";
	SetPos(16,1),cout<<"岩浆和爆炸波会使你减血...";
	SetPos(18,1),cout<<"岩浆还会使你上跳，宝石可以加血。";
	SetPos(20,1),cout<<"画面每隔一段时间会下降...";
	SetPos(22,1),cout<<"当你抵达画面底部时，画面会随你一起下降...";
	SetPos(24,1),cout<<"当你被抵在画面顶部时，会持续减血。";
	SetPos(26,1),cout<<"每隔一段时间分数会增加...",Color(5),cout<<"200分即可通关！";
	SetPos(28,1);Color(1);cout<<"按 y 开始游戏!";
	char tt;while(tt!='y') tt=_getch();
}
void Start()
{
	Color(5);
	SetPos(2,3);Color(7),cout<<" ■        ",Color(5),cout<<"掘",Color(7),cout<<"    ■■■■ ";
	SetPos(3,3);Color(7),cout<<" ■■     ",Color(5),cout<<" ↓",Color(7),cout<<"    ■■■■ ";
	SetPos(4,3);Color(7),cout<<" ■■■■ ",Color(5),cout<<" ↓",Color(7),cout<<"  ■■■■■ ";
	SetPos(5,3);Color(7),cout<<" ■■■■■",Color(5),cout<<"↓",Color(7),cout<<"  ■■■■■ ";
	SetPos(6,3);Color(7),cout<<" ■■■■■  ■■■",Color(5),cout<<"地",Color(7),cout<<"■■ ";
	SetPos(7,3);Color(7),cout<<" ■■■■■■■■■■■■ ";Color(5);
	SetPos(22,2);Color(1);cout<<"按 y 确定!";
	SetPos(22,10);Color(9);cout<<"    开始游戏!    ";
	SetPos(24,10);Color(0);cout<<"    操作攻略!    ";
	SetPos(27,1);Color(3);cout<<"注意！这里 绝对不能是拼音输入法!";
	SetPos(28,5);Color(3);cout<<"↓";Color(0);
	char tt;int Ee=0;
	while(tt!='y')
	{
		tt=_getch();
		if(tt==72&&Ee!=0) Ee--;
		if(tt==80&&Ee!=2) Ee++;
		SetPos(22,10);if(Ee==0) Color(9);else Color(0);cout<<"    开始游戏!    ";
		SetPos(24,10);if(Ee==1) Color(9);else Color(0);cout<<"    操作攻略!    ";
	}system("color 0F");system("cls");Color(0);
	if(Ee==1) Jiao();
}
int main()
{
	system("mode con cols=42 lines=31");
	CONSOLE_CURSOR_INFO cursor_info={1,0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
	srand((unsigned)time(NULL));
	Start();
	St:system("cls");
	K=-5,T=Win=Score=0;X=Y=2;Blood=20;
	memset(m,0,sizeof(m));memset(B,0,sizeof(B));
	for(int i=5;i<=15;i++) for(int j=1;j<=20;j++) m[i][j]=1;
	for(int i=1;i<=10;i++) Sheng(1);
	Map(0);
	while(Win==0)
	{
		T++;Kb=0;if(Score>=200) Win=1;
		if(GetAsyncKeyState(VK_UP)&0x8000&&tT==0&&Er<2) tT++,tX+=4,Er++;
		if((GetAsyncKeyState(VK_UP)&0x8000)?0:1) tT=0;
		if(GetAsyncKeyState(VK_LEFT)&0x8000&&Y>1&&(m[X][Y-1]==1||m[X][Y-1]==7||m[X][Y-1]==8||m[X][Y-1]==9)&&(tY==0||tY>=2)) SetPos(X-K,Y),cout<<"  ",Y--;
		if(GetAsyncKeyState(VK_RIGHT)&0x8000&&Y<20&&(m[X][Y+1]==1||m[X][Y+1]==7||m[X][Y+1]==8||m[X][Y+1]==9)&&(tY2==0||tY2>=2)) SetPos(X-K,Y),cout<<"  ",Y++;
		if((GetAsyncKeyState(VK_LEFT)&0x8000)?0:1) tY=0;if((GetAsyncKeyState(VK_RIGHT)&0x8000)?0:1) tY2=0;
		if((GetAsyncKeyState(VK_LEFT)&0x8000)?1:0) tY++;if((GetAsyncKeyState(VK_RIGHT)&0x8000)?1:0) tY2++;
		if(kbhit()) {char e=_getch();if(e==' '&&m[X-1][Y]!=0&&m[X-1][Y]!=2&&(m[X+1][Y]<=10||m[X+1][Y]>=31)&&Kb==0) m[X][Y]=10,X--;}
		int zb=Zb;
		for(int i=zb-10;i<=zb;i++)
		{
			if(B[i].Zhong==1) CircleBomb(B[i].xx,B[i].yy,B[i].rr,B[i].mr),B[i].Zhong=0;
			if(B[i].Zhong==2) LineBomb(B[i].xx,B[i].yy),B[i].Zhong=0;
			if(B[i].Zhong==3) ThreeBomb(B[i].xx,B[i].yy,B[i].rr,B[i].mr),B[i].Zhong=0;
			if(B[i].Zhong==4) ZuanBomb(B[i].xx,B[i].yy),B[i].Zhong=0;
			if(B[i].Zhong==5) TrigleBomb(B[i].xx,B[i].yy,B[i].rr,B[i].mr),B[i].Zhong=0;
			if(B[i].Zhong==6) GunBomb(B[i].xx,B[i].yy,B[i].rr,B[i].mr),B[i].Zhong=0;
			if(B[i].Zhong==7) GunBomb2(B[i].xx,B[i].yy,B[i].rr,B[i].mr),B[i].Zhong=0;
		}
		Map(1);Sleep(40);
		if(T%max(10,40-Score/16)==0||Kb>=1) Sheng(0);
		if(T%20==0) Score++;
	}
	if(Win!=0)
	{
		if(Win>0) {system("color 6E"),Color(3);SetPos(0,2);cout<<"You! Win!!!",Sleep(1000);}
		if(Win<0) {system("color 7F"),Color(4);SetPos(0,2);cout<<"You! Die!!!",Sleep(1000);}
		SetPos(1,2);cout<<"请输入y重新开始游戏";
		A:char e=_getch();if(e!='y') goto A;goto St;
	}
}
