#include "stdafx.h"
#include <Winsock2.h>
#include <Windows.h>
#include "Comm.h"

Comm::Comm()
{
}

Comm::~Comm()
{
}

void Comm::GetCurrentPath(CString &strPathName)
{
  GetModuleFileName(NULL, strPathName.GetBuffer(256), 256);
  strPathName.ReleaseBuffer(256);
  int nPos = strPathName.ReverseFind('\\');
  strPathName = strPathName.Left(nPos + 1);
}

void Comm::InitConsoleWindow()
{
  AllocConsole();
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  int hCrt = _open_osfhandle((long)handle, _O_TEXT);
  FILE * hf = _fdopen(hCrt, "w");
  *stdout = *hf;
}

void Comm::GetLocalIPList(char **iplist, int *cnt, int max)
{
  char hostname[128];
  if (gethostname(hostname, 128) != 0)  {
    AfxMessageBox("ËÑËØ±¾»úIPÊ§°Ü");
    return;
  }
  
  struct hostent *pHost = gethostbyname(hostname);
  if (pHost == NULL) {
    AfxMessageBox("ËÑËØ±¾»úIPÊ§°Ü");
    return;
  }

  int i;

  *cnt = 0;
  for (i = 0; pHost->h_addr_list[i] != NULL && i < max; i++) {
    strcpy(iplist[i], inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i]));
    logDbg("%s", iplist[i]);
  }
  *cnt = i;
}
	