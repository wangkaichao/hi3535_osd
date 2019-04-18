// Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Winsock2.h>
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>

#include "NetLib.h"
#pragma comment(lib, "NetLib_Win32D")
//#pragma comment(lib, "NetLib_Win32")
//#pragma comment(lib, "NetLib_Win64D")
//#pragma comment(lib, "NetLib_Win64")

#define USE_DEBUG	(1)
#if USE_DEBUG

#define logErr(...) do {\
  printf("[%s:%d]", __FUNCTION__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); \
} while (0)

#define logDbg(...) do {\
  printf("[%s:%d]", __FUNCTION__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); \
} while (0)

#else

#define logErr(...)	
#define logDbg(...) 

#endif

int _tmain(int argc, _TCHAR* argv[])
{
  search_st list[256];
  int len;
  NetLib_Init();
  NetLib_Search(list, &len, 256, 1000);
  
  for (int i = 0; i < len; i++) {
    logDbg("[%s][%s][%s]", list[i].ip_addr, list[i].mac_addr, list[i].version);
  }
  while (true)
  {
    Sleep(2000);
  }
  NetLib_DeInit();
  return 0;
}
