#pragma once
#include <Windows.h>
#include <vector>

#ifndef LINESEE_H
#define LINESEE_H

//连连看系统
class LineSee
{
private:
	int *map = NULL;	//地图
	int size;		//图片数量
	int col, row;		//行数、列数
	int img_num;		//图片种类数目
	bool isWin;		//是否过关
	bool hint;		//是否提示
	RECT rect;		//图片区域矩形
	POINT lastImg;	//上一次点中的图片坐标
	int hintPoint1, hintPoint2;	//提示所给出的两张图片坐标
public:
	LineSee(){ };
	void InitGame(int rank); //初始化
	~LineSee();
	void PutImage();	//贴图
	void PlayGame();	//开始游戏
private:
	void _JudgeIsWin();	//判断是否胜利
	void _InitMap();		//初始化地图
	void _Celebrate();	//庆祝胜利
	bool _NoWayOut(bool hint);	//是否没有可以消除的图片
	bool _JudgeMenu(int x, int y);	//判断菜单区域是否有按钮被点中
	void _JudgeImg(int x, int y);	//判断图片区域
	void _EraseImg(std::vector<POINT> track);	//擦除图片
	int _GetTwoDimensionMap(int y, int x);	//得到二维地图矩阵的值
	bool _Link(POINT p1, POINT p2);	//判断p1与p2是否无障碍连通
	bool _FindPoint(POINT obj, std::vector<POINT> vtr);	//判断vtr中是否含有obj
	void _GetClosePoint(POINT at, bool x, std::vector<POINT> &close);//得到周围空点
	bool _JudgeDeep(int y, int x, std::vector<POINT> &track);//判断两张图片是否能在三条线内连接
};

#endif