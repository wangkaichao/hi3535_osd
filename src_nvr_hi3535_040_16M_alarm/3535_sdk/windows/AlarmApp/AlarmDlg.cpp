// AlarmDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AlarmApp.h"
#include "AlarmDlg.h"
#include "afxdialogex.h"
#include "Comm.h"
#include "AlarmAppDlg.h"

// AlarmDlg �Ի���

IMPLEMENT_DYNAMIC(AlarmDlg, CDialog)

AlarmDlg::AlarmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AlarmDlg::IDD, pParent)
{
  memset(&m_alarm, 0, sizeof(alarm_config_t));
  m_col = 0;
  m_row = 0;
}

AlarmDlg::~AlarmDlg()
{
}

void AlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_COMBO1, m_fontSizeComb);
  DDX_Control(pDX, IDC_EDIT2, m_intervalEdt);
  DDX_Control(pDX, IDC_COMBO2, m_chNumComb);
  DDX_Control(pDX, IDC_BUTTON1, m_setBtn);
  DDX_Control(pDX, IDC_BUTTON2, m_getBtn);
  DDX_Control(pDX, IDC_LIST2, m_listCtl);
  DDX_Control(pDX, IDC_EDIT1, m_edit);
}

BOOL AlarmDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // ��ʼ������ѡ��
  char *fontSize[] = { _T("��"), _T("��"), _T("С") };
  for (int i = 0; i < sizeof(fontSize) / sizeof(char *); i++)
    m_fontSizeComb.InsertString(i, fontSize[i]);

  // ��ʼ��ͨ����ѡ��[0-256]
  for (int i = 0; i < MAX_ALARM_NUM; i++) {
    CString strIdx;
    strIdx.Format("%d", i+1);
    m_chNumComb.InsertString(i, strIdx);
  }

  //��ʼ���б�
  m_edit.ShowWindow(SW_HIDE);
  m_edit.SetLimitText(32);
  m_listCtl.SetExtendedStyle(m_listCtl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  return true;
}

BEGIN_MESSAGE_MAP(AlarmDlg, CDialog)
  ON_CBN_SELCHANGE(IDC_COMBO1, &AlarmDlg::OnCbnSelchangeCombo1)
  ON_CBN_SELCHANGE(IDC_COMBO2, &AlarmDlg::OnCbnSelchangeCombo2)
  ON_BN_CLICKED(IDC_BUTTON1, &AlarmDlg::OnBnClickedButton1)
  ON_BN_CLICKED(IDC_BUTTON2, &AlarmDlg::OnBnClickedButton2)
  ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &AlarmDlg::OnNMDblclkList2)
  ON_EN_KILLFOCUS(IDC_EDIT1, &AlarmDlg::OnEnKillfocusEdit1)
END_MESSAGE_MAP()


// AlarmDlg ��Ϣ�������
void AlarmDlg::reshList()
{
  //�����
  m_listCtl.DeleteAllItems();
  int oldcol = m_listCtl.GetHeaderCtrl()->GetItemCount();
  for (int i = 0; i < oldcol; i++)
    m_listCtl.DeleteColumn(0);

  if (m_alarm.m_fontsize == 0) {
    //���У�һ��12��
    m_pageCol = 1;
    m_pageRow = 11; //12;
  }
  else if (m_alarm.m_fontsize == 1) {
    //˫�У�һ��16��
    m_pageCol = 2;
    m_pageRow = 15; //16;
  }
  else if (m_alarm.m_fontsize == 2) {
    //���У�һ��24��
    m_pageCol = 3;
    m_pageRow = 23; //24;
  }

  //�����б�ͷ
  for (int i = 0; i < m_pageCol; i++) {
    m_listCtl.InsertColumn(i * 2, _T("ͨ����"), LVCFMT_LEFT, 50);
    m_listCtl.InsertColumn(i * 2 + 1, _T("ͨ������"), LVCFMT_LEFT, 240);
  }

  //�����б���
  int page = m_alarm.m_chnum / (m_pageCol * m_pageRow);
  if (m_alarm.m_chnum % (m_pageCol * m_pageRow))
    page++;

  int cnt = 0;
  for (int i = 0; i < page; i++) {
    for (int j = 0; j < m_pageCol; j++) {
      for (int k = 0; k < m_pageRow; k++) {
        int item = (cnt % (m_pageCol * m_pageRow)) % m_pageRow + (i * m_pageRow);
        int subitem = ((cnt % (m_pageCol * m_pageRow)) / m_pageRow) * 2;
        
        CString strCnt;
        strCnt.Format("%d", cnt + 1);

        if (subitem == 0) {
          m_listCtl.InsertItem(item, strCnt);
        }
        else {
          m_listCtl.SetItemText(item, subitem, strCnt);
        }
        m_listCtl.SetItemText(item, subitem + 1, m_alarm.m_strch[cnt]);

        if (++cnt >= m_alarm.m_chnum)
          return;
      }
    }
  }
}

int AlarmDlg::point2idx(int row, int col)
{
  //          ҳ��ʼλ��ƫ��                                 + ҳ����ƫ��
  return (row / m_pageRow) * (m_pageRow * m_pageCol) + (col / 2) * m_pageRow + (row % m_pageRow);
}

void AlarmDlg::idx2point(int idx, int *prow, int *pcol)
{
  *prow = (idx / (m_pageRow * m_pageCol)) * m_pageRow + ((idx % (m_pageRow * m_pageCol)) % m_pageRow);
  *pcol = ((idx % (m_pageRow * m_pageCol)) / m_pageRow) * 2 + 1;
}

void AlarmDlg::editListItem(int item, int subitem)
{
  int chidx = point2idx(item, subitem);
  
  // ͨ������
  if (chidx < m_alarm.m_chnum && subitem % 2 != 0)
  {
    CRect rc;
    m_row = item;
    m_col = subitem;

    m_listCtl.SetItemState(m_row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    m_listCtl.EnsureVisible(m_row, FALSE);
    m_listCtl.GetSubItemRect(m_row, m_col, LVIR_LABEL, rc);

    m_edit.SetParent(&m_listCtl);
    m_edit.MoveWindow(rc);
    m_edit.SetWindowText(m_listCtl.GetItemText(m_row, m_col));
    m_edit.ShowWindow(SW_SHOW);
    m_edit.SetFocus(); //����Edit����
    m_edit.ShowCaret(); //��ʾ���
    m_edit.SetSel(0, -1); //ȫѡ 
  }
}

void AlarmDlg::editNextListItem()
{
  int chidx = point2idx(m_row, m_col);

  if (chidx + 1 < m_alarm.m_chnum && m_col % 2 != 0) {
    int row, col;
    idx2point(chidx + 1, &row, &col);
    editListItem(row, col);
  }
}

//ѡ�������С
void AlarmDlg::OnCbnSelchangeCombo1()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  m_alarm.m_fontsize = m_fontSizeComb.GetCurSel();
  if (m_chNumComb.GetCurSel() == -1)
    return;

  reshList();
}

//ѡ��ͨ���ܸ���
void AlarmDlg::OnCbnSelchangeCombo2()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  m_alarm.m_chnum = m_chNumComb.GetCurSel() + 1;
  if (m_fontSizeComb.GetCurSel() == -1)
    return;

  reshList();
}

//����ͨ����Ϣ
void AlarmDlg::OnBnClickedButton1()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  CString str;
  m_intervalEdt.GetWindowText(str);
  if (str.IsEmpty()) {
    AfxMessageBox("����ʧ�ܣ�����Ϊ�գ�");
    return;
  }

  if (m_fontSizeComb.GetCurSel() == -1 || m_chNumComb.GetCurSel() == -1) {
    AfxMessageBox("����ʧ�ܣ�����Ϊ�գ�");
    return;
  }

  int interval = atoi(str);
  if (interval  <= 0 || interval > 10) {
    AfxMessageBox("����ʱ��������1~10�뷶Χ��");
    return;
  }

  m_alarm.m_interval = interval;

  CAlarmAppDlg *dlg = (CAlarmAppDlg *)GetParent()->GetParent();
  int row, col;
  for (int i = 0; i < m_alarm.m_chnum; i++) {
    idx2point(i, &row, &col);
    CString txt = m_listCtl.GetItemText(row, col);
    if (txt.IsEmpty()) {
      AfxMessageBox("����ʧ�ܣ�����Ϊ�գ�");
      return;
    }
    strcpy(m_alarm.m_strch[i], txt);
  }

  dlg->SetALarm(&m_alarm);
}

//��ѯͨ����Ϣ
void AlarmDlg::OnBnClickedButton2()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  CAlarmAppDlg *dlg = (CAlarmAppDlg *)GetParent()->GetParent();
  int ret = dlg->GetAlarm(&m_alarm);
  if (ret < 0)
    return;

  m_fontSizeComb.SetCurSel(m_alarm.m_fontsize);
  m_chNumComb.SetCurSel(m_alarm.m_chnum - 1);
  CString str;
  str.Format("%d", m_alarm.m_interval);
  m_intervalEdt.SetWindowText(str);
  reshList();
}

//ѡ����˫��������
void AlarmDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;

  if (pNMListView->iItem >= 0 && pNMListView->iSubItem >= 0)
    editListItem(pNMListView->iItem, pNMListView->iSubItem);

  *pResult = 0;
}

void AlarmDlg::OnEnKillfocusEdit1()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  CString str;
  m_edit.GetWindowText(str);
  m_listCtl.SetItemText(m_row, m_col, str);
  m_edit.ShowWindow(SW_HIDE);
}

BOOL AlarmDlg::PreTranslateMessage(MSG* pMsg)
{
  // TODO: �ڴ����ר�ô����/����û���  
  if (pMsg->message == WM_KEYDOWN)
  {
    if (pMsg->wParam == VK_RETURN) {
      if (GetFocus()->GetDlgCtrlID() == IDC_EDIT1)
      {
        m_listCtl.SetFocus(); //ʹ�б�ؼ���ý��㣬��CEdit���Զ�ʧȥ���㣬����EN_KILLFOCUS��Ϣ
      }

      return TRUE;
    }
    else if (pMsg->wParam == VK_ESCAPE) {
      return TRUE; //�����κβ���
    }
    else if (pMsg->wParam == VK_TAB) {
      if (GetFocus()->GetDlgCtrlID() == IDC_EDIT1)
      {
        m_listCtl.SetFocus();
        editNextListItem();
        return TRUE;
      }
    }
  }

  return CDialog::PreTranslateMessage(pMsg);
}