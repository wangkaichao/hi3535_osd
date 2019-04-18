#pragma once

// CUpdateDlg 对话框

class CUpdateDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateDlg)

public:
	CUpdateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUpdateDlg();
  void SetUpdatePos(int pos);
  void SetUpdateStep(int setup);
  void GetFileName(CString &filename);

// 对话框数据
	enum { IDD = IDD_UPDATEDLG };
  enum { MAX_PROGRESS_POS = 1024};
private:
  CString m_filename;

public:
  CEdit m_filenameEdt;
  CProgressCtrl m_updateGress;
  CStatic m_updateStatic;
  CButton m_updateBtn;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
  virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
  afx_msg void OnBnClickedButton1();
};
