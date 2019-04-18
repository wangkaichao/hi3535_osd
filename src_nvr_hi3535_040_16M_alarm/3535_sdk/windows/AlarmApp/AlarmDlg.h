#pragma once
#include "NetLib.h"

// AlarmDlg �Ի���

class AlarmDlg : public CDialog
{
	DECLARE_DYNAMIC(AlarmDlg)

public:
	AlarmDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AlarmDlg();

// �Ի�������
	enum { IDD = IDD_ALARMDLG };
  CComboBox m_fontSizeComb;
  CComboBox m_chNumComb;
  CEdit m_intervalEdt;
  CButton m_setBtn;
  CButton m_getBtn;
  CListCtrl m_listCtl;
  CEdit m_edit;

private:
  alarm_config_t m_alarm;
  int m_col; // �༭ѡ����
  int m_row; // �༭ѡ����

  int m_pageCol;
  int m_pageRow;
  void reshList();
  int point2idx(int row, int col);
  void idx2point(int idx, int *prow, int *pcol);
  void editListItem(int item, int subitem);
  void editNextListItem();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnCbnSelchangeCombo1();
  afx_msg void OnCbnSelchangeCombo2();
  afx_msg void OnBnClickedButton1();
  afx_msg void OnBnClickedButton2();
  afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnEnKillfocusEdit1();
};
