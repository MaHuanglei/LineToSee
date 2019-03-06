/**************************************************************
		项目名称：连连看
		项目描述：实现经典游戏连连看的核心算法
		编译环境：VS 2013
		最后时间：2017年12月28日 马黄雷
**************************************************************/
#include "stdafx.h"
#include "LineSee.h"

IMAGE bkImg;
IMAGE blackImg;
IMAGE Img[IMGNUM];
IMAGE Img_c[IMGNUM];
DWORD *src_img;
DWORD *des_img;

HWND g_hWnd;
RECT lastRect;

int main()
{
	initgraph(WIDTH, HEIGTH);
	LoadSrc();
	g_hWnd = GetHWnd();
	LineSee myGame;
    //int flag = 1;
	int rank = 1;
	while (rank)
	{
		rank = InitGame();
		if (rank)
		{
			myGame.InitGame(rank);
			myGame.PutImage();
			myGame.PlayGame();
		}
	}
	closegraph();
	return 0;
}

void LoadSrc()
{
	loadimage(&bkImg, _T("./resP/bk.jpg"), WIDTH, HEIGTH);
	loadimage(&blackImg, "./resP/black.jpg", IMG_WIDTH, IMG_HEIGTH);
	char imgName[50];
	for (int i = 0; i < IMGNUM; i++)
	{
		sprintf(imgName, "%s%d%s", "./resP/", i + 1, ".jpg");
		//_sprintf(imgName, "./resP/", i + 1, ".jpg");
		loadimage(&Img[i], imgName, IMG_WIDTH, IMG_HEIGTH);
		sprintf(imgName, "%s%d%s", "./resP/", i + 1, "-1.jpg");
		loadimage(&Img_c[i], imgName, IMG_WIDTH, IMG_HEIGTH);
	}
	mciSendString("open ./resM/104.mp3 alias bkmusic", NULL, 0, NULL);
}

int InitGame()
{
	putimage(0, 0, &bkImg);
	
	mciSendString("play bkmusic repeat", NULL, 0, NULL);
	lastRect.left = -1;

	bool flag = true; 
	int i = 0, rank;

	setbkmode(TRANSPARENT); //背景透明
	int rgb[5] = { 0x00AA00, 0x0000AA, 0xAA0000, 0x55FFFF, 0xAAAA00 };
	setlinecolor(RED);
	setlinestyle(1);
	rectangle(710, 575, 770, 605);
	settextstyle(20, 0, "楷体");
	settextcolor(BLUE);
	outtextxy(720, 580, "退出");
	settextstyle(30, 0, "楷体");
	while (flag)
	{
		while (!MouseHit())
		{
			setlinecolor(rgb[i]);
			rectangle(100, 490, 700, 550);
			line(250, 490, 250, 550);
			line(400, 490, 400, 550);
			line(550, 490, 550, 550);

			settextcolor(rgb[i + 1]);
			outtextxy(130, 505, "初  级");
			outtextxy(280, 505, "中  级");
			outtextxy(430, 505, "高  级");
			outtextxy(580, 505, "特  高");
			Sleep(400); i++;
			if (i == 4)i = 0;
		}
		rank = MessagePro();
		if (rank >= 0 && rank <= 4)flag = false;
	}
	return rank;
}

int MessagePro()
{
	int rank = -1;
	MOUSEMSG msg;
	msg = GetMouseMsg();
	if (msg.uMsg == WM_LBUTTONDOWN)
	{
		if (msg.x > 710 && msg.x<770 && msg.y>575 && msg.y < 605)
		{
			rank = 0;
		}
		else if (msg.x > 100 && msg.x<250 && msg.y>490 && msg.y < 550)
		{
			rank = 1;
		}
		else if (msg.x > 250 && msg.x<400 && msg.y>490 && msg.y < 550)
		{
			rank = 2;
		}
		else if (msg.x > 400 && msg.x<550 && msg.y>490 && msg.y < 550)
		{
			rank = 3;
		}
		else if (msg.x > 550 && msg.x<700 && msg.y>490 && msg.y < 550)
		{
			rank = 4;
		}
	}
	//else if (msg.uMsg == WM_MOUSEMOVE)
	//{
	//	HDC hdc = GetDC(g_hWnd);
	//	HBRUSH hr = CreateSolidBrush(BLUE);
	//	RECT rect;

	//	if (lastRect.left != -1)
	//	{
	//		HBRUSH bhr = CreateSolidBrush(BLACK);
	//		FillRect(hdc, &lastRect, hr);
	//		DeleteObject(bhr);
	//		lastRect.left = -1;
	//	}

	//	if (msg.x > 100 && msg.x<250 && msg.y>430 && msg.y < 490)
	//	{
	//		rect.left = 100;
	//		rect.top = 430;
	//		rect.right = rect.left + 150;
	//		rect.bottom = rect.top + 60;
	//		FillRect(hdc, &rect, hr);
	//		lastRect = rect;
	//	}
	//	else if (msg.x > 250 && msg.x<400 && msg.y>430 && msg.y < 490)
	//	{
	//		rect.left = 250;
	//		rect.top = 430;
	//		rect.right = rect.left + 150;
	//		rect.bottom = rect.top + 60;
	//		FillRect(hdc, &rect, hr);
	//		lastRect = rect;
	//	}
	//	else if (msg.x > 400 && msg.x<550 && msg.y>430 && msg.y < 490)
	//	{
	//		rect.left = 400;
	//		rect.top = 430;
	//		rect.right = rect.left + 150;
	//		rect.bottom = rect.top + 60;
	//		FillRect(hdc, &rect, hr);
	//		lastRect = rect;
	//	}
	//	else if (msg.x > 550 && msg.x<700 && msg.y>430 && msg.y < 490)
	//	{
	//		rect.left = 550;
	//		rect.top = 430;
	//		rect.right = rect.left + 150;
	//		rect.bottom = rect.top + 60;
	//		FillRect(hdc, &rect, hr);
	//		lastRect = rect;
	//	}
	//	DeleteObject(hr);
	//	ReleaseDC(g_hWnd, hdc);
	//}
	return rank;
}


