// TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AlarmApp.h"
#include "TestDlg.h"
#include "afxdialogex.h"
#include "Comm.h"
#include "AlarmAppDlg.h"

// CTestDlg 对话框

IMPLEMENT_DYNAMIC(CTestDlg, CDialog)

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
  memset(m_chState, 0, sizeof(m_chState));
}

CTestDlg::~CTestDlg()
{
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_BUTTON2, m_setBtn);
  DDX_Control(pDX, IDC_BUTTON1, m_testBtn);

  DDX_Control(pDX, IDC_CHECK1, m_alarmBtn[0]);
  DDX_Control(pDX, IDC_CHECK2, m_alarmBtn[1]);
  DDX_Control(pDX, IDC_CHECK3, m_alarmBtn[2]);
  DDX_Control(pDX, IDC_CHECK4, m_alarmBtn[3]);
  DDX_Control(pDX, IDC_CHECK5, m_alarmBtn[4]);
  DDX_Control(pDX, IDC_CHECK6, m_alarmBtn[5]);
  DDX_Control(pDX, IDC_CHECK7, m_alarmBtn[6]);
  DDX_Control(pDX, IDC_CHECK8, m_alarmBtn[7]);
  DDX_Control(pDX, IDC_CHECK9, m_alarmBtn[8]);
  DDX_Control(pDX, IDC_CHECK10, m_alarmBtn[9]);
  DDX_Control(pDX, IDC_CHECK11, m_alarmBtn[10]);
  DDX_Control(pDX, IDC_CHECK12, m_alarmBtn[11]);
  DDX_Control(pDX, IDC_CHECK13, m_alarmBtn[12]);
  DDX_Control(pDX, IDC_CHECK14, m_alarmBtn[13]);
  DDX_Control(pDX, IDC_CHECK15, m_alarmBtn[14]);
  DDX_Control(pDX, IDC_CHECK16, m_alarmBtn[15]);
  DDX_Control(pDX, IDC_CHECK17, m_alarmBtn[16]);
  DDX_Control(pDX, IDC_CHECK18, m_alarmBtn[17]);
  DDX_Control(pDX, IDC_CHECK19, m_alarmBtn[18]);
  DDX_Control(pDX, IDC_CHECK20, m_alarmBtn[19]);
	DDX_Control(pDX, IDC_CHECK21, m_alarmBtn[20]);
  DDX_Control(pDX, IDC_CHECK22, m_alarmBtn[21]);
  DDX_Control(pDX, IDC_CHECK23, m_alarmBtn[22]);
  DDX_Control(pDX, IDC_CHECK24, m_alarmBtn[23]);
  DDX_Control(pDX, IDC_CHECK25, m_alarmBtn[24]);
  DDX_Control(pDX, IDC_CHECK26, m_alarmBtn[25]);
  DDX_Control(pDX, IDC_CHECK27, m_alarmBtn[26]);
  DDX_Control(pDX, IDC_CHECK28, m_alarmBtn[27]);
  DDX_Control(pDX, IDC_CHECK29, m_alarmBtn[28]);
  DDX_Control(pDX, IDC_CHECK30, m_alarmBtn[29]);
	DDX_Control(pDX, IDC_CHECK31, m_alarmBtn[30]);
  DDX_Control(pDX, IDC_CHECK32, m_alarmBtn[31]);
  DDX_Control(pDX, IDC_CHECK33, m_alarmBtn[32]);
  DDX_Control(pDX, IDC_CHECK34, m_alarmBtn[33]);
  DDX_Control(pDX, IDC_CHECK35, m_alarmBtn[34]);
  DDX_Control(pDX, IDC_CHECK36, m_alarmBtn[35]);
  DDX_Control(pDX, IDC_CHECK37, m_alarmBtn[36]);
  DDX_Control(pDX, IDC_CHECK38, m_alarmBtn[37]);
  DDX_Control(pDX, IDC_CHECK39, m_alarmBtn[38]);
  DDX_Control(pDX, IDC_CHECK40, m_alarmBtn[39]);
	DDX_Control(pDX, IDC_CHECK41, m_alarmBtn[40]);
  DDX_Control(pDX, IDC_CHECK42, m_alarmBtn[41]);
  DDX_Control(pDX, IDC_CHECK43, m_alarmBtn[42]);
  DDX_Control(pDX, IDC_CHECK44, m_alarmBtn[43]);
  DDX_Control(pDX, IDC_CHECK45, m_alarmBtn[44]);
  DDX_Control(pDX, IDC_CHECK46, m_alarmBtn[45]);
  DDX_Control(pDX, IDC_CHECK47, m_alarmBtn[46]);
  DDX_Control(pDX, IDC_CHECK48, m_alarmBtn[47]);
  DDX_Control(pDX, IDC_CHECK49, m_alarmBtn[48]);
  DDX_Control(pDX, IDC_CHECK50, m_alarmBtn[49]);
	DDX_Control(pDX, IDC_CHECK51, m_alarmBtn[50]);
  DDX_Control(pDX, IDC_CHECK52, m_alarmBtn[51]);
  DDX_Control(pDX, IDC_CHECK53, m_alarmBtn[52]);
  DDX_Control(pDX, IDC_CHECK54, m_alarmBtn[53]);
  DDX_Control(pDX, IDC_CHECK55, m_alarmBtn[54]);
  DDX_Control(pDX, IDC_CHECK56, m_alarmBtn[55]);
  DDX_Control(pDX, IDC_CHECK57, m_alarmBtn[56]);
  DDX_Control(pDX, IDC_CHECK58, m_alarmBtn[57]);
  DDX_Control(pDX, IDC_CHECK59, m_alarmBtn[58]);
  DDX_Control(pDX, IDC_CHECK60, m_alarmBtn[59]);
	DDX_Control(pDX, IDC_CHECK61, m_alarmBtn[60]);
  DDX_Control(pDX, IDC_CHECK62, m_alarmBtn[61]);
  DDX_Control(pDX, IDC_CHECK63, m_alarmBtn[62]);
  DDX_Control(pDX, IDC_CHECK64, m_alarmBtn[63]);  
}

const char * CTestDlg::GetTestChState()
{
  int i;
  for (i = 0; i < ALARM_NUM; i++) {
    if (!m_alarmBtn[i].GetCheck()) {
      m_alarmBtn[i].SetCheck(TRUE);
      m_chState[i] = 1;
      break;
    }
  }

  if (i >= ALARM_NUM) {
    for (i = 0; i < ALARM_NUM; i++) {
      m_alarmBtn[i].SetCheck(FALSE);
      m_chState[i] = 0;
    }
  }

  return m_chState;
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
  ON_BN_CLICKED(IDC_CHECK1, &CTestDlg::OnBnClickedCheck1)
  ON_BN_CLICKED(IDC_CHECK2, &CTestDlg::OnBnClickedCheck2)
  ON_BN_CLICKED(IDC_CHECK3, &CTestDlg::OnBnClickedCheck3)
  ON_BN_CLICKED(IDC_CHECK4, &CTestDlg::OnBnClickedCheck4)
  ON_BN_CLICKED(IDC_CHECK5, &CTestDlg::OnBnClickedCheck5)
  ON_BN_CLICKED(IDC_CHECK6, &CTestDlg::OnBnClickedCheck6)
  ON_BN_CLICKED(IDC_CHECK7, &CTestDlg::OnBnClickedCheck7)
  ON_BN_CLICKED(IDC_CHECK8, &CTestDlg::OnBnClickedCheck8)
  ON_BN_CLICKED(IDC_CHECK9, &CTestDlg::OnBnClickedCheck9)
  ON_BN_CLICKED(IDC_CHECK10, &CTestDlg::OnBnClickedCheck10)
  ON_BN_CLICKED(IDC_CHECK11, &CTestDlg::OnBnClickedCheck11)
  ON_BN_CLICKED(IDC_CHECK12, &CTestDlg::OnBnClickedCheck12)
  ON_BN_CLICKED(IDC_CHECK13, &CTestDlg::OnBnClickedCheck13)
  ON_BN_CLICKED(IDC_CHECK14, &CTestDlg::OnBnClickedCheck14)
  ON_BN_CLICKED(IDC_CHECK15, &CTestDlg::OnBnClickedCheck15)
  ON_BN_CLICKED(IDC_CHECK16, &CTestDlg::OnBnClickedCheck16)
  ON_BN_CLICKED(IDC_CHECK17, &CTestDlg::OnBnClickedCheck17)
  ON_BN_CLICKED(IDC_CHECK18, &CTestDlg::OnBnClickedCheck18)
  ON_BN_CLICKED(IDC_CHECK19, &CTestDlg::OnBnClickedCheck19)
  ON_BN_CLICKED(IDC_CHECK20, &CTestDlg::OnBnClickedCheck20)
  ON_BN_CLICKED(IDC_CHECK21, &CTestDlg::OnBnClickedCheck21)
  ON_BN_CLICKED(IDC_CHECK22, &CTestDlg::OnBnClickedCheck22)
  ON_BN_CLICKED(IDC_CHECK23, &CTestDlg::OnBnClickedCheck23)
  ON_BN_CLICKED(IDC_CHECK24, &CTestDlg::OnBnClickedCheck24)
  ON_BN_CLICKED(IDC_CHECK25, &CTestDlg::OnBnClickedCheck25)
  ON_BN_CLICKED(IDC_CHECK26, &CTestDlg::OnBnClickedCheck26)
  ON_BN_CLICKED(IDC_CHECK27, &CTestDlg::OnBnClickedCheck27)
  ON_BN_CLICKED(IDC_CHECK28, &CTestDlg::OnBnClickedCheck28)
  ON_BN_CLICKED(IDC_CHECK29, &CTestDlg::OnBnClickedCheck29)
  ON_BN_CLICKED(IDC_CHECK30, &CTestDlg::OnBnClickedCheck30)
  ON_BN_CLICKED(IDC_CHECK31, &CTestDlg::OnBnClickedCheck31)
  ON_BN_CLICKED(IDC_CHECK32, &CTestDlg::OnBnClickedCheck32)
  ON_BN_CLICKED(IDC_CHECK33, &CTestDlg::OnBnClickedCheck33)
  ON_BN_CLICKED(IDC_CHECK34, &CTestDlg::OnBnClickedCheck34)
  ON_BN_CLICKED(IDC_CHECK35, &CTestDlg::OnBnClickedCheck35)
  ON_BN_CLICKED(IDC_CHECK36, &CTestDlg::OnBnClickedCheck36)
  ON_BN_CLICKED(IDC_CHECK37, &CTestDlg::OnBnClickedCheck37)
  ON_BN_CLICKED(IDC_CHECK38, &CTestDlg::OnBnClickedCheck38)
  ON_BN_CLICKED(IDC_CHECK39, &CTestDlg::OnBnClickedCheck39)
  ON_BN_CLICKED(IDC_CHECK40, &CTestDlg::OnBnClickedCheck40)
  ON_BN_CLICKED(IDC_CHECK41, &CTestDlg::OnBnClickedCheck41)
  ON_BN_CLICKED(IDC_CHECK42, &CTestDlg::OnBnClickedCheck42)
  ON_BN_CLICKED(IDC_CHECK43, &CTestDlg::OnBnClickedCheck43)
  ON_BN_CLICKED(IDC_CHECK44, &CTestDlg::OnBnClickedCheck44)
  ON_BN_CLICKED(IDC_CHECK45, &CTestDlg::OnBnClickedCheck45)
  ON_BN_CLICKED(IDC_CHECK46, &CTestDlg::OnBnClickedCheck46)
  ON_BN_CLICKED(IDC_CHECK47, &CTestDlg::OnBnClickedCheck47)
  ON_BN_CLICKED(IDC_CHECK48, &CTestDlg::OnBnClickedCheck48)
  ON_BN_CLICKED(IDC_CHECK49, &CTestDlg::OnBnClickedCheck49)
  ON_BN_CLICKED(IDC_CHECK50, &CTestDlg::OnBnClickedCheck50)
  ON_BN_CLICKED(IDC_CHECK51, &CTestDlg::OnBnClickedCheck51)
  ON_BN_CLICKED(IDC_CHECK52, &CTestDlg::OnBnClickedCheck52)
  ON_BN_CLICKED(IDC_CHECK53, &CTestDlg::OnBnClickedCheck53)
  ON_BN_CLICKED(IDC_CHECK54, &CTestDlg::OnBnClickedCheck54)
  ON_BN_CLICKED(IDC_CHECK55, &CTestDlg::OnBnClickedCheck55)
  ON_BN_CLICKED(IDC_CHECK56, &CTestDlg::OnBnClickedCheck56)
  ON_BN_CLICKED(IDC_CHECK57, &CTestDlg::OnBnClickedCheck57)
  ON_BN_CLICKED(IDC_CHECK58, &CTestDlg::OnBnClickedCheck58)
  ON_BN_CLICKED(IDC_CHECK59, &CTestDlg::OnBnClickedCheck59)
  ON_BN_CLICKED(IDC_CHECK60, &CTestDlg::OnBnClickedCheck60)
  ON_BN_CLICKED(IDC_CHECK61, &CTestDlg::OnBnClickedCheck61)
  ON_BN_CLICKED(IDC_CHECK62, &CTestDlg::OnBnClickedCheck62)
  ON_BN_CLICKED(IDC_CHECK63, &CTestDlg::OnBnClickedCheck63)
  ON_BN_CLICKED(IDC_CHECK64, &CTestDlg::OnBnClickedCheck64)
  ON_BN_CLICKED(IDC_BUTTON2, &CTestDlg::OnBnClickedButton2)
  ON_BN_CLICKED(IDC_BUTTON1, &CTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// CTestDlg 消息处理程序


void CTestDlg::OnBnClickedCheck1()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK1))->GetCheck();
  m_chState[0] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[0]);
}


void CTestDlg::OnBnClickedCheck2()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK2))->GetCheck();
  m_chState[1] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[1]);
}


void CTestDlg::OnBnClickedCheck3()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK3))->GetCheck();
  m_chState[2] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[2]);
}


void CTestDlg::OnBnClickedCheck4()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK4))->GetCheck();
  m_chState[3] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[3]);
}


void CTestDlg::OnBnClickedCheck5()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK5))->GetCheck();
  m_chState[4] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[4]);
}


void CTestDlg::OnBnClickedCheck6()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK6))->GetCheck();
  m_chState[5] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[5]);
}


void CTestDlg::OnBnClickedCheck7()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK7))->GetCheck();
  m_chState[6] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[6]);
}


void CTestDlg::OnBnClickedCheck8()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK8))->GetCheck();
  m_chState[7] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[7]);
}


void CTestDlg::OnBnClickedCheck9()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK9))->GetCheck();
  m_chState[8] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[8]);
}


void CTestDlg::OnBnClickedCheck10()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK10))->GetCheck();
  m_chState[9] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[9]);
}

//////////////
void CTestDlg::OnBnClickedCheck11()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK11))->GetCheck();
  m_chState[10] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[10]);
}


void CTestDlg::OnBnClickedCheck12()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK12))->GetCheck();
  m_chState[11] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[11]);
}


void CTestDlg::OnBnClickedCheck13()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK13))->GetCheck();
  m_chState[12] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[12]);
}


void CTestDlg::OnBnClickedCheck14()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK14))->GetCheck();
  m_chState[13] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[13]);
}


void CTestDlg::OnBnClickedCheck15()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK15))->GetCheck();
  m_chState[14] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[14]);
}


void CTestDlg::OnBnClickedCheck16()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK16))->GetCheck();
  m_chState[15] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[15]);
}


void CTestDlg::OnBnClickedCheck17()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK17))->GetCheck();
  m_chState[16] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[16]);
}


void CTestDlg::OnBnClickedCheck18()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK18))->GetCheck();
  m_chState[17] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[17]);
}


void CTestDlg::OnBnClickedCheck19()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK19))->GetCheck();
  m_chState[18] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[18]);
}


void CTestDlg::OnBnClickedCheck20()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK20))->GetCheck();
  m_chState[19] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[19]);
}

//////////////
void CTestDlg::OnBnClickedCheck21()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK21))->GetCheck();
  m_chState[20] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[20]);
}


void CTestDlg::OnBnClickedCheck22()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK22))->GetCheck();
  m_chState[21] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[21]);
}


void CTestDlg::OnBnClickedCheck23()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK23))->GetCheck();
  m_chState[22] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[22]);
}


void CTestDlg::OnBnClickedCheck24()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK24))->GetCheck();
  m_chState[23] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[23]);
}


void CTestDlg::OnBnClickedCheck25()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK25))->GetCheck();
  m_chState[24] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[24]);
}


void CTestDlg::OnBnClickedCheck26()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK26))->GetCheck();
  m_chState[25] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[25]);
}


void CTestDlg::OnBnClickedCheck27()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK27))->GetCheck();
  m_chState[26] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[26]);
}


void CTestDlg::OnBnClickedCheck28()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK28))->GetCheck();
  m_chState[27] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[27]);
}


void CTestDlg::OnBnClickedCheck29()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK29))->GetCheck();
  m_chState[28] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[28]);
}


void CTestDlg::OnBnClickedCheck30()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK30))->GetCheck();
  m_chState[29] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[29]);
}

//////////////
void CTestDlg::OnBnClickedCheck31()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK31))->GetCheck();
  m_chState[30] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[30]);
}


void CTestDlg::OnBnClickedCheck32()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK32))->GetCheck();
  m_chState[31] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[31]);
}


void CTestDlg::OnBnClickedCheck33()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK33))->GetCheck();
  m_chState[32] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[32]);
}


void CTestDlg::OnBnClickedCheck34()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK34))->GetCheck();
  m_chState[33] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[33]);
}


void CTestDlg::OnBnClickedCheck35()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK35))->GetCheck();
  m_chState[34] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[34]);
}


void CTestDlg::OnBnClickedCheck36()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK36))->GetCheck();
  m_chState[35] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[35]);
}


void CTestDlg::OnBnClickedCheck37()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK37))->GetCheck();
  m_chState[36] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[36]);
}


void CTestDlg::OnBnClickedCheck38()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK38))->GetCheck();
  m_chState[37] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[37]);
}


void CTestDlg::OnBnClickedCheck39()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK39))->GetCheck();
  m_chState[38] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[38]);
}


void CTestDlg::OnBnClickedCheck40()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK40))->GetCheck();
  m_chState[39] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[39]);
}

//////////////
void CTestDlg::OnBnClickedCheck41()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK41))->GetCheck();
  m_chState[40] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[40]);
}


void CTestDlg::OnBnClickedCheck42()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK42))->GetCheck();
  m_chState[41] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[41]);
}


void CTestDlg::OnBnClickedCheck43()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK43))->GetCheck();
  m_chState[42] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[42]);
}


void CTestDlg::OnBnClickedCheck44()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK44))->GetCheck();
  m_chState[43] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[43]);
}


void CTestDlg::OnBnClickedCheck45()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK45))->GetCheck();
  m_chState[44] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[44]);
}


void CTestDlg::OnBnClickedCheck46()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK46))->GetCheck();
  m_chState[45] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[45]);
}


void CTestDlg::OnBnClickedCheck47()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK47))->GetCheck();
  m_chState[46] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[46]);
}


void CTestDlg::OnBnClickedCheck48()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK48))->GetCheck();
  m_chState[47] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[47]);
}


void CTestDlg::OnBnClickedCheck49()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK49))->GetCheck();
  m_chState[48] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[48]);
}


void CTestDlg::OnBnClickedCheck50()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK50))->GetCheck();
  m_chState[49] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[49]);
}

//////////////
void CTestDlg::OnBnClickedCheck51()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK51))->GetCheck();
  m_chState[50] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[50]);
}


void CTestDlg::OnBnClickedCheck52()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK52))->GetCheck();
  m_chState[51] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[51]);
}


void CTestDlg::OnBnClickedCheck53()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK53))->GetCheck();
  m_chState[52] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[52]);
}


void CTestDlg::OnBnClickedCheck54()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK54))->GetCheck();
  m_chState[53] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[53]);
}


void CTestDlg::OnBnClickedCheck55()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK55))->GetCheck();
  m_chState[54] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[54]);
}


void CTestDlg::OnBnClickedCheck56()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK56))->GetCheck();
  m_chState[55] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[55]);
}


void CTestDlg::OnBnClickedCheck57()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK57))->GetCheck();
  m_chState[56] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[56]);
}


void CTestDlg::OnBnClickedCheck58()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK58))->GetCheck();
  m_chState[57] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[57]);
}


void CTestDlg::OnBnClickedCheck59()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK59))->GetCheck();
  m_chState[58] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[58]);
}


void CTestDlg::OnBnClickedCheck60()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK60))->GetCheck();
  m_chState[59] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[59]);
}

/////
void CTestDlg::OnBnClickedCheck61()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK61))->GetCheck();
  m_chState[60] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[60]);
}

void CTestDlg::OnBnClickedCheck62()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK62))->GetCheck();
  m_chState[61] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[61]);
}

void CTestDlg::OnBnClickedCheck63()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK63))->GetCheck();
  m_chState[62] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[62]);
}

void CTestDlg::OnBnClickedCheck64()
{
  // TODO:  在此添加控件通知处理程序代码
  int isCheck = ((CButton *)GetDlgItem(IDC_CHECK64))->GetCheck();
  m_chState[63] = isCheck ? 1 : 0;
  logDbg("%d", m_chState[63]);
}


void CTestDlg::OnBnClickedButton2()
{
  // TODO:  在此添加控件通知处理程序代码
  //设置发送
  CAlarmAppDlg *dlg = (CAlarmAppDlg *)GetParent()->GetParent();
  dlg->SndAlarm(m_chState, ALARM_NUM);
}


void CTestDlg::OnBnClickedButton1()
{
  // TODO:  在此添加控件通知处理程序代码
  //自动测试开关
  CAlarmAppDlg *dlg = (CAlarmAppDlg *)GetParent()->GetParent();

  CString txt;
  m_testBtn.GetWindowText(txt);
  if (txt.Compare(_T("开始测试")) == 0) {
    dlg->AutoTest(TRUE);
    m_testBtn.SetWindowText(_T("停止测试"));
  }
  else {
    dlg->AutoTest(FALSE);
    m_testBtn.SetWindowText(_T("开始测试"));
  }
}
