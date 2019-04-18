
// AlarmAppDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "TestDlg.h"
#include "UpdateDlg.h"
#include "SetNetworkDlg.h"
#include "AlarmDlg.h"
#include "Comm.h"
#include "NetLib.h"

// CAlarmAppDlg 对话框
class CAlarmAppDlg : public CDialogEx
{
// 构造
public:
	CAlarmAppDlg(CWnd* pParent = NULL);	// 标准构造函数
  virtual ~CAlarmAppDlg();

  int SndAlarm(const char *alarm, int num);
  int AutoTest(BOOL enable);
  int SetNet(const char *ip, const char *submask, const char *gateway);
  int UpdateFile(const char *filename);
  int SetALarm(const alarm_config_t *alarm);
  int GetAlarm(alarm_config_t *alarm);

// 对话框数据
	enum { IDD = IDD_ALARMAPP_DIALOG };
  enum { DEVICE_MAX_NUM = 256};
  enum { TAB_DLG_NUM = 4 };
  enum { AUTO_TEST_TIME_ID = 1025};

  friend UINT StartUpdateThread(void *param);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
  virtual BOOL PreTranslateMessage(MSG* pMsg);

// 实现
private:
  int m_radioVal;
  CComboBox m_comComb;
  CComboBox m_ipComb;
  CListCtrl m_list;
  CButton m_searchBtn;

  CTabCtrl m_tab;
  CTestDlg m_testDlg;
  CUpdateDlg m_updateDlg;
  SetNetworkDlg m_setNetDlg;
  AlarmDlg m_alarmDlg;

  CDialog *m_tabDlg[TAB_DLG_NUM];  

  long m_hand[DEVICE_MAX_NUM];
  AFX_THREADPROC m_updateThread[DEVICE_MAX_NUM];
  CWinThread *m_winThread[DEVICE_MAX_NUM];
  int m_updateNum;

  BOOL InitComComb();
  BOOL InitIPComb();
  BOOL InitListCtl();
  BOOL InitTabCtl();

protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedButton1();
  afx_msg void OnBnClickedRadio1();
  afx_msg void OnBnClickedRadio2();
  afx_msg void OnBnClickedButton2();
  afx_msg void OnLvnItemchangedList4(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
};
