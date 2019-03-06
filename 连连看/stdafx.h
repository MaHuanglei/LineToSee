#pragma once

#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")


#ifndef STDAFX_H
#define STDAFX_H

#define WIDTH   800
#define HEIGTH  625
#define MENUWIDTH  60
#define IMG_WIDTH  40
#define IMG_HEIGTH 50
#define IMGNUM  38


#ifndef IMG
#define IMG


#endif

void LoadSrc();
int InitGame();
int MessagePro();
//bool LocatRect()

#endif