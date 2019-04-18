#pragma once

// CTestDlg 对话框

class CTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestDlg();
  
// 对话框数据
	enum { IDD = IDD_TESTDLG };
  enum { ALARM_NUM = 64};

private:
  char m_chState[ALARM_NUM];

public:
  CButton m_alarmBtn[ALARM_NUM];
  CButton m_setBtn;
  CButton m_testBtn;

public:
  const char *GetTestChState();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedCheck1();
  afx_msg void OnBnClickedCheck2();
  afx_msg void OnBnClickedCheck3();
  afx_msg void OnBnClickedCheck4();
  afx_msg void OnBnClickedCheck5();
  afx_msg void OnBnClickedCheck6();
  afx_msg void OnBnClickedCheck7();
  afx_msg void OnBnClickedCheck8();
  afx_msg void OnBnClickedCheck9();
  afx_msg void OnBnClickedCheck10();
  afx_msg void OnBnClickedCheck11();
  afx_msg void OnBnClickedCheck12();
  afx_msg void OnBnClickedCheck13();
  afx_msg void OnBnClickedCheck14();
  afx_msg void OnBnClickedCheck15();
  afx_msg void OnBnClickedCheck16();
  afx_msg void OnBnClickedCheck17();
  afx_msg void OnBnClickedCheck18();
  afx_msg void OnBnClickedCheck19();
  afx_msg void OnBnClickedCheck20();
  afx_msg void OnBnClickedCheck21();
  afx_msg void OnBnClickedCheck22();
  afx_msg void OnBnClickedCheck23();
  afx_msg void OnBnClickedCheck24();
  afx_msg void OnBnClickedCheck25();
  afx_msg void OnBnClickedCheck26();
  afx_msg void OnBnClickedCheck27();
  afx_msg void OnBnClickedCheck28();
  afx_msg void OnBnClickedCheck29();
  afx_msg void OnBnClickedCheck30();
  afx_msg void OnBnClickedCheck31();
  afx_msg void OnBnClickedCheck32();
  afx_msg void OnBnClickedCheck33();
  afx_msg void OnBnClickedCheck34();
  afx_msg void OnBnClickedCheck35();
  afx_msg void OnBnClickedCheck36();
  afx_msg void OnBnClickedCheck37();
  afx_msg void OnBnClickedCheck38();
  afx_msg void OnBnClickedCheck39();
  afx_msg void OnBnClickedCheck40();
  afx_msg void OnBnClickedCheck41();
  afx_msg void OnBnClickedCheck42();
  afx_msg void OnBnClickedCheck43();
  afx_msg void OnBnClickedCheck44();
  afx_msg void OnBnClickedCheck45();
  afx_msg void OnBnClickedCheck46();
  afx_msg void OnBnClickedCheck47();
  afx_msg void OnBnClickedCheck48();
  afx_msg void OnBnClickedCheck49();
  afx_msg void OnBnClickedCheck50();
  afx_msg void OnBnClickedCheck51();
  afx_msg void OnBnClickedCheck52();
  afx_msg void OnBnClickedCheck53();
  afx_msg void OnBnClickedCheck54();
  afx_msg void OnBnClickedCheck55();
  afx_msg void OnBnClickedCheck56();
  afx_msg void OnBnClickedCheck57();
  afx_msg void OnBnClickedCheck58();
  afx_msg void OnBnClickedCheck59();
  afx_msg void OnBnClickedCheck60();
  afx_msg void OnBnClickedCheck61();
  afx_msg void OnBnClickedCheck62();
  afx_msg void OnBnClickedCheck63();
  afx_msg void OnBnClickedCheck64();
  afx_msg void OnBnClickedButton2();
  afx_msg void OnBnClickedButton1();
};
