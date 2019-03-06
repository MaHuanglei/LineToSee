#include "LineSee.h"
#include "stdafx.h"

extern IMAGE bkImg;
extern IMAGE blackImg;
extern IMAGE Img[IMGNUM];
extern IMAGE Img_c[IMGNUM];
extern DWORD *src_img;
extern DWORD *des_img;

extern HWND g_hWnd;

LineSee::~LineSee()
{
	delete[] map;
	cleardevice();
}

void LineSee::InitGame(int rank)
{
	switch (rank)
	{
	case 1:
		row = 7; col = 12;
		img_num = 20;
		break;
	case 2:
		row = 8; col = 14;
		img_num = 25;
		break;
	case 3:
		row = 9; col = 16;
		img_num = 30;
		break;
	case 4:
		row = 10; col = 18;
		img_num = 35;
		break;
	}
	size = row*col;
	if (map)delete[]map;

	map = new int[size];
	if (!map)
	{
		MessageBox(g_hWnd, "空间分配失败", "提示", MB_OK);
		exit(0);
	}
	memset(map, 0, size*sizeof(int));

	rect.left = (WIDTH - IMG_WIDTH*col) / 2;
	rect.top = (HEIGTH - MENUWIDTH - IMG_HEIGTH*row) / 2 + MENUWIDTH;
	rect.right = rect.left + col*IMG_WIDTH;
	rect.bottom = rect.top + row*IMG_HEIGTH;
	isWin = false; hint = false;
	lastImg.x = lastImg.y = -1;

	_InitMap();
}

void LineSee::PutImage()
{
	cleardevice();
	
	for (int i = 0,*p = map; i < size;p++, i++)
	{
		if (*p == -1)continue;
		putimage(i%col*IMG_WIDTH + rect.left, i / col*IMG_HEIGTH + rect.top, &Img[*p]);
	}

	setlinecolor(BLUE);
	rectangle(720, 15, 780, 45);
	settextstyle(20, 0, "楷体");
	settextcolor(RED);
	outtextxy(730, 20, "放弃");

	rectangle(640, 15, 700, 45);
	outtextxy(650, 20, "提示");
}

void LineSee::PlayGame()
{
	bool gameover = false;
	MOUSEMSG msg;
	while (!isWin && !gameover)
	{
		msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN)
		{
			gameover = _NoWayOut(false);
			if (gameover)
			{
				MessageBox(g_hWnd, "您已没有可消除的图片", "提示", MB_OK);
				break;
				//gameover = false;
			}

			if (msg.x<rect.left || msg.x>rect.right || msg.y<rect.top || msg.y>rect.bottom)
				gameover = _JudgeMenu(msg.x, msg.y);
			else
			{
				if (hint)
				{
					putimage((hintPoint1 % col)*IMG_WIDTH + rect.left, (hintPoint1 / col)*IMG_HEIGTH + rect.top, &Img[*(map + hintPoint1)]);
					putimage((hintPoint2 % col)*IMG_WIDTH + rect.left, (hintPoint2 / col)*IMG_HEIGTH + rect.top, &Img[*(map + hintPoint2)]);
					hint = false;
				}
				_JudgeImg(msg.x, msg.y);
			}
		}
	}
}

bool LineSee::_NoWayOut(bool hint)
{
	POINT temp = lastImg;
	//bool flag = true;
	std::vector<POINT> vtr;
	for (int n = 0; n < size; n++)
	{
		if (*(map + n) == -1)continue;
		lastImg.x = n % col;
		lastImg.y = n / col;
		for (int m = n + 1; m < size; m++)
		{
			if (*(map + m) != *(map + n))continue;
			if (_JudgeDeep(m/col, m%col, vtr))
			{
				if (hint)
				{
					putimage((n % col)*IMG_WIDTH + rect.left, (n/col)*IMG_HEIGTH + rect.top, &Img_c[*(map+n)]);
					putimage((m % col)*IMG_WIDTH + rect.left, (m/col)*IMG_HEIGTH + rect.top, &Img_c[*(map+m)]);
					hintPoint1 = n; hintPoint2 = m;
				}
				lastImg = temp;
				return false;
			}
		}
	}

	lastImg = temp;
	return true;
}

bool LineSee::_JudgeMenu(int x, int y)
{
	if (x>720 && x<780 && y>15 && y < 45)
	{
		return true;
	}
	else if (x>640 && x<700 && y>15 && y < 45)
	{
		_NoWayOut(true);
		hint = true;
		return false;
	}
	else return false;
}

void LineSee::_JudgeImg(int x, int y)
{
	//点中已消除区域
	x = (x - rect.left) / IMG_WIDTH;
	y = (y - rect.top) / IMG_HEIGTH;
	int temp1 = _GetTwoDimensionMap(y, x);
	if (temp1 == -1)return;

	//之前没有点中的图片
	if (lastImg.x == -1)
	{
		putimage(x*IMG_WIDTH + rect.left, y*IMG_HEIGTH + rect.top, &Img_c[temp1]);
		lastImg.x = x;
		lastImg.y = y;
		return;
	} //前后两次点中同一张图片
	else if (x == lastImg.x && y == lastImg.y)
	{
		putimage(x*IMG_WIDTH + rect.left, y*IMG_HEIGTH + rect.top, &Img[temp1]);
		lastImg.x = -1;
		lastImg.y = -1;
		return;
	}

	//点中图片与之前的图片不一样
	int temp2 = _GetTwoDimensionMap(lastImg.y, lastImg.x);
	if (temp1 != temp2)
	{
		putimage(lastImg.x*IMG_WIDTH + rect.left, lastImg.y*IMG_HEIGTH + rect.top, &Img[temp2]);
		lastImg.x = -1;
		lastImg.y = -1;
		return;
	}

	std::vector<POINT> track;
	//判断前后两张图片是否满足消除条件
	if (_JudgeDeep(y, x, track))
	{
		putimage(x*IMG_WIDTH + rect.left, y*IMG_HEIGTH + rect.top, &Img_c[temp1]);
		_EraseImg(track);
		*(map + lastImg.x + lastImg.y*col) = -1;
		*(map + y*col + x) = -1;
		lastImg.x = -1;
		lastImg.y = -1;
	}
	else
	{
		putimage(lastImg.x*IMG_WIDTH + rect.left, lastImg.y*IMG_HEIGTH + rect.top, &Img[temp2]);
		lastImg.x = -1;
		lastImg.y = -1;
		return;
	}
	_JudgeIsWin();
}

void LineSee::_JudgeIsWin()
{
	int *p = map;
	for (int i = 0; i < size; ++p, ++i)
	{
		if (*p != -1)return;
	}
	isWin = true;
	_Celebrate();
}

void LineSee::_InitMap()
{
	int _size = size / 2;
	int *temp = new int[_size];
	srand((unsigned int)time(NULL));
	
	for (int i = 0; i < _size; i++)
	{
		map[i] = temp[i] = rand() % img_num;
	}

	int *mm = new int[_size];
	for (int i = 0; i < _size; i++)
		mm[i] = i;

	int n = _size, p1, p2;
	for (int i = 0; i < _size; i++)
	{
		p1 = i + _size;
		p2 = rand() % n;
		map[p1] = temp[mm[p2]];
		for (; p2 < n - 1; p2++)
			mm[p2] = mm[p2 + 1];
		n -= 1;
	}

	delete[] temp;
	delete[] mm;
}

void LineSee::_Celebrate()
{
	MessageBox(g_hWnd, "恭喜您过关了", "提示", MB_OK);


}

bool LineSee::_JudgeDeep(int y, int x, std::vector<POINT> &track)
{
	int flag;
	POINT now;
	now.x = x; now.y = y;
	track.push_back(now);
	track.push_back(lastImg);
	//无拐点连接
	if (x == lastImg.x)
	{
		if (_Link(now, lastImg))return true;
	}
	else if (y == lastImg.y)
	{
		if (_Link(now, lastImg))return true;
	}


	//一个拐点连接, 两图片不在一条直线上
	if (x != lastImg.x && y != lastImg.y)
	{
		int y_begin = y > lastImg.y ? lastImg.y : y;
		int y_end = lastImg.y + y - y_begin;
		int x_begin = x > lastImg.x ? lastImg.x : x;
		int x_end = x + lastImg.x - x_begin;
		POINT crossPoint1, crossPoint2;
		if ((x<lastImg.x&&y<lastImg.y) || (x>lastImg.x&&y>lastImg.y))
		{
			crossPoint1.x = x_end;
			crossPoint1.y = y_begin;
			crossPoint2.x = x_begin;
			crossPoint2.y = y_end;
		}
		else
		{
			crossPoint1.x = x_begin;
			crossPoint1.y = y_begin;
			crossPoint2.x = x_end;
			crossPoint2.y = y_end;
		}
		if (_GetTwoDimensionMap(crossPoint1.y, crossPoint1.x) == -1)
		{
			if (_Link(now, crossPoint1) && _Link(lastImg, crossPoint1))
			{
				track.insert(track.begin() + 1, crossPoint1);
				return true;
			}
				
		}

		if (_GetTwoDimensionMap(crossPoint2.y, crossPoint2.x) == -1)
		{
			if (_Link(now, crossPoint2) && _Link(lastImg, crossPoint2))
			{
				track.insert(track.begin() + 1, crossPoint2);
				return true;
			}
		}
	}


	//两个拐点连接
	std::vector<POINT> now_x; //纵向
	std::vector<POINT> now_y; //横向
	std::vector<POINT> last_x; //
	std::vector<POINT> last_y; //
	_GetClosePoint(now, false, now_y);
	_GetClosePoint(lastImg, false, last_y);
	if (!now_y.empty() && !last_y.empty())
	{
		POINT temp;
		for (int i = 0; i < now_y.size(); i++)
		{
			temp.x = now_y[i].x;
			temp.y = lastImg.y;
			if (_FindPoint(temp, last_y) && _Link(now_y[i], temp))
			{
				track.insert(track.begin() + 1, now_y[i]);
				track.insert(track.begin() + 2, temp);
				return true;
			}
		}
	}

	_GetClosePoint(now, true, now_x);
	_GetClosePoint(lastImg, true, last_x);
	if (!now_x.empty() && !last_x.empty())
	{
		POINT temp;
		for (int i = 0; i < now_x.size(); i++)
		{
			temp.x = lastImg.x;
			temp.y = now_x[i].y;
			if (_FindPoint(temp, last_x) && _Link(now_x[i], temp))
			{
				track.insert(track.begin() + 1, now_x[i]);
				track.insert(track.begin() + 2, temp);
				return true;
			}
		}
	}

	return false;
}

bool LineSee::_FindPoint(POINT obj, std::vector<POINT> vtr)
{
	std::vector<POINT>::iterator it = vtr.begin();
	for (; it != vtr.end(); it++)
	{
		if (it->x == obj.x && it->y == obj.y)
		{
			return true;
			break;
		}
	}
	return false;
}

bool LineSee::_Link(POINT p1, POINT p2)
{
	if (p1.x == p2.x)
	{
		int max_y = p1.y > p2.y ? p1.y : p2.y;
		int min_y = p1.y + p2.y - max_y;
		for (int i = min_y + 1; i < max_y; i++)
		{
			if (p1.x == -1 || p1.x == col)continue;
			if (_GetTwoDimensionMap(i, p1.x) != -1)
			{
				return false;
			}
		}
	}
	else if (p1.y == p2.y)
	{
		int max_x = p1.x > p2.x ? p1.x : p2.x;
		int min_x = p1.x + p2.x - max_x;
		for (int i = min_x + 1; i < max_x; i++)
		{
			if (p1.y == -1 || p1.y == row)continue;
			if (_GetTwoDimensionMap(p1.y, i) != -1)
			{
				return false;
			}
		}
	}
	return true;
}

void LineSee::_EraseImg(std::vector<POINT> track)
{
	//划线
	int rgb[5] = { 0x00AA00, 0x0000AA, 0xAA0000, 0xAAAA00 };
	for (int j = 0; j < 4; j++)
	{
		setlinecolor(rgb[j]);
		setlinestyle(0,5);
		for (int i = 1; i < track.size(); i++)
		{
			line((track[i - 1].x*IMG_WIDTH) + rect.left + IMG_WIDTH / 2, 
				(track[i - 1].y*IMG_HEIGTH)+rect.top+IMG_HEIGTH/2, 
				(track[i].x*IMG_WIDTH) + rect.left + IMG_WIDTH / 2, 
				(track[i].y*IMG_HEIGTH) + rect.top + IMG_HEIGTH / 2);
		}
		Sleep(150);
	}
	
	setlinecolor(BLACK);
	setlinestyle(0,5);
	for (int i = 1; i < track.size(); i++)
	{
		line((track[i - 1].x*IMG_WIDTH) + rect.left + IMG_WIDTH / 2,
			(track[i - 1].y*IMG_HEIGTH) + rect.top + IMG_HEIGTH / 2,
			(track[i].x*IMG_WIDTH) + rect.left + IMG_WIDTH / 2,
			(track[i].y*IMG_HEIGTH) + rect.top + IMG_HEIGTH / 2);
	}

	putimage(track[0].x*IMG_WIDTH + rect.left, track[0].y*IMG_HEIGTH + rect.top, &blackImg);
	putimage((track.end()-1)->x*IMG_WIDTH + rect.left, (track.end()-1)->y*IMG_HEIGTH + rect.top, &blackImg);
}

int LineSee::_GetTwoDimensionMap(int y, int x)
{
	return *(map + y*col + x);
}

void LineSee::_GetClosePoint(POINT at, bool portait, std::vector<POINT> &close)
{
	POINT temp;
	if (portait)
	{
		temp.x = at.x;
		for (int i = at.y + 1; i < row + 1; i++)
		{
			if (i == row || (_GetTwoDimensionMap(i, at.x) == -1))
			{ 
				temp.y = i;
				close.push_back(temp);
			}
			else break;
		}
		for (int i = at.y - 1; i > -2; i--)
		{
			if (i == -1 || (_GetTwoDimensionMap(i, at.x) == -1))
			{
				temp.y = i;
				close.push_back(temp);
			}
			else break;
		}
	}
	else
	{
		temp.y = at.y;
		for (int i = at.x + 1; i < col + 1; i++)
		{
			if (i == col || (_GetTwoDimensionMap(at.y, i) == -1))
			{
				temp.x = i;
				close.push_back(temp);
			}
			else break;
		}
		for (int i = at.x - 1; i > -2; i--)
		{
			if (i == -1 || (_GetTwoDimensionMap(at.y, i) == -1))
			{
				temp.x = i;
				close.push_back(temp);
			}
			else break;
		}
	}
}