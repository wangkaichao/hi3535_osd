#pragma once
// SetNetworkDlg �Ի���

class SetNetworkDlg : public CDialog
{
	DECLARE_DYNAMIC(SetNetworkDlg)

public:
	SetNetworkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SetNetworkDlg();

// �Ի�������
	enum { IDD = IDD_SETNETWORKDLG };

public:
  CIPAddressCtrl m_ipCtl;
  CIPAddressCtrl m_submaskCtl;
  CIPAddressCtrl m_gatewayCtl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton1();
};
