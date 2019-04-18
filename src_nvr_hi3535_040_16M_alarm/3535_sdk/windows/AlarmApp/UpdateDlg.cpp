// UpdateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AlarmApp.h"
#include "UpdateDlg.h"
#include "afxdialogex.h"
#include "AlarmAppDlg.h"
#include "Comm.h"

// CUpdateDlg �Ի���

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
    m_updateStatic.SetWindowText("�������");
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

// CUpdateDlg ��Ϣ��������

//���������ť
void CUpdateDlg::OnBnClickedButton2()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
  
  CAlarmAppDlg *dlg = (CAlarmAppDlg *)GetParent()->GetParent();
  CString strFileName;
  m_filenameEdt.GetWindowText(strFileName);
  char *filename = strFileName.GetBuffer(strFileName.GetLength() + 1);
  strFileName.ReleaseBuffer();

  dlg->UpdateFile(filename);
}

//���ѡ���ļ���ť
void CUpdateDlg::OnBnClickedButton1()
{
  // TODO:  �ڴ����ӿؼ�֪ͨ�����������
  //ѡ���ļ��Ի���
  BOOL isOpen = TRUE; //�Ƿ��(����Ϊ����)  
  CString filter = "�ļ�(*.ram)|*.ram|�����ļ�(*.*)|*.*";
  CFileDialog fileDlg(isOpen, _T("ram"), NULL, 0, filter, this);

  if (fileDlg.DoModal() == IDOK) {
    m_filename = fileDlg.GetPathName();
    m_filenameEdt.SetWindowText(m_filename);
  }
}