#include<stdio.h>
#include<stdlib.h>
#include<graphics.h>
#include<conio.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include<time.h>
#include"tools.hpp"//一个封装好的函数，可以直接打印不规则图片，不用搞掩码图和非掩码图，直接抠图就行了
#define MY_MENU 0
#define MY_GRADE 1
#define MY_SET 2
#define MY_MAKER 3
#define MY_RULE 4
#define MY_OVER -1
#define MY_LONGIN 5
typedef struct {
	int uMsg;
	int x;
	int y;
} MODSEMSG;
// -------------------- 数据设计 --------------------

/*
	前端地图
    map1[i][j]表示坐标（i,j)
    0表示未被点开
    1表示被点开
    2表示插旗
*/
int map1[16][30];

/*
    后端地图
    如map[i][j]表示坐标（i,j)
    0~8表示周围八格里雷的数量
    9表示该坐标有雷
	10表示该坐标为宝藏
*/
int map2[16][30];

/*
	表示当前在使用哪套皮肤
*/
int skin = 1;

/*
	表示当前音效开关状态
*/
int sound = 1;

/*
	用于储存结算界面返回的操作
*/
int winView = 0;

/*
	用于控制界面切换
*/
int flag = 0;
/*
	用于宝藏模式皮肤启用
*/
int xunbaomoshi = 0;
// -------------------- 数据设计 --------------------

// -------------------- view --------------------

/*
	负责人：云水
	功能：
		2.把窗口句柄改为“扫雷”
		3.BeginBatchDraw();
		4.载入各种东西
		while（1）
		{
		5.cleardevice();
		6.展示页面
		7.展示选项：		进入游戏/游戏设置/开发者概述/游戏规则/退出游戏
		8.点击进入游戏：		flag=1
		9.点击游戏设置：		flag=2
		10.点击开发者概述:	flag=3
		11.游戏规则:			flag=4
		12.退出游戏:			flag=-1
		13.FlushBatchDraw();
		14.如果flag!=0		break;
		}
	参数：void
	返回值：void
*/
void menuView();



/*
	负责人：峠
	功能：				控制界面切换（参考项目常用思路）
				  while (1) {
								switch (flag) {
								case MY_MENU:
									menuView();
									break;
								case MY_SET:
									setView();
									break;
								case MY_GRADE:
									gradeView();
									break;
								case MY_MAKER:
									makerView();
									break;
								case MY_RULE:
									ruleView();
									break;
								case MY_OVER:
									EndBatchDraw();
									closegraph();
									exit(0);
								}
							}

	参数：void
	返回值：void
*/
void controller();
//用于自动展开
void boom(int i, int j, int x, int y);
/*
	负责人：积羽
	功能：
	1.展示选项
						while(1)
						{
						如果winView=1		则flag=0;	break;	winView=0;
						初级/中级/高级/寻宝/返回
						初级: 把相应地图参数赋给x,y,z,调用gameView(x, y, z) x为纵格数，y为横格数，z为模式（1为经典，2为寻宝）
						中级: 同上
						高级: 同上
						寻宝: 同上
						返回: break;		flag=0;
						}
	参数：void
	返回值：int
*/
void gradeView();

/*
	负责人：忔往
	功能：	1.根据x，y调用init(x, y, z)初始化
			while(1)
			{
			2.cleardevice();
			3.展示选项：	重置/重新选择难度
						重置：调用init(x,y,z)
						重新选择难度:   break;
			4.根据x和y展示地图  调用mapshowView(x,y)
			5.根据玩家鼠标点击的方格给playerclick(i,j,k)传入相应坐标和行为(i为纵坐标，j为横坐标，k为鼠标行为（1为左击，2为右击））
			6.根据参数z选择胜利判定：	z=1则调用isWin1(x,y);	z=2则调用isWin2();
			7.根据选择的那个胜利判定的返回值选择：      0则继续；1，2则调用gameoverView( （胜利判定的返回值） );
			8.如果胜利判定返回1或2就根据winView的值选择：		1则break;	0则调用init(x,y,z)
			9.FlushBatchDraw();
			}
	参数：x为纵格数，y为横格数，z为模式（1为经典，2为寻宝）
	返回值：void
*/
void gameView(int x, int y, int z);

/*
	负责人：云水
	功能：while(1)
	{
	  展示选项  音量设置/皮肤选择/返回
	  点击音量设置  切换音量开/关状态(全局变量sound在0和1之间切换)
	  点击皮肤选择  调用skinView()
	  点击返回	   flag=0;	break;
	}
	参数：void
	返回值：void
*/
void setView();

/*
	负责人：711
	功能：展示选项  皮肤1/皮肤1/皮肤1/返回
		 点击皮肤 改全局变量skin为相应数字
		 点击返回 flag=0;	结束函数
	参数：void
	返回值：void
*/
void skinView();

/*
	负责人：如今
	功能：	展示开发者概述内容
			展示翻页/返回
			点击翻页  进行翻页操作
			点击返回  flag=0;	结束函数
	参数：
	返回值：
*/
void makerView();
//计时器，用于积分倒减
int Timer(int duration, int id);
/*
	负责人：积羽
	功能：	展示规则内容
			展示返回/翻页
			点击翻页  进行翻页操作
			点击返回  flag=0;	结束函数
	参数：
	返回值：
*/
void ruleView();

/*
	负责人：711
	功能：	如果x=1则展示失败界面（包含重新开始/返回）同时如果sound=1则播放失败音效
			点击返回则winView=1
			点击重新开始则winView=0

			如果x=2则展示胜利界面（包含返回）同时如果sound=1则播放胜利音效
			点击返回则winView=1
	参数：int
	返回值：void
*/
void gameoverView(int x);

/*
	负责人：忔往
	功能：	根据skin的值选择显示的皮肤
			skin=1则显示皮肤1
			skin=2则显示皮肤2
			skin=3则显示皮肤3
			综合map1和map2的情况在游戏界面显示数字或雷
			如果map1[i][j]=1且map2[i][j]=(1~8)则在相应位置显示数字
			如果map1[i][j]=1且map2[i][j]=0则在相应位置显示点开状态
			如果map1[i][j]=1且map2[i][j]=9则在相应位置显示雷,同时如果sound=1则播放爆炸音效
			如果map1[i][j]=1且map2[i][j]=10则在相应位置显示宝藏，同时如果sound=1则播放宝藏打开音效
			如果map1[i][j]=2则在相应位置显示旗帜
	参数：x为纵格数，y为横格数
	返回值：void
*/
void mapshowView(int x, int y);
// -------------------- view --------------------

// -------------------- service --------------------

/*
	负责人：16鵺
	功能：	如果z=1
			从map1[0][0]到map1[x][y]全部初始化为0
			从map2[0][0]到map2[x][y]随机埋雷(个数参考需求文档，根据x和y来定)
			把相应坐标上的元素赋为相应值：
    				0~8表示周围八格里雷的数量
    				9代表该坐标有雷

			如果z=2
			从map1[0][0]到map1[x][y]全部初始化为0
			从map2[0][0]到map2[x][y]随机埋宝藏(个数为1个)
			从map2[0][0]到map2[x][y]随机埋雷(个数为56个)
			把相应坐标上的元素赋为相应值：
    				0~8表示周围八格里雷的数量
    				9代表该坐标有雷
				10表示该坐标有宝藏
	参数：x为纵坐标，y为横坐标，z为模式
	返回值：void
*/
void init(int x, int y, int z);

/*
	负责人：如今
	功能：	1.如果map1[i][j]为1则直接结束该函数
			2.如果map1[i][j]=0且k=1则把map1[i][j]改为1,同时如果sound=1则播放点开音效
			3.如果map1[i][j]=0且k=2则把map1[i][j]改为2,同时如果sound=1则播放插旗音效
			4.如果map1[i][j]=2且k=2则把map1[i][j]改为0,同时如果sound=1则播放拔旗音效
			5.实现自动展开

	参数:i为横坐标，j为纵坐标，k为鼠标行为（1表示左击，2表示右击）
	返回值：void
*/
void playerclick(int i, int j, int k, int x, int y);




/*
	负责人：季风
	功能：	综合map1和map2，在[0][0]到[x][y]的范围里判断是否点开了雷，点开了则return 1;
			综合map1和map2，在[0][0]到[x][y]的范围里判断是否点开所有0~8数字,点开了所有0~8数字则return 2
			以上都不成立则return 0;
	参数：x为纵格数，y为横格数
	返回值：int
*/
int isWin1(int x, int y);
//
void View();
/*
	负责人：峠
	功能：	综合map1和map2，在[0][0]到[12][30]的范围里判断点开的雷是否超过3个，超过三个则return 1
			综合map1和map2，在[0][0]到[12][30]的范围里判断是否点开宝藏，点开了则return 2
			以上都不成立则return 0;
	参数：x为纵格数，y为横格数
	返回值：int
*/
int isWin2(int x, int y);
void shuchu(int h, int minci, int x, int y);
void denglu();
char nicheng[30] = { 0 };
int quanjvjifen = 0;
int _check();
int  _delete();
void paihangView();
// -------------------- service --------------------


int main()
{
	denglu();
	controller();
	return 0;
}

void denglu() {
	initgraph(400, 400, 0);
	char ch1[100];
	outtextxy(105, 105, L"请输入昵称(不包含汉字)");
	char zi[50]{ 0 };
	int i = 0;
	int k = 0;
	while (1) {
		rectangle(100, 100, 300, 130);
		char ch = _getch();
		cleardevice();
		if (ch == '\r')
		{
			break;
		}
		else if (ch == '\b')
		{
			if (strlen(zi) > 0)
			{
				i--;
				zi[i] = '\0';
			}
		}
		else
		{
			zi[i] += ch;
			i++;
			k++;
		}
		outtextxy(105, 105, _T("        "));
		for (int j = 0; j < k; j++) {
			outtextxy(105 + j * 7, 105, zi[j]);
		}
	}
	strcpy(nicheng, zi);
	closegraph();
}
void controller()
{

	initgraph(1200, 800);
	while (1) {
		switch (flag) {
		case MY_MENU:
			menuView();
			break;
		case MY_SET:
			setView();
			break;
		case MY_GRADE:
			gradeView();
			break;
		case MY_MAKER:
			makerView();
			break;
		case MY_RULE:
			ruleView();
			break;
		case 6:
			paihangView();
			break;
		case MY_OVER:
			EndBatchDraw();
			closegraph();
			exit(0);
		}
	}
}

void menuView()
{
	//修改窗口句柄
	HWND hwnd = GetHWnd();
	SetWindowText(hwnd, L"扫雷");
	BeginBatchDraw();
	IMAGE img_1, img_2;
	IMAGE ppp;
	loadimage(&img_2, L"扫雷素材/2.png");
	loadimage(&img_1, L"扫雷素材/1.png");
	loadimage(&ppp, L"扫雷素材/奖杯.jpg", 50, 70);
	while (1)
	{
		cleardevice();
		//插入图片
		putimage(0, 0, &img_2);
		drawImg(226, 343, &img_1);
		drawImg(20, 600, &ppp);
		setbkcolor(BLACK);
		setbkmode(TRANSPARENT);
		int i;
		//画出圆角矩形

		for (i = 0; i <= 4; i++)
		{
			roundrect(850, 40 + i * 155, 1070, 130 + i * 155, 20, 20);
		}
		RECT rects[5] = {};
		const wchar_t* menu[5] = { L"进入游戏",L"游戏设置",L"游戏规则",L"开发者概述",L"退出游戏" };
		// 用已设定的圆角矩形方框设定drawtexts所需参数
		for (int i = 0; i <= 4; i++)
		{
			rects[i].left = 850;
			rects[i].top = 40 + i * 155;
			rects[i].right = 1070;
			rects[i].bottom = 130 + i * 155;
		}
		//在方框内补上文字
		settextstyle(50, 0, L"微软雅黑");
		for (int i = 0; i <= 4; i++)
		{
			drawtext(menu[i], &rects[i], DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_VCENTER | DT_CENTER);
		}

		//获取鼠标消息，并根据鼠标所在位置修改Flag的值
		ExMessage m;
		m = getmessage(EX_MOUSE);
		if (m.message == WM_LBUTTONDOWN && m.x >= 850 && m.x <= 1070 && m.y >= 40 && m.y <= 130)
		{
			flag = 1;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 850 && m.x <= 1070 && m.y >= 195 && m.y <= 285)
		{
			flag = 2;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 850 && m.x <= 1070 && m.y >= 350 && m.y <= 440)
		{
			flag = 4;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 850 && m.x <= 1070 && m.y >= 505 && m.y <= 595)
		{
			flag = 3;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 850 && m.x <= 1070 && m.y >= 660 && m.y <= 750)
		{
			flag = -1;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 20 && m.x <= 70 && m.y >= 600 && m.y <= 670)
		{
			flag = 6;
		}
		FlushBatchDraw();
		if (flag != 0) {
			break;
		}
	}
}

void gradeView()
{
	IMAGE img;
	ExMessage msg;
	loadimage(&img, L"./扫雷素材/难度背景2.0.png", 1200, 800);
	BeginBatchDraw();
	while (1)//持续进行难度选择的界面
	{
		cleardevice();
		peekmessage(&msg);
		if (winView == 1)//按照要求若winView为1则改变flag为0后停止循环
		{
			int* p = &flag;
			*p = 0;
			winView = 0;
			break;
		}
		setbkmode(TRANSPARENT);//将背景设置为透明模式
		putimage(0, 0, &img);
		//放置背景图片
		if (msg.x > 0 && msg.x < 300 && msg.y>114 && msg.y < 418 && msg.message == WM_LBUTTONDOWN) {
			gameView(9, 9, 1);
		}
		else if (msg.x > 415 && msg.x < 714 && msg.y>117 && msg.y < 421 && msg.message == WM_LBUTTONDOWN) {
			gameView(16, 16, 1);
		}
		else if (msg.x > 834 && msg.x < 1134 && msg.y>112 && msg.y < 415 && msg.message == WM_LBUTTONDOWN) {
			gameView(16, 20, 1);
		}
		else if (msg.x > 0 && msg.x < 780 && msg.y>525 && msg.y < 800 && msg.message == WM_LBUTTONDOWN) {
			gameView(12, 20, 2);
		}
		else if (msg.x > 1108 && msg.x < 1200 && msg.y>0 && msg.y < 70 && msg.message == WM_LBUTTONDOWN) {
			flag = 0; break;
		}
		FlushBatchDraw();
	}
}

void gameView(int x, int y, int z)
{
	if (z == 2) { xunbaomoshi = 1; }
	if (z == 1) { xunbaomoshi = 0; }
	IMAGE bk;
	IMAGE bk1;
	IMAGE bk2;
	IMAGE bk3;
	IMAGE bk4;
	IMAGE jingdian;
	IMAGE xunbao;
	IMAGE chongxin;
	IMAGE chongxin2;
	IMAGE chongzhi;
	IMAGE chongzhi2;
	ExMessage msg;
	if (xunbaomoshi == 0 && skin == 1) {
		loadimage(&bk, L"./扫雷素材/背景.png", 1200, 800);
	}
	else if (xunbaomoshi == 1) {
		loadimage(&bk, L"./扫雷素材/皮肤素材/寻宝模式/背景图.png", 1200, 800);
	}
	else if (xunbaomoshi == 0 && skin == 2) {
		loadimage(&bk, L"./扫雷素材/皮肤素材/花园主题皮肤/背景.png", 1200, 800);
	}
	else if (xunbaomoshi == 0 && skin == 3) {
		loadimage(&bk, L"./扫雷素材/皮肤素材/简约主题皮肤/背景.png", 1200, 800);
	}
	loadimage(&bk1, L"./扫雷素材/雷区背景.png", 257, 256);
	loadimage(&bk2, L"./扫雷素材/雷区背景.png", 453, 452);
	loadimage(&bk3, L"./扫雷素材/雷区背景.png", 565, 452);
	loadimage(&bk4, L"./扫雷素材/雷区背景.png", 565, 340);
	loadimage(&jingdian, L"./扫雷素材/经典.png", 120, 50);
	loadimage(&xunbao, L"./扫雷素材/寻宝.png");
	loadimage(&chongxin, L"./扫雷素材/重新.png");
	loadimage(&chongxin2, L"./扫雷素材/重新2.png");
	loadimage(&chongzhi, L"./扫雷素材/重置.png");
	loadimage(&chongzhi2, L"./扫雷素材/重置2.png");

	init(x, y, z);
	int result = 0;
	int i = -1, j = -1;
	int k = 0;
	int h = 0;
	while (1)
	{

		cleardevice();
		putimage(0, 0, &bk);
		if (z == 1) {
			putimage(10, 10, &jingdian);
		}
		else if (z == 2) {
			putimage(10, 10, &xunbao);
		}
		putimage(1075, 10, &chongzhi);
		putimage(1040, 100, &chongxin);

		if (z == 1) {
			if (y == 20) {
				putimage(158, 218, &bk3);
			}
			else if (y == 16) {
				putimage(158, 218, &bk2);
			}
			else if (y == 9) {
				putimage(158, 218, &bk1);
			}
		}
		else if (z == 2) {
			putimage(158, 218, &bk4);
		}

		peekmessage(&msg);
		if (msg.x > 1074 && msg.x < 1155 && msg.y>9 && msg.y < 45) {
			putimage(1075, 10, &chongzhi2);
			if (msg.message == WM_LBUTTONDOWN) {
				init(x, y, z);
			}
		}
		else if (msg.x > 1039 && msg.x < 1193 && msg.y>99 && msg.y < 137) {
			putimage(1040, 100, &chongxin2);
			if (msg.message == WM_LBUTTONDOWN) {
				Sleep(200);
				break;
			}
		}
		if (z == 1) {
			if (y == 20) {
				if (msg.x > 158 && msg.x < 720 && msg.y>218 && msg.y < 668) {
					if (msg.message == WM_LBUTTONDOWN) {
						i = (msg.x - 158) / 28;
						j = (msg.y - 218) / 28;
						playerclick(j, i, 1, x, y);
					}
					else if (msg.message == WM_RBUTTONDOWN) {
						i = (msg.x - 158) / 28;
						j = (msg.y - 218) / 28;
						playerclick(j, i, 2, x, y);
						if (sound == 1) {
							mciSendString(L"open ./扫雷素材/点按钮.mp3", 0, 0, 0);
							mciSendString(L"play ./扫雷素材/点按钮.mp3", 0, 0, 0);
							Sleep(350);
							mciSendString(L"close ./扫雷素材/点按钮.mp3", 0, 0, 0);
						}
					}

				}
			}
			else if (y == 16) {
				if (msg.x > 158 && msg.x < 607 && msg.y>218 && msg.y < 668) {
					if (msg.message == WM_LBUTTONDOWN) {
						i = (msg.x - 158) / 28;
						j = (msg.y - 218) / 28;
						playerclick(j, i, 1, x, y);
					}
					else if (msg.message == WM_RBUTTONDOWN) {
						i = (msg.x - 158) / 28;
						j = (msg.y - 218) / 28;
						playerclick(j, i, 2, x, y);
						if (sound == 1) {
							mciSendString(L"open ./扫雷素材/点按钮.mp3", 0, 0, 0);
							mciSendString(L"play ./扫雷素材/点按钮.mp3", 0, 0, 0);
							Sleep(350);
							mciSendString(L"close ./扫雷素材/点按钮.mp3", 0, 0, 0);
						}
					}
				}
			}
			else if (y == 9) {
				if (msg.x > 158 && msg.x < 412 && msg.y>218 && msg.y < 472) {
					if (msg.message == WM_LBUTTONDOWN) {
						i = (msg.x - 158) / 28;
						j = (msg.y - 218) / 28;
						playerclick(j, i, 1, x, y);
					}
					else if (msg.message == WM_RBUTTONDOWN) {
						i = (msg.x - 158) / 28;
						j = (msg.y - 218) / 28;
						playerclick(j, i, 2, x, y);
						if (sound == 1) {
							mciSendString(L"open ./扫雷素材/点按钮.mp3", 0, 0, 0);
							mciSendString(L"play ./扫雷素材/点按钮.mp3", 0, 0, 0);
							Sleep(350);
							mciSendString(L"close ./扫雷素材/点按钮.mp3", 0, 0, 0);
						}
					}
				}
			}
		}
		else if (z == 2) {
			if (msg.x > 158 && msg.x < 720 && msg.y>218 && msg.y < 555) {
				if (msg.message == WM_LBUTTONDOWN) {
					i = (msg.x - 158) / 28;
					j = (msg.y - 218) / 28;
					playerclick(j, i, 1, x, y);
				}
				else if (msg.message == WM_RBUTTONDOWN) {
					i = (msg.x - 158) / 28;
					j = (msg.y - 218) / 28;
					playerclick(j, i, 2, x, y);
					if (sound == 1) {
						mciSendString(L"open ./扫雷素材/点按钮.mp3", 0, 0, 0);
						mciSendString(L"play ./扫雷素材/点按钮.mp3", 0, 0, 0);
						Sleep(350);
						mciSendString(L"close ./扫雷素材/点按钮.mp3", 0, 0, 0);
					}
				}
			}
		}
		mapshowView(x, y);
		if (z == 1) {
			result = isWin1(x, y);
		}
		else if (z == 2) {
			result = isWin2(x, y);
		}
		if (result == 1) {
			FlushBatchDraw();
			mapshowView(x, y);
			if (sound == 1) {
				mciSendString(L"open ./爆炸.mp3", 0, 0, 0);
				mciSendString(L"play ./爆炸.mp3", 0, 0, 0);
				Sleep(2000);
				mciSendString(L"close ./爆炸.mp3", 0, 0, 0);
			}
			gameoverView(result);
			if (winView == 1) { break; }
			if (winView == 0) { init(x, y, z); Sleep(200); }
		}
		else if (result == 2) {
			FlushBatchDraw();
			mapshowView(x, y);
			Sleep(1000);
			gameoverView(result);
			if (winView == 1) { break; }
			if (winView == 0) { init(x, y, z); Sleep(200); }
		}
		k = quanjvjifen;
		for (int j = 0; k > 0; j++) {
			outtextxy(155 - j * 20, 105, k % 10 + 48);
			k /= 10;
		}

		if (Timer(1000, 0))quanjvjifen--;
		FlushBatchDraw();
	}
}

void setView()
{
	IMAGE img;
	loadimage(&img, L"./扫雷素材/设置背景.png");
	int count = 1;//数取余实现开关
	while (1)
	{
		cleardevice();
		setbkcolor(WHITE);
		setfillcolor(BLACK);
		BeginBatchDraw();
		putimage(0, 0, &img);//放置背景图片
		sound = 1;
		ExMessage m;
		m = getmessage(EX_MOUSE);
		if (m.message == WM_LBUTTONDOWN)
		{
			if (m.x >= 400 && m.x <= 810 && m.y >= 234 && m.y <= 358)
			{
				count += 1;
				if (count % 2 == 0)
				{
					sound = 1;
					solidroundrect(633, 272, 683, 311, 10, 10);
				}
				else
				{
					sound = 0;
					solidroundrect(706, 270, 765, 311, 10, 10);
				}
			}
			if (m.x >= 400 && m.x <= 810 && m.y >= 464 && m.y <= 587)
			{
				skinView();
			}
			if (m.x >= 973 && m.x <= 1185 && m.y >= 722 && m.y <= 786)
			{
				flag = 0;
				break;
			}
		}
		FlushBatchDraw();
	}
}
void skinView()
{
	IMAGE img;
	IMAGE img_1;
	IMAGE img_2;
	IMAGE img_3;
	IMAGE img_4;
	loadimage(&img, L"./扫雷素材/2.png", 1200, 800);
	loadimage(&img_1, L"./扫雷素材/皮肤选择/经典.png", 150, 150);
	loadimage(&img_2, L"./扫雷素材/皮肤选择/花园.png", 150, 150);
	loadimage(&img_3, L"./扫雷素材/皮肤选择/简约.png", 150, 150);
	loadimage(&img_4, L"./扫雷素材/皮肤选择/返回.png", 50, 50);
	BeginBatchDraw();
	while (1)
	{
		cleardevice();
		putimage(0, 0, &img);
		putimage(150, 300, &img_1);
		putimage(525, 300, &img_2);
		putimage(900, 300, &img_3);
		putimage(0, 0, &img_4);
		ExMessage m;
		m = getmessage(EX_MOUSE);
		if (m.message == WM_LBUTTONDOWN && m.x >= 150 && m.x <= 300 && m.y >= 250 && m.y <= 400)
		{
			skin = 1;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 525 && m.x <= 675 && m.y >= 250 && m.y <= 400)
		{
			skin = 2;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 900 && m.x <= 1050 && m.y >= 250 && m.y <= 400)
		{
			skin = 3;
		}
		if (m.message == WM_LBUTTONDOWN && m.x >= 0 && m.x <= 50 && m.y >= 0 && m.y <= 50)
		{
			Sleep(200);
			break;
		}
		FlushBatchDraw();
	}
}

void makerView() {
	IMAGE imge;
	ExMessage msg;
	loadimage(&imge, L"./扫雷素材/背景.png", 1200, 800);
	cleardevice();
	putimage(0, 0, &imge);
	peekmessage(&msg);
	settextstyle(20, 0, _T("宋体"));
	outtextxy(10, 300, _T("1 组长季风 \n2 副组长 711 \n3 信息兼会议记录官 峠\n4 监督官 东风未散\n5 产品经理 云水\n6 技术官 如今\n7技术官 忆往\n8 技术官 16鵺"));
	outtextxy(500, 400, _T("返回"));
	FlushBatchDraw();
	while (1) {
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			if (m.x >= 500 && m.x <= 550 && m.y >= 350 && m.y <= 380) {
				// 翻页操作，这里可根据实际需求添加翻页后的内容展示
				cleardevice();
				outtextxy(500, 300, _T("Codechampion共有八人，每一个人都各司其职，认真完成自己的任务,名不显示心不朽，再挑灯火看文章"));
				outtextxy(500, 400, _T("返回"));
			}
			else if (m.x >= 500 && m.x <= 550 && m.y >= 400 && m.y <= 430) {
				flag = 0;
				break;
			}
		}
	}
}

void ruleView()
{
	IMAGE imge;
	ExMessage msg;
	loadimage(&imge, L"./扫雷素材/扫雷规则书.png", 1200, 800);
	while (1)
	{
		cleardevice();
		putimage(0, 0, &imge);
		peekmessage(&msg);
		if (msg.x > 1040 && msg.x < 1200 && msg.y>685 && msg.y < 800 && msg.message == WM_LBUTTONDOWN) {
			flag = 0;
			break;
		}
		FlushBatchDraw();
	}
}

void gameoverView(int x)
{
	IMAGE fail;
	IMAGE win;
	BeginBatchDraw();
	loadimage(&fail, L"./扫雷素材/失败界面.png", 1200, 800);
	loadimage(&win, L"./扫雷素材/胜利界面.png", 1200, 800);
	if (x == 1) {
		quanjvjifen = 0;
	}
	while (1) {

		ExMessage m;
		m = getmessage(EX_MOUSE);
		if (x == 1) {

			cleardevice();
			putimage(0, 0, &fail);
			if (sound == 1) {
				mciSendString(L"open 音乐/失败.mp3 alias fail", NULL, 0, NULL);
				mciSendString(L"play fail repeat", NULL, 0, NULL);
				mciSendString(L"setaudio fail volume to 500", NULL, 0, NULL);//失败音效
			}
			if (m.message == WM_LBUTTONDOWN && m.x >= 1060 && m.x <= 1186 && m.y >= 19 && m.y <= 94)//返回
			{
				mciSendString(L"close fail", NULL, 0, NULL);
				winView = 1;
				break;
			}
			if (m.message == WM_LBUTTONDOWN && m.x >= 21 && m.x <= 382 && m.y >= 19 && m.y <= 94)//重新开始
			{
				mciSendString(L"close fail", NULL, 0, NULL);
				winView = 0;
				break;
			}
		}
		else if (x == 2) {
			putimage(0, 0, &win);
			if (sound == 1) {
				mciSendString(L"open 音乐/胜利.mp3 alias win", NULL, 0, NULL);
				mciSendString(L"play win repeat", NULL, 0, NULL);
				mciSendString(L"setaudio win volume to 500", NULL, 0, NULL);//胜利音效
			}
			if (m.message == WM_LBUTTONDOWN && m.x >= 1060 && m.x <= 1186 && m.y >= 19 && m.y <= 94)//返回
			{
				mciSendString(L"close win", NULL, 0, NULL);
				winView = 1;
				break;
			}
		}
		FlushBatchDraw();
	}

	char jifen[20] = { 0 };
	int k = quanjvjifen;
	for (int j = 0; k > 0; j++) {
		jifen[j] = k % 10 + 48;
		k /= 10;
	}
	if (k == 0) {
		jifen[0] = '1';
	}
	char ch2[128];
	FILE* fp = fopen("./昵称.txt", "a");
	FILE* fpt = fopen("./积分.txt", "a");
	printf("%d\n", ch2[0]);
	fputs("\n", fp);
	fputs(nicheng, fp);
	fputs("\n", fpt);
	fputs(jifen, fpt);
	fclose(fp);
	fclose(fpt);
	fp = fopen("./昵称.txt", "a");
	fpt = fopen("./积分.txt", "a");
	FILE* fp1 = fopen("./4.txt", "r");
	FILE* fpt1 = fopen("./1.txt", "r");
	fclose(fp);
	fclose(fpt);
	fclose(fp1);
	fclose(fpt1);
	if (_check() > 10) {
		int n = _delete();
		if (n == 12) {
			int i = 0;
			FILE* fpt = fopen("./昵称.txt", "w");
			FILE* fp = fopen("./1.txt", "r");
			FILE* fpt1 = fopen("./积分.txt", "w");
			FILE* fp1 = fopen("./4.txt", "r");
			fclose(fpt);
			fclose(fp);
			fclose(fpt1);
			fclose(fp1);
			fpt = fopen("./昵称.txt", "w");
			fp = fopen("./1.txt", "r");
			fpt1 = fopen("./积分.txt", "w");
			fp1 = fopen("./4.txt", "r");
			char ch2[128];
			while (!feof(fp)) {
				i++;
				if (n == 12 && i == 11) {
					fgets(ch2, 128, fp);
					int z = 0;
					for (z = 0; ch2[z] != '\n'; z++) {}
					ch2[z] = '\0';
					fputs(ch2, fpt);
					break;
				}
				fputs(fgets(ch2, 128, fp), fpt);
			}
			char ch1[128];
			i = 0;
			while (!feof(fp1)) {
				i++;
				if (n == 12 && i == 11) {
					fgets(ch2, 128, fp1);
					int z = 0;
					for (z = 0; ch2[z] != '\n'; z++) {}
					ch2[z] = '\0';
					fputs(ch2, fpt1);
					break;
				}
				fputs(fgets(ch1, 128, fp1), fpt1);
			}
			fclose(fpt);
			fclose(fp);
			fclose(fpt1);
			fclose(fp1);
		}
		else {
			char oldname[] = { "./昵称.txt" };
			char newname[] = { "./2.txt" };
			rename(oldname, newname);
			char oldname1[] = { "./1.txt" };
			char newname1[] = { "./昵称.txt" };
			rename(oldname1, newname1);
			char oldname2[] = { "./2.txt" };
			char newname2[] = { "./1.txt" };
			rename(oldname2, newname2);

			char oldname3[] = { "./积分.txt" };
			char newname3[] = { "./3.txt" };
			rename(oldname3, newname3);
			char oldname4[] = { "./4.txt" };
			char newname4[] = { "./积分.txt" };
			rename(oldname4, newname4);
			char oldname5[] = { "./3.txt" };
			char newname5[] = { "./4.txt" };
			rename(oldname5, newname5);
		}
	}
}

void mapshowView(int x, int y)
{
	IMAGE leikuai;
	IMAGE qizi;
	IMAGE leikuai2;
	IMAGE lei;
	IMAGE shuzi1;
	IMAGE shuzi2;
	IMAGE shuzi3;
	IMAGE shuzi4;
	IMAGE shuzi5;
	IMAGE shuzi6;
	IMAGE shuzi7;
	IMAGE shuzi8;
	IMAGE baozang;
	loadimage(&qizi, L"./扫雷素材/雷块图标/棋子.png", 28, 28);
	loadimage(&shuzi1, L"./扫雷素材/雷块数字/1.png", 20, 20);
	loadimage(&shuzi2, L"./扫雷素材/雷块数字/2.png", 20, 20);
	loadimage(&shuzi3, L"./扫雷素材/雷块数字/3.png", 20, 20);
	loadimage(&shuzi4, L"./扫雷素材/雷块数字/4.png", 20, 20);
	loadimage(&shuzi5, L"./扫雷素材/雷块数字/5.png", 20, 20);
	loadimage(&shuzi6, L"./扫雷素材/雷块数字/6.png", 20, 20);
	loadimage(&shuzi7, L"./扫雷素材/雷块数字/7.png", 20, 20);
	loadimage(&shuzi8, L"./扫雷素材/雷块数字/8.png", 20, 20);
	if (xunbaomoshi == 1) {
		loadimage(&leikuai, L"./扫雷素材/皮肤素材/寻宝模式/未点开的格子.png", 28, 28);
		loadimage(&leikuai2, L"./扫雷素材/皮肤素材/寻宝模式/点开数字为0.png", 28, 28);
		loadimage(&lei, L"./扫雷素材/皮肤素材/寻宝模式/雷.png", 28, 28);
		loadimage(&baozang, L"./扫雷素材/宝藏.png", 26, 26);
	}
	else if (xunbaomoshi == 0 && skin == 1) {
		loadimage(&leikuai, L"./扫雷素材/雷块状态/默认.png", 28, 28);
		loadimage(&leikuai2, L"./扫雷素材/雷块状态/按下.png", 28, 28);
		loadimage(&lei, L"./扫雷素材/雷块图标/雷.png", 28, 28);
	}
	else if (xunbaomoshi == 0 && skin == 2) {
		loadimage(&leikuai, L"./扫雷素材/皮肤素材/花园主题皮肤/未点击的格子.png", 28, 28);
		loadimage(&leikuai2, L"./扫雷素材/皮肤素材/花园主题皮肤/点击后没有数字的格子.png", 28, 28);
		loadimage(&lei, L"./扫雷素材/皮肤素材/花园主题皮肤/雷.png", 28, 28);
	}
	else if (xunbaomoshi == 0 && skin == 3) {
		loadimage(&leikuai, L"./扫雷素材/皮肤素材/简约主题皮肤/未点击的格子.png", 28, 28);
		loadimage(&leikuai2, L"./扫雷素材/皮肤素材/简约主题皮肤/点开没有数字的格子.png", 28, 28);
		loadimage(&lei, L"./扫雷素材/皮肤素材/简约主题皮肤/雷.png", 28, 28);
	}
	int k = 0;
	for (int i = 0; i < x; i++) {

		for (int j = 0; j < y; j++) {

			if (map1[i][j] == 0) {
				putimage(160 + j * 28, 220 + i * 28, &leikuai);
			}
			else if (map1[i][j] == 2) {
				putimage(160 + j * 28, 220 + i * 28, &leikuai);
				drawImg(162 + j * 28, 222 + i * 28, &qizi);
			}
			else if (map1[i][j] == 1 && map2[i][j] == 9) {
				putimage(160 + j * 28, 220 + i * 28, &leikuai2);
				drawImg(160 + j * 28, 220 + i * 28, &lei);
			}
			else if (map1[i][j] == 1 && map2[i][j] > 0 && map2[i][j] < 9) {
				k = map2[i][j];
				switch (k) {
				case 1:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi1);
					break;
				case 2:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi2);
					break;
				case 3:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi3);
					break;
				case 4:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi4);
					break;
				case 5:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi5);
					break;
				case 6:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi6);
					break;
				case 7:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi7);
					break;
				case 8:
					putimage(160 + j * 28, 220 + i * 28, &leikuai2);
					drawImg(164 + j * 28, 224 + i * 28, &shuzi8);
					break;
				}
			}
			else if (map1[i][j] == 1 && map2[i][j] == 10) {
				putimage(160 + j * 28, 220 + i * 28, &leikuai2);
				drawImg(160 + j * 28, 220 + i * 28, &baozang);
			}
			else if (map1[i][j] == 1 && map2[i][j] == 0) {
				putimage(160 + j * 28, 220 + i * 28, &leikuai2);
			}

		}
	}
}

void init(int x, int y, int z)
{

	int i, k, r, c;
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			map1[i][j] = 0;
		}
	}
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			map2[i][j] = 0;
		}
	}
	srand((unsigned)time(NULL));
	if (z == 1)
	{
		if (x == 9 && y == 9)
		{
			quanjvjifen = 100;
			for (i = 0; i < 10; )
			{
				r = rand() % 9;
				c = rand() % 9;
				if (map2[r][c] == 0)
				{
					map2[r][c] = 9;
				}i++;
			}
			for (i = 0; i < x; i++)
			{
				for (k = 0; k < y; k++)
				{
					if (map2[i][k] != 9) {
						if (i - 1 >= 0 && k - 1 >= 0) {
							if (map2[i - 1][k - 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i >= 0 && k - 1 >= 0) {
							if (map2[i][k - 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i - 1 >= 0 && k >= 0) {
							if (map2[i - 1][k] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k + 1 >= 0) {
							if (map2[i + 1][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i >= 0 && k + 1 >= 0) {
							if (map2[i][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k >= 0) {
							if (map2[i + 1][k] == 9) {
								map2[i][k]++;
							}
						}
						if (i - 1 >= 0 && k + 1 >= 0) {
							if (map2[i - 1][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k - 1 >= 0) {
							if (map2[i + 1][k - 1] == 9) {
								map2[i][k]++;
							}
						}
					}
				}
			}
			for (i = 0; i < x; i++)
			{
				for (k = 0; k < y; k++)
				{
					printf("%2d ", map2[i][k]);
				}
				printf("\n");
			}
		}
		if (x == 16 && y == 16)
		{
			quanjvjifen = 300;
			for (i = 0; i < 30; )
			{
				r = rand() % 16;
				c = rand() % 16;
				if (map2[r][c] == 0)
				{
					map2[r][c] = 9;
				}i++;
			}
			for (i = 0; i < x; i++)
			{
				for (k = 0; k < y; k++)
				{
					if (map2[i][k] != 9) {
						if (i - 1 >= 0 && k - 1 >= 0) {
							if (map2[i - 1][k - 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i >= 0 && k - 1 >= 0) {
							if (map2[i][k - 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i - 1 >= 0 && k >= 0) {
							if (map2[i - 1][k] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k + 1 >= 0) {
							if (map2[i + 1][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i >= 0 && k + 1 >= 0) {
							if (map2[i][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k >= 0) {
							if (map2[i + 1][k] == 9) {
								map2[i][k]++;
							}
						}
						if (i - 1 >= 0 && k + 1 >= 0) {
							if (map2[i - 1][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k - 1 >= 0) {
							if (map2[i + 1][k - 1] == 9) {
								map2[i][k]++;
							}
						}
					}
				}
			}
			for (i = 0; i < x; i++)
			{
				for (k = 0; k < y; k++)
				{
					printf("%2d ", map2[i][k]);
				}
				printf("\n");
			}
		}
		if (x == 16 && y == 20)
		{
			quanjvjifen = 700;
			for (i = 0; i < 70; )
			{
				r = rand() % 16;
				c = rand() % 20;
				if (map2[r][c] == 0)
				{
					map2[r][c] = 9;
				}i++;
			}
			for (i = 0; i < x; i++)
			{
				for (k = 0; k < y; k++)
				{
					if (map2[i][k] != 9) {
						if (i - 1 >= 0 && k - 1 >= 0) {
							if (map2[i - 1][k - 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i >= 0 && k - 1 >= 0) {
							if (map2[i][k - 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i - 1 >= 0 && k >= 0) {
							if (map2[i - 1][k] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k + 1 >= 0) {
							if (map2[i + 1][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i >= 0 && k + 1 >= 0) {
							if (map2[i][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k >= 0) {
							if (map2[i + 1][k] == 9) {
								map2[i][k]++;
							}
						}
						if (i - 1 >= 0 && k + 1 >= 0) {
							if (map2[i - 1][k + 1] == 9) {
								map2[i][k]++;
							}
						}
						if (i + 1 >= 0 && k - 1 >= 0) {
							if (map2[i + 1][k - 1] == 9) {
								map2[i][k]++;
							}
						}
					}
				}
			}
			for (i = 0; i < x; i++)
			{
				for (k = 0; k < y; k++)
				{
					printf("%2d ", map2[i][k]);
				}
				printf("\n");
			}
		}
	}
	if (z == 2)
	{
		for (i = 0; i < 56; )
		{
			quanjvjifen = 560;
			r = rand() % 12;
			c = rand() % 20;
			if (map2[r][c] == 0)
			{
				map2[r][c] = 9;
			}i++;
		}

		for (i = 0; i < x; i++)
		{
			for (k = 0; k < y; k++)
			{
				if (map2[i][k] != 9) {
					if (i - 1 >= 0 && k - 1 >= 0) {
						if (map2[i - 1][k - 1] == 9) {
							map2[i][k]++;
						}
					}
					if (i >= 0 && k - 1 >= 0) {
						if (map2[i][k - 1] == 9) {
							map2[i][k]++;
						}
					}
					if (i - 1 >= 0 && k >= 0) {
						if (map2[i - 1][k] == 9) {
							map2[i][k]++;
						}
					}
					if (i + 1 >= 0 && k + 1 >= 0) {
						if (map2[i + 1][k + 1] == 9) {
							map2[i][k]++;
						}
					}
					if (i >= 0 && k + 1 >= 0) {
						if (map2[i][k + 1] == 9) {
							map2[i][k]++;
						}
					}
					if (i + 1 >= 0 && k >= 0) {
						if (map2[i + 1][k] == 9) {
							map2[i][k]++;
						}
					}
					if (i - 1 >= 0 && k + 1 >= 0) {
						if (map2[i - 1][k + 1] == 9) {
							map2[i][k]++;
						}
					}
					if (i + 1 >= 0 && k - 1 >= 0) {
						if (map2[i + 1][k - 1] == 9) {
							map2[i][k]++;
						}
					}
				}
			}
		}
		r = rand() % 12;
		c = rand() % 20;
		map2[r][c] = 10;
		for (i = 0; i < x; i++)
		{
			for (k = 0; k < y; k++)
			{
				printf("%2d ", map2[i][k]);
			}
			printf("\n");
		}
	}
}

void playerclick(int i, int j, int k, int x, int y) {
	if (k == 1 && map1[i][j] == 0) {
		map1[i][j] = 1;
		boom(i, j, x, y);
		quanjvjifen += 2;
		if (sound == 1) {
			mciSendString(L"open ./扫雷素材/点按钮.mp3", 0, 0, 0);
			mciSendString(L"play ./扫雷素材/点按钮.mp3", 0, 0, 0);
			Sleep(400);
			mciSendString(L"close ./扫雷素材/点按钮.mp3", 0, 0, 0);
		}
	}
	else if (k == 2) {
		if (map1[i][j] == 0) {
			map1[i][j] = 2;
		}
		else if (map1[i][j] == 2) {
			map1[i][j] = 0;
		}

	}

}

int isWin1(int x, int y)
{
	int a = 0, b = 0;//a为点开的雷数，b为 未点开的0~8数字的数量
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {//遍历棋盘

			if (map1[i][j] == 1 && map2[i][j] == 9) {//如果点开有雷格子
				a++;
			}
			else if (map1[i][j] != 1 && map2[i][j] != 9) {//如果有未点开的0~8数字
				b++;
			}
		}
	}

	if (a > 0) {
		return 1;//至少点开一个雷，游戏失败
	}
	else if (b == 0)
		// 所有0~8数字的格子都被点开，游戏胜利
	{
		return 2;
	}
	else {
		return 0;//游戏继续
	}
}

int isWin2(int x, int y)
{
	int a = 0, b = 0;
	//a为雷数，b为宝藏数。
	for (int i = 0; i <= x; i++) {
		for (int j = 0; j <= y; j++) {//遍历棋盘
			if (map2[i][j] == 9 && map1[i][j] == 1) {//找已知雷
				a++;
			}
			else if (map2[i][j] == 10 && map1[i][j] == 1) {//找已知宝藏
				b++;
			}
		}
	}

	if (a > 3 && b < 1) {
		return 1;
	}
	else if (b >= 1) {
		return 2;
	}
	else if (a != 4 && b != 1) {
		return 0;
	}
}

void boom(int i, int j, int x, int y) {
	if (map2[i][j] == 0) {
		for (int r = i - 1; r <= i + 1; r++) {
			for (int c = j - 1; c <= j + 1; c++) {
				if (r >= 0 && r < x && c >= 0 && c < y && map1[r][c] == 0 && map2[r][c] != 9 && map2[r][c] != 10) {
					map1[r][c] = 1;
					boom(r, c, x, y);
				}
			}
		}
	}
}
int Timer(int duration, int id) {
	static int starTime[100];
	int endTime = clock();
	if (endTime - starTime[id] > duration) {
		starTime[id] = endTime;

		return 1;
	}
	return 0;
}

int _check() {
	int i = 0;
	char ch[10];
	FILE* fp = fopen("./积分.txt", "r");
	if (fp == NULL) {
		perror("open failed:");
		return 0;
	}
	while (!feof(fp)) {
		fgets(ch, 10, fp);
		i++;
	}
	fclose(fp);
	i--;
	return i;
}

int _delete() {
	int i = 0;
	int j = 0;
	int ak[10];
	int jifen[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	FILE* fp9 = fopen("./积分.txt", "r");
	if (fp9 == NULL) {
		perror("open failed:");
		return 0;
	}
	char ch[10];
	while (!feof(fp9)) {
		int shuliangji = 1;
		fgets(ch, 10, fp9);
		for (i = 0; ch[i] != '\0' && ch[i] != '\n'; i++) {
			ak[i] = (int)ch[i] - 48;
		}
		i--;
		for (i; i >= 0; i--) {
			jifen[j] += ak[i] * shuliangji;
			shuliangji *= 10;
		}
		j++;
	}
	fflush(fp9);
	fclose(fp9);
	int minci[11][2];
	int max = 0;
	for (int x = 0; x < 11; x++) {
		max = 0;
		for (i = 0; i < 11 + 1; i++) {
			if (max < jifen[i]) {
				max = jifen[i];
				minci[x][1] = i;
			}
		}
		jifen[minci[x][1]] = 0;
		minci[x][0] = max;
	}

	int n = minci[10][1] + 1;
	printf("%d\n", n);
	i = 0;
	FILE* fp = fopen("./昵称.txt", "r");
	FILE* fpt = fopen("./1.txt", "w");
	FILE* fp1 = fopen("./积分.txt", "r");
	FILE* fpt1 = fopen("./4.txt", "w");
	char ch2[128];
	while (!feof(fp)) {
		i++;
		//if (n == 12 && i == 11) {
		//	fgets(ch2, 128, fp);
		//	int z = 0;
		//	for (z = 0; ch2[z] != '\n'; z++) {}
		//	ch2[z] = ' ';
		//	fputs(ch2, fpt);
		//	continue;
		//}
		if (i == n)fgets(ch2, 128, fp);
		else fputs(fgets(ch2, 128, fp), fpt);
	}
	char ch1[128];
	i = 0;
	while (!feof(fp1)) {
		i++;
		//if (n == 12 && i == 11) {
		//	fgets(ch2, 128, fp);
		//	int z = 0;
		//	for (z = 0; ch2[z] != '\n'; z++) {}
		//	ch2[z] = ' ';
		//	fputs(ch2, fpt);
		//	continue;
		//}
		if (i == n)fgets(ch1, 128, fp1);
		else fputs(fgets(ch1, 128, fp1), fpt1);
	}
	fclose(fpt);
	fclose(fp);
	fclose(fpt1);
	fclose(fp1);
	return n;
}
void shuchu(int h, int minci, int x, int y) {
	int i = 0;
	char nicheng[10];
	char jifen[10];
	char minci1[2];
	minci1[0] = minci + 48;
	FILE* fp = fopen("./积分.txt", "r");
	FILE* fpt = fopen("./昵称.txt", "r");
	if (fpt == NULL) {
		perror("open failed:");
		return;
	}
	while (!feof(fp)) {
		i++;

		fgets(jifen, 10, fp);
		fgets(nicheng, 10, fpt);
		if (i == h + 1) {
			outtextxy(x, y, minci1[0]);
			int a = 0;
			for (a = 0; nicheng[a] != '\n' && nicheng[a] != '\0'; a++);
			nicheng[a] = '\0';
			for (int j = 0; j < strlen(nicheng); j++) {
				if (nicheng[j] != '\n') {
					outtextxy(x + 50 + j * 40, y, nicheng[j]);
				}
			}
			for (a = 0; jifen[a] != '\n' && jifen[a] != '\0'; a++);
			nicheng[a] = '\0';
			for (int j = 0; j < strlen(jifen); j++) {
				if (jifen[j] != '\n') {
					outtextxy(x + 450 + j * 40, y, jifen[j]);
				}
			}
		}
	}
	fclose(fp);
	fclose(fpt);
}
void View() {

	int amount = _check();
	int i = 0;
	int j = 0;
	int ak[10];
	int jifen[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	FILE* fp = fopen("./积分.txt", "r");
	if (fp == NULL) {
		perror("open failed:");
		return;
	}
	char ch[10];
	while (!feof(fp)) {
		int shuliangji = 1;
		fgets(ch, 10, fp);
		for (i = 0; ch[i] != '\0' && ch[i] != '\n'; i++) {
			ak[i] = (int)ch[i] - 48;
		}
		i--;
		for (i; i >= 0; i--) {
			jifen[j] += ak[i] * shuliangji;
			shuliangji *= 10;
		}
		j++;
	}
	fclose(fp);
	int minci[12][2];
	int max = 0;
	for (int x = 0; x < amount; x++) {
		max = 0;
		for (i = 0; i < amount + 1; i++) {
			if (max < jifen[i]) {
				max = jifen[i];
				minci[x][1] = i;
			}
		}
		jifen[minci[x][1]] = 0;
		minci[x][0] = max;
	}
	for (i = 0; i < amount; i++)shuchu(minci[i][1], i + 1, 100, 10 + i * 50);
}
void paihangView() {
	IMAGE bk;
	IMAGE bk2;
	BeginBatchDraw();
	loadimage(&bk, L"./白.png", 1200, 800);
	loadimage(&bk2, L"./扫雷素材/背景.png", 200, 100);
	cleardevice();
	putimage(0, 0, &bk);
	putimage(1000, 500, &bk2);
	FlushBatchDraw();
	FILE* fp = fopen("./积分.txt", "r");
	FILE* fpt = fopen("./昵称.txt", "r");
	FILE* fp1 = fopen("./4.txt", "r");
	FILE* fpt1 = fopen("./1.txt", "r");
	fclose(fp);
	fclose(fpt);
	fclose(fp1);
	fclose(fpt1);
	if (_check() > 10) {
		_delete();
		char oldname[] = { "./昵称.txt" };
		char newname[] = { "./2.txt" };
		rename(oldname, newname);

		char oldname1[] = { "./1.txt" };
		char newname1[] = { "./昵称.txt" };
		rename(oldname1, newname1);

		char oldname2[] = { "./2.txt" };
		char newname2[] = { "./1.txt" };
		rename(oldname2, newname2);


		char oldname3[] = { "./积分.txt" };
		char newname3[] = { "./3.txt" };
		rename(oldname3, newname3);

		char oldname4[] = { "./4.txt" };
		char newname4[] = { "./积分.txt" };
		rename(oldname4, newname4);

		char oldname5[] = { "./3.txt" };
		char newname5[] = { "./4.txt" };
		rename(oldname5, newname5);

	}

	View();
	FlushBatchDraw();

	ExMessage m;
	while (1) {
		m = getmessage(EX_MOUSE);
		if (m.message == WM_LBUTTONDOWN && m.x >= 1000 && m.x <= 1200 && m.y >= 500 && m.y <= 600)
		{
			flag = 0; break;
		}

	}
}