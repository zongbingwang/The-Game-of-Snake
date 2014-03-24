//系统函数库
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//EasyX图形库
#include <graphics.h>

//pthread多线程运行库
#include <pthread.h>

//API函数
#pragma comment(lib,"Winmm.lib")


//////////////宏定义参数

#define		ScrWide		800					//界面宽度
#define		ScrHigh		600					//界面高度
#define		ScrBlank	100					//屏幕上端状态栏留空

#define		Body		20					//蛇身宽度
#define		WallWide	40					//墙宽度

#define		Up			0					//定义方向参数
#define		Right		1
#define		Down		2
#define		Left		3
#define		R2U			4
#define		D2R			5
#define		L2D			6
#define		U2L			7
#define		L2U			8
#define		U2R			9
#define		R2D			10
#define		D2L			11

#define		Menu		0					//定义游戏阶段
#define		Play42		1
#define		Dead		2
#define		Help		3
#define		Stop		4

#define		X			0					//定义坐标轴参数
#define		Y			1

#define		Gre			0					//定义蛇颜色参数
#define		Blu			1

#define		Wall		0					//定义碰撞物参数
#define		Snake		1
#define     None		2

#define		LifeUp		0					//定义道具参数
#define		SpdUp		1
#define		Replace		2
#define		Spdmax		3
#define		Unbeatable  4
#define		Uncontrol	5

#define		MaxLife		25					//定义极限参数
#define		MaxSpd		100

#define		Red			0					//定义HP血槽颜色参数
#define		Yellow		1
#define		Green		2

#define		Bgm_menu	-1
#define		Bgm_start	-2
#define		Bgm_dead	-3


//////////////函数申明

void change_phase();
void loadimage();
void restart();
void data_shift(int, int);
void draw_snake();
void cmd(int);
void prop_replace(int);
void state_cmd();

int dstchax(int, int);
int judge_point();
int judge_snake(int);
int judge_direct(int);

//////////////数据结构定义

//蛇相关数据
struct snake
{
IMAGE snahead[4];							//蛇头图像
IMAGE snabody[2][4];						//蛇身前进图像
IMAGE snaturn[2][8];						//蛇身转弯图像
IMAGE snatail[4];							//蛇尾图像
int snadst[30];								//蛇各节点坐标，共六位，高位(三位)为x坐标，低位(三位)为y坐标
int snadstdir[30];							//蛇各节点方向
int state;									//状态
int life;									//hp
int direction;								//行进方向
int speed;									//行进速度
}sna;

//图片相关数据
struct picture
{
IMAGE UI_Menu;
IMAGE UI_Buttom;
IMAGE UI_background;
IMAGE UI_Dead;
IMAGE UI_Help;
IMAGE UI_HpBar;
IMAGE UI_HpBarMask;
IMAGE UI_HP[3];
IMAGE UI_SpeedBar;
IMAGE UI_SpeedBarMask;
IMAGE UI_Speed;
IMAGE pro_pic[6];
}pic;

//道具相关数据
struct pro
{
int state;
int dst;
}prop[3];

//////////////全局变量
int game_phase,score;
MOUSEMSG mouse;

//////////////辅助函数

//点坐标转换
int dstchax(int dst, int xy)
{
	return (xy?dst%1000:dst/1000);
}

//按键方向冲突判定
int judge_direct(int keyin)
{
	if  (keyin == sna.snadstdir[0] ||
		(keyin == Right && sna.snadstdir[0] == Left) ||
		(keyin == Left && sna.snadstdir[0] == Right) ||
		(keyin == Up && sna.snadstdir[0] == Down) ||
		(keyin == Down && sna.snadstdir[0] == Up))
		return 0;
	else
		return 1;
}

//碰撞判断
int judge_point()
{
	int tmp;
	switch (sna.direction)
	{
	case Up:
		tmp = sna.snadst[0] - Body;
		break;
	case Right:
		tmp = sna.snadst[0] + Body*1000;
		break;
	case Left:
		tmp = sna.snadst[0] - Body*1000;
		break;
	case Down:
		tmp = sna.snadst[0] + Body;
		break;
	}
	if (dstchax(tmp, X) +Body > ScrWide - WallWide || dstchax(tmp, X) < WallWide ||
		dstchax(tmp, Y) +Body < WallWide + ScrBlank || dstchax(tmp, Y) + Body > ScrHigh - WallWide )
	{
		return Wall;
	}
	else 
		if (judge_snake(tmp))
	{
		return Snake;
	}
		else
	{
		return None;
	}
}

//判定鼠标位置
int judge_mouse(int buttom)
{
	if (game_phase == Menu)
	{
		switch (buttom)
		{
		case 0:
		if (mouse.x>350 && mouse.x<620 && mouse.y>250 && mouse.y<350)		return 1;
		else																return 0;
		case 1:
		if (mouse.x>350 && mouse.x<620 && mouse.y>350 && mouse.y<450)		return 1;
		else																return 0;
		case 2:
		if (mouse.x>350 && mouse.x<620 && mouse.y>450 && mouse.y<550)		return 1;
		else																return 0;
		}
	}

	if (game_phase == Dead)
	{
		switch (buttom)
		{
		case 0:
		if (mouse.x>350 && mouse.x<620 && mouse.y>300 && mouse.y<400)		return 1;
		else																return 0;
		case 1:
		if (mouse.x>350 && mouse.x<620 && mouse.y>430 && mouse.y<530)		return 1;
		else																return 0;
		}
	}

	if (game_phase == Help)
	{
		if (mouse.x>480 && mouse.x<750 && mouse.y>438 && mouse.y<538)		return 1;
		else																return 0;
	}

	if (game_phase == Stop)
	{
		switch(buttom)
		{
		case 0:
		if (mouse.x>350 && mouse.x<620 && mouse.y>250 && mouse.y<350)		return 1;
		else																return 0;
		case 1:
		if (mouse.x>350 && mouse.x<620 && mouse.y>350 && mouse.y<450)		return 1;
		else																return 0;
		case 2:
		if (mouse.x>350 && mouse.x<620 && mouse.y>450 && mouse.y<550)		return 1;
		else																return 0;
		}
	}
	return NULL;
}

//蛇体坐标重复判定
int judge_snake(int dst)
{
	int j;
	for(j=0; j<=sna.life + 1; j++)
	{
		if (dst == sna.snadst[j])  return 1;
	}
	return 0; 
}

//判断食物
int judge_food()
{
	int i;
	for (i=0; i!=3; i++)
	{
		if(sna.snadst[0] == prop[i].dst)
		{
			return prop[i].state + i*10;
		}
	}
	return -1;
}

//////////////资源载入

//载入图像
void load_image()
{
	int i;
	IMAGE gresnake;
	IMAGE blusnake;
	IMAGE gresnaketurn;
	IMAGE blusnaketurn;
	IMAGE foods;
	loadimage(&pic.UI_background, "rescourse\\JPEG\\Background.jpg");
	loadimage(&foods, "rescourse\\JPEG\\Foods.jpg");
	loadimage(&gresnake, "rescourse\\JPEG\\GreSna.jpg");
	loadimage(&gresnaketurn, "rescourse\\JPEG\\GreSnaTurn.jpg");

	loadimage(&pic.UI_Menu, "rescourse\\JPEG\\Menu.jpg");
	loadimage(&pic.UI_Buttom, "rescourse\\JPEG\\Buttom.jpg");
	loadimage(&pic.UI_Dead, "rescourse\\JPEG\\Dead.jpg");
	loadimage(&pic.UI_Help, "rescourse\\JPEG\\Help.jpg");

	loadimage(&pic.UI_HpBar, "rescourse\\BMP\\GreHpBar.bmp");
	loadimage(&pic.UI_HpBarMask, "rescourse\\BMP\\GreHpBarMask.bmp");
	loadimage(&pic.UI_HP[Red], "rescourse\\BMP\\GreHpRed.bmp");
	loadimage(&pic.UI_HP[Yellow], "rescourse\\BMP\\GreHpYellow.bmp");
	loadimage(&pic.UI_HP[Green], "rescourse\\BMP\\GreHpGreen.bmp");

	loadimage(&pic.UI_SpeedBar, "rescourse\\BMP\\GreSpeedBar.bmp");
	loadimage(&pic.UI_SpeedBarMask, "rescourse\\BMP\\GreSpeedBarMask.bmp");
	loadimage(&pic.UI_Speed, "rescourse\\BMP\\Speed.bmp");

	SetWorkingImage(&gresnake);
	for (i=0; i!=4; i++)
	{
		getimage(&sna.snahead[i], Body*i, 0, Body, Body);
		getimage(&sna.snabody[1][i], Body*i, Body, Body, Body);
		getimage(&sna.snabody[0][i],Body*i, Body*2, Body, Body);
		getimage(&sna.snatail[i], Body*i, Body*3, Body, Body);
	}
	SetWorkingImage(&gresnaketurn);
	for (i=0; i!=8; i++)
	{
		getimage(&sna.snaturn[0][i],Body*i, 0, Body, Body);
		getimage(&sna.snaturn[1][i],Body*i, Body, Body, Body);
	}
	SetWorkingImage(&foods);
	for (i=0; i!=6; i++)
	{
		getimage(&pic.pro_pic[i], Body*i, 0, Body, Body);
	}
	SetWorkingImage();
}


//////////////程序运行时的函数

//行走于转弯
void data_shift(int new_snadst, int new_snadstdir)
{
	int i;
	for (i=sna.life+1; i>=1; i--)
	{
		sna.snadst[i] = sna.snadst[i-1];
		sna.snadstdir[i] = sna.snadstdir[i-1];
	}
	sna.snadst[0] = new_snadst;
	sna.snadstdir[0] = new_snadstdir;

	if (sna.snadstdir[0] != sna.snadstdir[1])   //转弯
	{
		switch (sna.snadstdir[1])
		{
		case Up:
			if (sna.snadstdir[0] == Right)		sna.snadstdir[1] = D2R;
			else								sna.snadstdir[1] = D2L;
			break;
		case Down:
			if (sna.snadstdir[0] == Right)		sna.snadstdir[1] = U2R;
			else								sna.snadstdir[1] = U2L;
			break;
		case Left:
			if(sna.snadstdir[0] == Up)			sna.snadstdir[1] = R2U;
			else								sna.snadstdir[1] = R2D;
			break;
		case Right:
			if(sna.snadstdir[0] == Up)			sna.snadstdir[1] = L2U;
			else								sna.snadstdir[1] = L2D;
			break;
		}
	}
}

//道具放置
void prop_replace(int prop_type)
{
	do{
	prop[prop_type].dst = (rand()%(ScrWide - 2*WallWide)/Body*Body + WallWide)*1000 + 
						  (rand()%(ScrHigh - ScrBlank - 2*WallWide)/Body*Body +WallWide +ScrBlank);
	}while(judge_snake(prop[prop_type].dst));
	if (prop_type==0) 
		prop[prop_type].state = 0;
	else
		prop[prop_type].state = rand()%3;
}

//游戏阶段转换
void change_phase()
{
	switch (game_phase)
	{
	case Menu:
		if (judge_mouse(0))
		{
			game_phase = Play42;
			restart();
			FlushMouseMsgBuffer();
			break;
		}
		if (judge_mouse(1))
		{
			game_phase = Help;
			FlushMouseMsgBuffer();
			break;
		}
		if (judge_mouse(2))
		{
			exit(0);
		}
		break;

	case Dead:
		if (judge_mouse(0))
		{
			game_phase = Play42;
			restart();
			FlushMouseMsgBuffer();
			break;
		}
		if (judge_mouse(1))
		{
			game_phase = Menu;
			FlushMouseMsgBuffer();
			break;
		}
		break;

	case Help:
		if (judge_mouse(0))
		{
			game_phase = Menu;
			FlushMouseMsgBuffer();
			break;
		}
		break;

	case Stop:
		if (judge_mouse(0))
		{
			game_phase = Play42;
			FlushMouseMsgBuffer();
			break;
		}
		if (judge_mouse(1))
		{
			game_phase = Play42;
			restart();
			FlushMouseMsgBuffer();
			break;
		}
		if (judge_mouse(2))
		{
			game_phase = Menu;
			FlushMouseMsgBuffer();
			break;
		}
	}
}

//初始化
void restart()
{
	int i;
	sna.life = 1;
	sna.direction = Right;
	sna.speed = 399;
	sna.state = 0;
	score=0;
	for (i=0; i<=sna.life + 1; i++)
	{
		sna.snadst[i] = (Body*sna.life+WallWide-Body*i)*1000 + (ScrBlank+Body);
		sna.snadstdir[i] = Right;
	}
	for (i=0; i!=3; i++)
	{
		prop_replace(i);
	}
}

//对UI画图
void draw_UI()
{
	IMAGE tmp;
	char s[20];
	if (game_phase == Menu)
	{
		putimage(0, 0, &pic.UI_Menu);
		putimage(350, 250, &pic.UI_Buttom);
		putimage(350, 350, &pic.UI_Buttom);
		putimage(350, 450, &pic.UI_Buttom);
		settextstyle(80, 0, _T("RETURN TO CASTLE"));
		setbkmode(TRANSPARENT);
		if (judge_mouse(0))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(380, 250, "START");
		if (judge_mouse(1))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(400, 350, "HELP");
		if (judge_mouse(2))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(400, 450, "QUIT");
	}

	if (game_phase == Play42)
	{
		settextcolor(BLACK);
		settextstyle(50, 0, _T("Gwibble"));
		sprintf(s, "%d", score);
		outtextxy(520,15,"总分:");
		outtextxy(650,15,s);
		if(sna.life <= 5)
		{
			SetWorkingImage(&pic.UI_HP[Red]);
		}else
		if (sna.life <=15)
		{
			SetWorkingImage(&pic.UI_HP[Yellow]);
		}else
		{
			SetWorkingImage(&pic.UI_HP[Green]);
		}
		getimage(&tmp, 0, 0, 223*sna.life/MaxLife, 22);
		SetWorkingImage();

		putimage(65, 10, &tmp);
		putimage(0, 4, &pic.UI_HpBarMask,SRCAND);
		putimage(0, 4, &pic.UI_HpBar,SRCPAINT);

		SetWorkingImage(&pic.UI_Speed);
		getimage(&tmp, 0, 0, 108-108*(sna.speed-MaxSpd)/(400-MaxSpd), 11);
		SetWorkingImage();
		putimage(64, 55, &tmp);
		putimage(0, 45 ,&pic.UI_SpeedBarMask,SRCAND);
		putimage(0, 45 ,&pic.UI_SpeedBar,SRCPAINT);
	}

	if (game_phase == Dead)
	{
		putimage(0, 0, &pic.UI_Dead);
		settextstyle(150, 0, _T("Gwibble"));
		settextcolor(BLUE);
		setbkmode(TRANSPARENT);
		outtextxy(70, 70, " The end!");
		putimage(350, 300, &pic.UI_Buttom);
		putimage(350, 430, &pic.UI_Buttom);
		settextstyle(80, 0, _T("RETURN TO CASTLE"));
		if (judge_mouse(0))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(415, 295, "AGAIN");
		if (judge_mouse(1))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(420, 425, "MENU");
	}

	if (game_phase == Help)
	{
		putimage(480, 438, &pic.UI_Buttom);
		settextstyle(80, 0, _T("RETURN TO CASTLE"));
		setbkmode(TRANSPARENT);
		if (judge_mouse(0))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(540, 435, "BACK");
	}

	if (game_phase == Stop)
	{
		putimage(0, 0, &pic.UI_Dead);
		settextstyle(70, 0, _T("Gwibble"));
		settextcolor(BLUE);
		setbkmode(TRANSPARENT);
		outtextxy(100, 100, "Game Suspension");
		putimage(350, 250, &pic.UI_Buttom);
		putimage(350, 350, &pic.UI_Buttom);
		putimage(350, 450, &pic.UI_Buttom);
		settextstyle(80, 0, _T("RETURN TO CASTLE"));
		setbkmode(TRANSPARENT);
		if (judge_mouse(0))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(380, 250, "Resume");
		if (judge_mouse(1))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(380, 350, "Restart");
		if (judge_mouse(2))					settextcolor(YELLOW);
		else								settextcolor(WHITE);
		outtextxy(400, 450, "Menu");
	}
}

//对道具画图
void draw_prop()
{
	int i;
	for(i=0; i!=3; i++)
	{
		putimage(dstchax(prop[i].dst, X),
				 dstchax(prop[i].dst, Y),
				 &pic.pro_pic[prop[i].state]);
	}
}

//对蛇绘图
void draw_snake()
{
	int i;
	putimage(dstchax(sna.snadst[0], X), 
			 dstchax(sna.snadst[0], Y), 
			 &sna.snahead[sna.snadstdir[0]%4]);
	for (i=1; i<=sna.life; i++)
	{
		if (sna.snadstdir[i]<4)
		{
			putimage(dstchax(sna.snadst[i], X), 
					 dstchax(sna.snadst[i], Y), 
					 &sna.snabody[i%2][sna.snadstdir[i]]);
		}else
		{
			putimage(dstchax(sna.snadst[i], X), 
					 dstchax(sna.snadst[i], Y), 
					 &sna.snaturn[i%2][sna.snadstdir[i]-4]);
		}
	}
	if (sna.snadstdir[i]<4)
	{
		putimage(dstchax(sna.snadst[i], X), 
				 dstchax(sna.snadst[i], Y), 
				 &sna.snatail[sna.snadstdir[i]]);
	}else
	{
		putimage(dstchax(sna.snadst[i], X), 
				 dstchax(sna.snadst[i], Y), 
				 &sna.snatail[sna.snadstdir[i]%4]);
	}
}

//蛇状态操作
void state_cmd()
{
	if (sna.state/100000 == 1) //加生命
	{
		if (sna.life <= MaxLife)
		{
			sna.life ++;
			if (sna.speed>=100) sna.speed -= 30;
		}
		sna.state -= 100000;
		score +=100;
	}
	if (sna.state/10000 == 1) //减一个生命
	{
		sna.life --;
		if (sna.speed<399) sna.speed += 30;
		sna.state -= 10000;
		score -=100;
	}
	if (sna.state/1000 == 1) //减一半生命
	{
		sna.life = sna.life/2;
		if (sna.life<=11) sna.speed = 399-30*(sna.life-1);
		score -=100*sna.life;
		sna.state -= 1000;
	}

}

//蛇行进坐标计算
void go()
{
	int i;
	state_cmd();
	switch(judge_point())
	{
	case Wall:
		sna.life=0;
		break;
	case Snake:
		sna.life=0;
		break;
	case None:
		switch(sna.direction)
		{
		case Up:
			data_shift(sna.snadst[0] - Body, sna.direction);
			break;
		case Right:
			data_shift(sna.snadst[0] + Body*1000, sna.direction);
			break;
		case Left:
			data_shift(sna.snadst[0] - Body*1000, sna.direction);
			break;
		case Down:
			data_shift(sna.snadst[0] + Body, sna.direction);
			break;
		}
		break;
	}
	if (judge_food()!=-1)
	{
		switch(judge_food()%10)
		{
		case LifeUp:
			sna.state += 100000;
			break;
		case SpdUp:
			sna.state += 10000;
			break;
		case Replace:
			sna.state += 1000;
			break;
		}
		prop_replace(judge_food()/10);
	}
	for (i=sna.life+2; i<30; i++)
	{
		sna.snadst[i] = 0;
		sna.snadstdir[i] = 0; 
	}
}

//按键操作处理
void cmd(int keyin)
{
	switch (game_phase)
	{
	case Play42:
		if (judge_direct(keyin))
		{
			sna.direction = keyin;
		}
		if (judge_point() < None)
		{
			sna.direction = sna.snadstdir[0];
		}
		break;
	}
}


//////////////多线程编程///////////////////////////

//绘图
void* process_draw(void* args)
{
	while (1)
	{
		BeginBatchDraw();    //批量绘图
		switch (game_phase)
		{
		case Menu:
			SetWorkingImage();
			draw_UI();
			break;
		case Play42:
			SetWorkingImage();
			putimage(0, 0, &pic.UI_background);
			draw_prop();
			draw_snake();
			draw_UI();
			break;
		case Dead:
			SetWorkingImage();
			draw_UI();
		case Help:
			SetWorkingImage();
			putimage(0, 0, &pic.UI_Help);
			draw_UI();
		case Stop:
			SetWorkingImage();
			draw_UI();
		}
		EndBatchDraw();
		Sleep(1);
	}
	return NULL;
}

//键盘与鼠标输入
void* process_keyin(void* args)
{
	while (1)
	{
		switch (game_phase)
		{
		case Menu:
			while(MouseHit())
			{
				mouse=GetMouseMsg();
				if (mouse.uMsg == WM_LBUTTONUP)
				change_phase();
			}
			break;
		case Play42:
			if (GetAsyncKeyState(VK_UP) & 0x8000)		cmd(Up);
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000)	cmd(Right);
			if (GetAsyncKeyState(VK_LEFT) & 0x8000)		cmd(Left);
			if (GetAsyncKeyState(VK_DOWN) & 0x8000)		cmd(Down);
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)	game_phase = Stop;
			break;
		case Dead:
			while(MouseHit())
			{
				mouse=GetMouseMsg();
				if (mouse.uMsg == WM_LBUTTONUP)
				change_phase();
			}
			break;
		case Help:
			while(MouseHit())
			{
				mouse=GetMouseMsg();
				if (mouse.uMsg == WM_LBUTTONUP)
				change_phase();
			}
			break;
		case Stop:
			while(MouseHit())
			{
				mouse=GetMouseMsg();
				if (mouse.uMsg == WM_LBUTTONUP)
				change_phase();
			}
		}
		Sleep(1);
	}
    return NULL;
}

//蛇行进
void* process_gresna(void* args)
{
	while (1)
	{
		while (game_phase == Play42)
		{
			go();
			Sleep(sna.speed);
			if (sna.life == 0)	game_phase = Dead;
		
		}
		
		Sleep(1);
	}
	return NULL;
}

//////////////主函数

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	pthread_t t1;
    pthread_t t2;
	pthread_t t3;
	initgraph(ScrWide, ScrHigh);
	srand((unsigned)time(NULL));
	HWND hwnd = GetHWnd();
	SetWindowText(hwnd, "贪吃蛇");
	load_image();
	game_phase = Menu;
    pthread_create(&t1, NULL, process_draw,NULL);
	pthread_create(&t2, NULL, process_keyin, NULL);
	pthread_create(&t3, NULL, process_gresna, NULL);
    pthread_join(t1,NULL);
	return 0;
}

