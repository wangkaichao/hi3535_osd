// SetNetworkDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AlarmApp.h"
#include "SetNetworkDlg.h"
#include "afxdialogex.h"
#include "AlarmAppDlg.h"

// SetNetworkDlg 对话框

IMPLEMENT_DYNAMIC(SetNetworkDlg, CDialog)

SetNetworkDlg::SetNetworkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetNetworkDlg::IDD, pParent)
{

}

SetNetworkDlg::~SetNetworkDlg()
{
}

void SetNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_IPADDRESS1, m_ipCtl);
  DDX_Control(pDX, IDC_IPADDRESS2, m_submaskCtl);
  DDX_Control(pDX, IDC_IPADDRESS3, m_gatewayCtl);
}


BEGIN_MESSAGE_MAP(SetNetworkDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTON1, &SetNetworkDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// SetNetworkDlg 消息处理程序


void SetNetworkDlg::OnBnClickedButton1()
{
  // TODO:  在此添加控件通知处理程序代码
  // 修改设备网络
  if (m_ipCtl.IsBlank() || m_submaskCtl.IsBlank() || m_gatewayCtl.IsBlank()) {
    AfxMessageBox("输入不能为空");
    return;
  }

  BYTE b1, b2, b3, b4;
  char ip[16];
  char submask[16];
  char gateway[16];

  m_ipCtl.GetAddress(b1, b2, b3, b4);
  sprintf(ip, "%d.%d.%d.%d", b1, b2, b3, b4);

  m_submaskCtl.GetAddress(b1, b2, b3, b4);
  sprintf(submask, "%d.%d.%d.%d", b1, b2, b3, b4);

  m_gatewayCtl.GetAddress(b1, b2, b3, b4);
  sprintf(gateway, "%d.%d.%d.%d", b1, b2, b3, b4);

  CAlarmAppDlg *dlg = (CAlarmAppDlg *)GetParent()->GetParent();
  dlg->SetNet(ip, submask, gateway);
}
