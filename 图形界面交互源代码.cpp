#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
using namespace std;
// 保存棋盘的类
class box
{
public:
	void draw();            // 绘制
public:
	int x = 0;              // x 坐标
	int y = 0;              // y 坐标
	int value = -1;         // 值（黑棋：1，白棋：0，空位：-1）
	int modle = 0;          // 模式
	bool isnew = false;     // 是否有选择框
	COLORREF color = WHITE; // 棋盘背景色
};

// 全局变量
box map[9][9];      // 棋盘
bool whoplay = 0;      // 轮到谁下棋了
bool playercolor = 0;  // 玩家颜色
bool s = 0;//是否退出
int win = -1;//胜负
int X, Y,N=0;
int prex, prey, prexx, preyy;//上一回合下的棋
int Round = 0;//回合数
struct property
{
	int val;
	int myEye;
	int commonEye;
	int myDeadPoint;
	int myTigerPoint;
	int eneEye;
	int eneDeadPoint;
	int eneTigerPoint;
};
struct step { int x; int y; int chess; };

//单个棋子的气
int singleAir(int x, int y)//计算单个有棋位置棋子的气（注意连通性）
{
	int airNum = 0;
	if (x > 0 && map[x - 1][y].value == -1) airNum++;
	if (x < 8 && map[x + 1][y].value == -1) airNum++;
	if (y > 0 && map[x][y - 1].value == -1) airNum++;
	if (y < 8 && map[x][y + 1].value == -1) airNum++;
	return airNum;
}

//连通棋子的气
int multiAir(int x, int y, box tmap[9][9])//计算一片棋子总气数
{
	tmap[x][y].value = 1;
	int airNum = singleAir(x, y);
	if (x > 0 && map[x - 1][y].value == map[x][y].value && tmap[x - 1][y].value == -1)
		airNum = airNum + multiAir(x - 1, y, tmap);
	if (x < 8 && map[x + 1][y].value == map[x][y].value && tmap[x + 1][y].value == -1)
		airNum = airNum + multiAir(x + 1, y, tmap);
	if (y > 0 && map[x][y - 1].value == map[x][y].value && tmap[x][y - 1].value == -1)
		airNum = airNum + multiAir(x, y - 1, tmap);
	if (y < 8 && map[x][y + 1].value == map[x][y].value && tmap[x][y + 1].value == -1)
		airNum = airNum + multiAir(x, y + 1, tmap);
	return airNum;
}

//判断胜负
void isWIN()
{
	box tmap[9][9];
	if (multiAir(X, Y, tmap) == 0) 
	{
		win= !whoplay;
		return;
	}//禁止自杀
	
	int t1 = 0, t2 = 0;
	for (t1 = 0; t1 < 9; t1++) 
	{
		for (t2 = 0; t2 < 9; t2++)
		{
			box ttmap[9][9];
			if (map[t1][t2].value != -1&&multiAir(t1, t2, ttmap) == 0)
			{
				win = map[t1][t2].value;
				return;
			}
		}
	}//胜负判断
}

property func(int chess, int A, int B, int C)
{
	property result;
	int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
	int myEyeNum = 0, commonEyeNum = 0, myTigerPointNum = 0, myDeadPointNum = 0, eneEyeNum = 0, eneTigerPointNum = 0, eneDeadPointNum = 0;
	for (t1 = 0; t1 < 9; t1++) {
		for (t2 = 0; t2 < 9; t2++) {
			if (map[t1][t2].value != 0) continue;
			//计算Eye
			int tmp = map[t1][t2].value;
			map[t1][t2].value = !chess;//对方不允许下的地方
			box tmap0[9][9];
			if (multiAir(t1, t2, tmap0) == 0) {
				map[t1][t2].value = chess;
				box tmap1[9][9];
				if (multiAir(t1, t2, tmap1) != 0)
					myEyeNum++;
				else
					commonEyeNum++;
			}
			else {
				map[t1][t2].value = chess;
				box tmap4[9][9];
				if (multiAir(t1, t2, tmap4) == 0)
					eneEyeNum++;
			}

			//计算myTigerPoint
			map[t1][t2].value = !chess;
			box tmap2[9][9];
			if (singleAir(t1, t2) == 1 && multiAir(t1, t2, tmap2) == 1)
				myTigerPointNum++;
			map[t1][t2].value = chess;
			box tmap5[9][9];
			if (singleAir(t1, t2) == 1 && multiAir(t1, t2, tmap5) == 1)
				eneTigerPointNum++;

			//计算myDeadPoint
			map[t1][t2].value = chess;
			box tmapA[9][9];
			box tmapB[9][9];
			box tmapC[9][9];
			box tmapD[9][9];
			if ((t1 > 0 && multiAir(t1 - 1, t2, tmapA) == 0) || (t1 < 8 && multiAir(t1 + 1, t2, tmapB) == 0) ||
				(t2 > 0 && multiAir(t1, t2 - 1, tmapC) == 0) || (t2 < 8 && multiAir(t1, t2 + 1, tmapD) == 0))
			{
				myDeadPointNum++;
			}
			map[t1][t2].value = !chess;
			box tmapE[9][9];
			box tmapF[9][9];
			box tmapG[9][9];
			box tmapH[9][9];
			if ((t1 > 0 && multiAir(t1 - 1, t2, tmapE) == 0) || (t1 < 8 && multiAir(t1 + 1, t2, tmapF) == 0) ||
				(t2 > 0 && multiAir(t1, t2 - 1, tmapG) == 0) || (t2 < 8 && multiAir(t1, t2 + 1, tmapH) == 0))
			{
				eneDeadPointNum++;
			}

			map[t1][t2].value = tmp;
		}
	}
	result.myEye = myEyeNum;
	result.commonEye = commonEyeNum;
	result.myTigerPoint = myTigerPointNum;
	result.myDeadPoint = myDeadPointNum;
	result.eneTigerPoint = eneTigerPointNum;
	result.eneDeadPoint = eneDeadPointNum;
	result.eneEye = eneEyeNum;
	result.val = A * (myEyeNum - eneEyeNum) - B * (myDeadPointNum - eneDeadPointNum) + C * (myTigerPointNum - eneTigerPointNum);
	return result;
}

bool judgeDeadPoint(int chess, int x, int y)
{
	int tmp = map[x][y].value;
	int t3 = 0, t4 = 0;
	bool flag = 0;
	map[x][y].value = chess;
	for (t3 = 0; t3 < 9; t3++) {
		for (t4 = 0; t4 < 9; t4++) {
			box tmap0[9][9];
			if (multiAir(t3, t4, tmap0) == 0 && map[t3][t4].value == !chess) {
				flag = 1;
				t3 = 9; t4 = 9;
			}
		}
	}
	map[x][y].value = tmp;
	return flag;
}

//黑白选择
void choice()
{
	LOGFONT nowstyle;
	gettextstyle(&nowstyle);
	settextstyle(30, 15, 0, 0, 0, 1000, false, false, false);
	settextcolor(BLACK);
	outtextxy(290, 80, _T("选择"));
	outtextxy(190, 200, _T("执黑"));
	outtextxy(390, 200, _T("执白"));
	COLORREF thefillcolor = getfillcolor();
	setlinestyle(PS_SOLID, 2);
	setfillcolor(BLACK);
	setlinecolor(WHITE);
	

	line(183, 190, 183, 238);
	line(260, 190, 260, 238);
	line(183, 190, 260, 190);
	line(183, 238, 260, 238);

	line(383, 190, 383, 238);
	line(460, 190, 460, 238);
	line(383, 190, 460, 190);
	line(383, 238, 460, 238);
	while (1)
	{
		MOUSEMSG cho = GetMouseMsg();
		if (cho.x > 373 && cho.x < 450 && cho.y>170 && cho.y < 218)
		{
			if (cho.mkLButton)
			{
				playercolor = 0;//white
				break;
			}
		}
		else if (cho.x > 173 && cho.x < 250 && cho.y>170 && cho.y < 218)
		{
			if (cho.mkLButton)
			{
				playercolor = 1;//black
				break;
			}
		}
	}
	
	
}

//开始界面
void start()
{
	LOGFONT nowstyle;
	gettextstyle(&nowstyle);
	settextstyle(30, 15, 0, 0, 0, 1000, false, false, false);
	settextcolor(BLACK);
	outtextxy(290, 80, _T("新游戏"));
	outtextxy(275, 180, _T("继续游戏"));
	outtextxy(305, 280, _T("退出"));
	COLORREF thefillcolor = getfillcolor();
	setlinestyle(PS_SOLID, 2);
	setfillcolor(BLACK);
	setlinecolor(WHITE);
	line(283, 70, 283, 118);
	line(390, 70, 390, 118);
	line(283, 70, 390, 70);
	line(283, 118, 390, 118);

	line(405, 170, 405, 218);
	line(270, 170, 270, 218);
	line(405, 170, 270, 170);
	line(405, 218, 270, 218);

	line(300, 270, 300, 318);
	line(373, 270, 373, 318);
	line(300, 270, 373, 270);
	line(300, 318, 373, 318);
	while (1)
	{
		MOUSEMSG cho = GetMouseMsg();
		if (cho.x > 283 && cho.x < 390 && cho.y>70 && cho.y < 118)//新游戏
		{
			if (cho.mkLButton)
			{
				cleardevice();
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++)
						map[i][j].value = -1;
				setfillcolor(RGB(250, 205, 150));
				solidrectangle(40, 25, 645, 630);
				choice();
				break;
			}
		}
		else if (cho.x > 300 && cho.x <373 && cho.y>270 && cho.y < 318)//退出
		{
			if (cho.mkLButton)
			{
				s = 1;
				break;
			}
		}
		else if (cho.x > 270 && cho.x < 405 && cho.y>170 && cho.y < 218)//继续游戏
		{
			if (cho.mkLButton)
			{
				cleardevice();
				setfillcolor(RGB(250, 205, 150));
				solidrectangle(40, 25, 645, 630);
				break;
			}
		}
	}
	
}

// 绘制函数
void box::draw()
{
	COLORREF thefillcolor = getfillcolor(); // 备份填充颜色
	setlinestyle(PS_SOLID, 2);              // 线样式设置
	setfillcolor(color);                    // 填充颜色设置
	solidrectangle(x, y, x + 30, y + 30);   // 绘制无边框的正方形
	if (isnew)
	{
		// 如果是新下的
		// 绘制边框线
		setlinecolor(LIGHTGRAY);
		line(x + 1, y + 2, x + 8, y + 2);
		line(x + 2, y + 1, x + 2, y + 8);
		line(x + 29, y + 2, x + 22, y + 2);
		line(x + 29, y + 1, x + 29, y + 8);
		line(x + 2, y + 29, x + 8, y + 29);
		line(x + 2, y + 22, x + 2, y + 29);
		line(x + 29, y + 29, x + 22, y + 29);
		line(x + 29, y + 22, x + 29, y + 29);
	}
	setlinecolor(BLACK);
	switch (modle)
	{
		// 以下是不同位置棋盘的样式
	case 0:
		line(x + 15, y, x + 15, y + 30);
		line(x - 1, y + 15, x + 30, y + 15);
		break;
		//  *
		// ***
		//  *
	case 1:
		line(x + 14, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		// *
		// ***
		// *
	case 2:
		line(x - 1, y + 15, x + 15, y + 15);
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		//   *
		// ***
		//   *
	case 3:
		line(x + 15, y + 15, x + 15, y + 30);
		setlinestyle(PS_SOLID, 3);
		line(x - 1, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		// ***
		//  *
		//  *
	case 4:
		line(x + 15, y, x + 15, y + 15);
		setlinestyle(PS_SOLID, 3);
		line(x - 1, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		//  *
		//  *
		// ***
	case 5:
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 15);
		line(x + 15, y + 15, x + 30, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		// *
		// *
		// ***
	case 6:
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y, x + 15, y + 15);
		line(x - 1, y + 15, x + 15, y + 15);
		setlinestyle(PS_SOLID, 2);
		break;
		//   *
		//   *
		// ***
	case 7:
		setlinestyle(PS_SOLID, 3);
		line(x - 1, y + 15, x + 15, y + 15);
		line(x + 15, y + 15, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		// ***
		//   *
		//   *
	case 8:
		setlinestyle(PS_SOLID, 3);
		line(x + 15, y + 15, x + 30, y + 15);
		line(x + 15, y + 15, x + 15, y + 30);
		setlinestyle(PS_SOLID, 2);
		break;
		// ***
		// *
		// *
	case 9:
		line(x + 15, y, x + 15, y + 30);
		line(x - 1, y + 15, x + 30, y + 15);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 4);
		break;
		//  *
		// *O*
		//  *
	}
	switch (value)
	{
	case 0: // 白棋
		setfillcolor(WHITE);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 13);
		break;
	case 1: // 黑棋
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID, 1);
		fillcircle(x + 15, y + 15, 13);
		break;
	}
	setfillcolor(thefillcolor); // 还原填充色
}

// 绘制棋盘
void draw()
{
	setfillcolor(RGB(250, 205, 150));
	solidrectangle(40, 25, 645, 630);
	// 设置字体样式
	settextstyle(0, 0, NULL);
	settextcolor(BLACK);
	if (playercolor == 0)
	{
		outtextxy(450, 150, _T("        AI"));
		outtextxy(450, 200, _T("屏幕前的你"));
	}
	else
	{
		outtextxy(450, 200, _T("        AI"));
		outtextxy(450, 150, _T("屏幕前的你"));
	}
	int number = 0; // 坐标输出的位置
	// 坐标（数值）
	TCHAR strnum[9][3] = { _T("1"),_T("2") ,_T("3") ,_T("4"),_T("5") ,_T("6") ,_T("7"),_T("8"),_T("9") };
	// 坐标（字母）
	TCHAR strabc[9][3] = { _T("A"),_T("B") ,_T("C") ,_T("D"),_T("E") ,_T("F") ,_T("G"),_T("H"),_T("I") };
	LOGFONT nowstyle;
	gettextstyle(&nowstyle);
	settextstyle(0, 0, NULL);
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			map[i][j].draw(); // 绘制
			if (map[i][j].isnew == true)
			{
				map[i][j].isnew = false; // 把上一个下棋位置的黑框清除
			}
		}
	}
	outtextxy(400, 100, _T("落子方："));
	if (whoplay == 1)
		outtextxy(480, 100, _T("黑方"));
	else if(whoplay==0)
		outtextxy(480, 100, _T("白方"));
	outtextxy(400, 150, _T("黑方："));
	outtextxy(400, 200, _T("白方："));
	outtextxy(150, 330, _T("悔  棋"));
	outtextxy(300, 330, _T("认  输"));
	outtextxy(450, 330, _T("返 回 菜 单"));
	setlinecolor(WHITE);
	
	rectangle(145, 325, 197, 351);
	
	rectangle(295, 325, 347, 351);

	rectangle(445, 325, 534, 351);
	
	for (int i = 0; i < 9; i++)
	{
		outtextxy(75 + number, 35, strnum[i]);
		outtextxy(53, 55 + number, strabc[i]);
		number += 30;
	}
	settextstyle(&nowstyle);
}

// 对局初始化
void init()
{
	for (int i = 0, k = 0; i < 270; i += 30)
	{
		for (int j = 0, g = 0; j < 270; j += 30)
		{
			int modle = 0;  // 棋盘样式
			map[k][g].value = -1;
			map[k][g].color = RGB(250, 205, 150);// 棋盘底色
			// x、y 坐标
			map[k][g].x = 65 + j;
			map[k][g].y = 50 + i;
			// 棋盘样式的判断
			if (k == 0 && g == 0)
			{
				modle = 8;
			}
			else if (k == 0 && g == 8)
			{
				modle = 7;
			}
			else if (k == 8 && g == 8)
			{
				modle = 6;
			}
			else if (k == 8 && g == 0)
			{
				modle = 5;
			}
			else if (k == 0)
			{
				modle = 3;
			}
			else if (k == 8)
			{
				modle = 4;
			}
			else if (g == 0)
			{
				modle = 1;
			}
			else if (g == 8)
			{
				modle = 2;
			}
			else  if ((k == 2 && g == 2) || (k == 2 && g == 6) || (k == 6 && g == 2) || (k == 6 && g == 6) || (k == 4 && g == 4) )
			{
				modle = 9;
			}
			else
			{
				modle = 0;
			}
			map[k][g].modle = modle;
			g++;
		}
		k++;
	}
}

step AI(int chess, int N, int tmp, int roundNum, int A, int B, int C)
{
	if (roundNum % 10 == 0) N++;
	step result;
	result.chess = chess;
	struct dot { int x; int y; int VALUE; };
	dot goodChoice[100]; int goodNum = 0;

	int t1 = 0, t2 = 0, VALUE = -1000;
	for (t1 = 0; t1 < 9; t1++) 
	{
		for (t2 = 0; t2 < 9; t2++) 
		{
			if (map[t1][t2].value == -1) 
			{
				int tmp3 = map[t1][t2].value;
				map[t1][t2].value = chess;
				box tmap0[9][9];
				int valNum = 0;
				valNum = func(chess, A, B, C).val;
				if (VALUE < valNum && multiAir(t1, t2, tmap0) != 0 
					&& judgeDeadPoint(chess, t1, t2) == 0) {
					//int flag=0,t3=0,t4=0;
					VALUE = valNum;
					result.x = t1; result.y = t2;
					goodChoice[goodNum].VALUE = VALUE;
					goodChoice[goodNum].x = t1;
					goodChoice[goodNum].y = t2;
					goodNum++;
				}
				map[t1][t2].value = tmp3;
			}
		}
	}
	int maxValue = VALUE;
	//负极大值方案
	if (tmp < N && roundNum >= 0)
	{
		tmp++;
		int t0 = 0;
		t1 = 0, t2 = 0, VALUE = 1000;
		for (t0 = 0; t0 < goodNum; t0++)
		{
			t1 = goodChoice[t0].x;
			t2 = goodChoice[t0].y;
			if (maxValue == goodChoice[t0].VALUE)
			{
				box tmap1[9][9] = { 0 };
				if (map[t1][t2].value != -1 || multiAir(t1, t2, tmap1) == 0 ||
					judgeDeadPoint(!chess, t1, t2) != 0) continue;
				int tmp0 = map[t1][t2].value;
				map[t1][t2].value = chess;
				step enemyGo = AI(!chess, N, tmp, roundNum, A, B, C);
				int t3 = enemyGo.x;
				int t4 = enemyGo.y;
				int tmp1 = map[t3][t4].value;
				map[t3][t4].value = !chess;
				box tamp0[9][9];
				if (VALUE > func(!chess, A, B, C).val) {
					VALUE = func(!chess, A, B, C).val;
					result.x = t1; result.y = t2;
				}
				map[t3][t4].value = tmp1;
				map[t1][t2].value = tmp0;
			}
		}
	}
	return result;
}

// 游戏主函数
void game()
{
	bool isinit;
	// 上一个鼠标停的坐标
	int oldi = 0;
	int oldj = 0;
    
	// 绘制背景
	setfillcolor(RGB(250, 205, 150));
	solidrectangle(40, 25, 645, 630);
	start();
	if (s == 1) return;
	if (playercolor == 0)
	{
		isinit = 1;
		whoplay = 1;
	}
	else
	{
		isinit = 0;
		whoplay = 1;
	}
	draw(); // 绘制
	
	while (1)
	{
	NEXTPLAYER:
		if (whoplay == playercolor)
		{
			while (1)
			{
				MOUSEMSG cho = GetMouseMsg();
				if (cho.y > 325 && cho.y < 351 && cho.x>145 && cho.x < 197)//悔棋
				{
					if (cho.mkLButton)
					{
						map[prex][prey].value = -1;
						map[prexx][preyy].value = -1;
						goto DRAW;
					}
				}
				if (cho.y > 325 && cho.y < 351 && cho.x>295 && cho.x < 347)//认输
				{
					if (cho.mkLButton)
					{
						if (playercolor == 1)
						{
							outtextxy(450, 250, _T("白胜！"));
							Sleep(5000);
							game();
						}
						else if (playercolor == 0)
						{
							outtextxy(450, 250, _T("黑胜！"));
							Sleep(5000);
							game();
						}
					}
				}
				if (cho.y > 325 && cho.y < 445 && cho.x>351 && cho.x < 534)//返回菜单
					if (cho.mkLButton)
					{
						game();
	
					}
				for (int i = 0; i < 9; i++)
				{
					for (int j = 0; j < 9; j++)
					{
						if (cho.x > map[i][j].x && cho.x < map[i][j].x + 30
							&& cho.y>map[i][j].y && cho.y < map[i][j].y + 30
							&& map[i][j].value == -1)
						{
							if (cho.mkLButton)
							{
								prex = i;
								prey = j;
								map[i][j].value = playercolor;
								map[i][j].isnew = true;
								whoplay = !playercolor;
								oldi = -1;
								oldj = -1;
								X = i;
								Y = j;
								goto DRAW;
							}
							map[oldi][oldj].isnew = false;
							map[oldi][oldj].draw();
							map[i][j].isnew = true;
							map[i][j].draw();
							oldi = i;
							oldj = j;
						}
					}
				}
			}

		}
		else
		{
			if (isinit)
			{
				isinit = 0;
				unsigned seed = time(0);
				srand(seed);
				int xx = rand() % 9;
				int yy = rand() % 9;
				map[xx][yy].value = 1;
				map[xx][yy].isnew = true;
			}
			else
			{
				int xx, yy,roundNum=0;
				int A = 50, B = 40, C = 40;
				xx = AI(whoplay, N, 0, roundNum, A, B, C).x;
				yy = AI(whoplay, N, 0, roundNum, A, B, C).y;
				map[xx][yy].value = whoplay;
				map[xx][yy].isnew = true;
				prexx = xx;
				preyy = yy;
			}
			whoplay = !whoplay;
			goto DRAW;
		}

	DRAW:
		isWIN();
		cleardevice();
		draw();
		oldi = 0;
		oldj = 0;
		if (win == -1)
		{
			Sleep(500);
			goto NEXTPLAYER;
		}
		settextcolor(BLACK);
		if (win == 0)
		{
			outtextxy(450, 250, _T("白胜！"));
			Sleep(5000);
			game();
		}
		if (win == 1)
		{
			outtextxy(450, 250, _T("黑胜！"));
			Sleep(5000);
			game();
		}
		Sleep(5000);
		break;
	}

	return;
}

// main函数
int main()
{
	initgraph(685, 370); // 初始化绘图环境
    setbkcolor(WHITE);
	cleardevice();
	setbkmode(TRANSPARENT); // 设置透明文字输出背景
	init(); // 初始化
	game(); // 游戏开始
	
}
