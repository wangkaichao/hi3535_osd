// UpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AlarmApp.h"
#include "UpdateDlg.h"
#include "afxdialogex.h"
#include "AlarmAppDlg.h"
#include "Comm.h"

// CUpdateDlg 对话框

IMPLEMENT_DYNAMIC(CUpdateDlg, CDialog)

CUpdateDlg::CUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateDlg::IDD, pParent)
{

}

CUpdateDlg::~CUpdateDlg()
{
}

void CUpdateDlg::SetUpdatePos(int pos)
{
  if (pos < 0 || pos > MAX_PROGRESS_POS)
    return;

  m_updateGress.SetPos(pos);

  if (pos == MAX_PROGRESS_POS) {
    m_updateStatic.SetWindowText("升级完成");
  }
  else if (pos == 0) {
    m_updateStatic.SetWindowText("");
  }
  else {
    CString strRate;
    strRate.Format("%d%%", pos);
    m_updateStatic.SetWindowText(strRate);
  }
}

void CUpdateDlg::SetUpdateStep(int setup)
{
  m_updateGress.SetStep(setup);
}

void CUpdateDlg::GetFileName(CString &filename)
{
  filename = m_filename;
}

void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_EDIT1, m_filenameEdt);
  DDX_Control(pDX, IDC_PROGRESS1, m_updateGress);
  DDX_Control(pDX, IDC_STATIC_RATE, m_updateStatic);
  DDX_Control(pDX, IDC_BUTTON2, m_updateBtn);
}

BOOL CUpdateDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_updateStatic.SetWindowText("");
  m_updateGress.SetRange(0, MAX_PROGRESS_POS);
  m_updateGress.SetStep(16);
  m_updateGress.SetPos(0);

  return true;
}

BEGIN_MESSAGE_MAP(CUpdateDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CUpdateDlg::OnBnClickedButton2)
  ON_BN_CLICKED(IDC_BUTTON1, &CUpdateDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// CUpdateDlg 消息处理程序

//点击升级按钮
void CUpdateDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
  
  CAlarmAppDlg *dlg = (CAlarmAppDlg *)GetParent()->GetParent();
  CString strFileName;
  m_filenameEdt.GetWindowText(strFileName);
  char *filename = strFileName.GetBuffer(strFileName.GetLength() + 1);
  strFileName.ReleaseBuffer();

  dlg->UpdateFile(filename);
}

//点击选择文件按钮
void CUpdateDlg::OnBnClickedButton1()
{
  // TODO:  在此添加控件通知处理程序代码
  //选择文件对话框
  BOOL isOpen = TRUE; //是否打开(否则为保存)  
  CString filter = "文件(*.ram)|*.ram|所有文件(*.*)|*.*";
  CFileDialog fileDlg(isOpen, _T("ram"), NULL, 0, filter, this);

  if (fileDlg.DoModal() == IDOK) {
    m_filename = fileDlg.GetPathName();
    m_filenameEdt.SetWindowText(m_filename);
  }
}
