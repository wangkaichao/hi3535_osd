#pragma once
// SetNetworkDlg 对话框

class SetNetworkDlg : public CDialog
{
	DECLARE_DYNAMIC(SetNetworkDlg)

public:
	SetNetworkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SetNetworkDlg();

// 对话框数据
	enum { IDD = IDD_SETNETWORKDLG };

public:
  CIPAddressCtrl m_ipCtl;
  CIPAddressCtrl m_submaskCtl;
  CIPAddressCtrl m_gatewayCtl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton1();
};
