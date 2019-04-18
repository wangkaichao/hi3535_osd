#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#pragma comment(lib, "Ws2_32")

#define USE_DEBUG	(0)
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

class Comm
{
public:
  Comm();
  ~Comm();
  static void GetCurrentPath(CString &strPathName);
  static void InitConsoleWindow();
  static void GetLocalIPList(char **iplist, int *cnt, int max);
};

