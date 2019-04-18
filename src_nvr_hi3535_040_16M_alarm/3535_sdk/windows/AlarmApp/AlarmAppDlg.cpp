
// AlarmAppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AlarmApp.h"
#include "AlarmAppDlg.h"
#include "afxdialogex.h"

#pragma comment(lib, "NetLib_Win32D")
//#pragma comment(lib, "NetLib_Win32")
//#pragma comment(lib, "NetLib_Win64D")
//#pragma comment(lib, "NetLib_Win64")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//CAlarmAppDlg ����Ԫ�����������߳����
UINT StartUpdateThread(void *param)
{
  static int cnt = 0;

  int i = (int)param;
  CAlarmAppDlg *mainDlg = (CAlarmAppDlg *)AfxGetApp()->m_pMainWnd;
  CString filename;

  mainDlg->m_updateDlg.GetFileName(filename);

  int ret = NetLib_Update(&(mainDlg->m_hand[i]), (LPCTSTR)filename);

  return ret;
}

// CAlarmAppDlg �Ի���
CAlarmAppDlg::CAlarmAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAlarmAppDlg::IDD, pParent)
  , m_radioVal(0), m_updateNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  NetLib_Init();
  memset(m_hand, 0, sizeof(m_hand));
  memset(m_winThread, 0, sizeof(m_winThread));
  for (int i = 0; i < DEVICE_MAX_NUM; i++) {
    m_updateThread[i] = StartUpdateThread;
  }
}

CAlarmAppDlg::~CAlarmAppDlg()
{
  for (int i = 0; i < DEVICE_MAX_NUM; i++) {
    if (m_hand[i] != NULL) {
      closesocket(m_hand[i]);
      m_hand[i] = NULL;
      KillTimer(i);
    }
  }
  NetLib_DeInit();
}

BOOL CAlarmAppDlg::InitComComb()
{
  return true;
}

BOOL CAlarmAppDlg::InitIPComb()
{
#define LOCAL_IP_NUM  (64)

  char *iplist[LOCAL_IP_NUM];
  char buf[LOCAL_IP_NUM][32];
  int cnt;

  memset(iplist, 0, sizeof(iplist));
  for (int i = 0; i < LOCAL_IP_NUM; i++) {
    iplist[i] = buf[i];
  }

  Comm::GetLocalIPList(iplist, &cnt, LOCAL_IP_NUM);

  for (int i = 0; i < cnt; i++)
    m_ipComb.AddString(iplist[i]);

  m_ipComb.SetCurSel(0);

  return true;
}

BOOL CAlarmAppDlg::InitListCtl()
{
  m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
  //���ñ�ͷ
  CString strHeader[4] = { "���", "�豸IP", "�汾����", "״̬" };
  m_list.InsertColumn(0, strHeader[0], LVCFMT_LEFT, 50);
  m_list.InsertColumn(1, strHeader[1], LVCFMT_LEFT, 120);
  m_list.InsertColumn(2, strHeader[2], LVCFMT_LEFT, 100);
  m_list.InsertColumn(3, strHeader[3], LVCFMT_LEFT, 90);

  return true;
}

BOOL CAlarmAppDlg::InitTabCtl()
{
  //ΪTab Control��������ҳ��
  m_tab.InsertItem(0, _T("����"));
  m_tab.InsertItem(1, _T("����"));
  m_tab.InsertItem(2, _T("��������"));
  m_tab.InsertItem(3, _T("ͨ������"));

  m_testDlg.Create(IDD_TESTDLG, &m_tab);
  m_tabDlg[0] = &m_testDlg;
  m_updateDlg.Create(IDD_UPDATEDLG, &m_tab);
  m_tabDlg[1] = &m_updateDlg;
  m_setNetDlg.Create(IDD_SETNETWORKDLG, &m_tab);
  m_tabDlg[2] = &m_setNetDlg;
  m_alarmDlg.Create(IDD_ALARMDLG, &m_tab);
  m_tabDlg[3] = &m_alarmDlg;

  //�趨��Tab����ʾ�ķ�Χ
  CRect rc;
  m_tab.GetClientRect(rc);
  rc.top += 20;
  rc.bottom -= 0;
  rc.left += 0;
  rc.right -= 0;
  for (int i = 0; i < TAB_DLG_NUM; i++){
    m_tabDlg[i]->MoveWindow(&rc);
    m_tabDlg[i]->ShowWindow(SW_HIDE);
  }
  m_tabDlg[0]->ShowWindow(SW_SHOW);

  return true;
}

//֧�ִ��ں�����ģʽ
int CAlarmAppDlg::SndAlarm(const char *alarm, int num)
{
  if (m_radioVal == 0) {
    int cnt = m_list.GetItemCount();
    for (int i = 0; i < cnt; i++)
      if (m_list.GetCheck(i) && m_hand[i] != NULL)
        NetLib_SndAlarm(&m_hand[i], alarm, num);
  }
  else {
    AfxMessageBox("����ģʽû��ʵ��");
  }

  return 0;
}

//֧�ִ��ں�����ģʽ
int CAlarmAppDlg::AutoTest(BOOL enable)
{
  if (m_radioVal == 0) {
    if (enable) {
      SetTimer(AUTO_TEST_TIME_ID, 1000, NULL);
    }
    else {
      KillTimer(AUTO_TEST_TIME_ID);
    }
  }
  else {
    AfxMessageBox("����ģʽû��ʵ��");
  }

  return 0;
}

int CAlarmAppDlg::SetNet(const char *ip, const char *submask, const char *gateway)
{
  logDbg("ip[%s] submask[%s] gateway[%s]", ip, submask, gateway);

  int cnt = m_list.GetItemCount();
  if (cnt != 1) {
    AfxMessageBox("ֻ�ܹ�ѡһ̨�豸����������");
    return -1;
  }

  int ret, i;
  for (i = 0; i < cnt; i++) {
    if (m_list.GetCheck(i) && m_hand[i] != NULL) {
      ret = NetLib_SetNet(&m_hand[i], ip, submask, gateway);
      break;
    }
  }

  if (ret < 0) {
    AfxMessageBox("��������ʧ�ܣ�");
  }
  else {
    NetLib_Reboot(&m_hand[i]);
    AfxMessageBox("�������óɹ����豸������������");
  }

  return ret;
}

int CAlarmAppDlg::UpdateFile(const char *filename)
{
  if (strlen(filename) == 0) {
    AfxMessageBox("�����ļ�Ϊ�գ�");
    return -1;
  }
    
  if (m_radioVal == 0) {
    int cnt = m_list.GetItemCount();

    for (int i = 0; i < cnt; i++) {
      if (m_list.GetCheck(i) && m_hand[i] != NULL) {
        m_updateNum++;
      }
    }

    if (m_updateNum == 0) {
      AfxMessageBox("�豸δ���ӣ�");
      return -1;
    }

    int step = CUpdateDlg::MAX_PROGRESS_POS / m_updateNum;
    int idx = 0;
    
    SetCapture();
    ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
    m_updateDlg.m_updateBtn.EnableWindow(0);
    // ���߳����洦�������
    m_updateDlg.SetUpdatePos(0);
    for (int i = 0; i < cnt; i++) {
      if (m_list.GetCheck(i) && m_hand[i] != NULL) {
        //�����̣߳���ʼΪ����״̬
        m_winThread[i] = AfxBeginThread(m_updateThread[i], (LPVOID)i, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
        //�߳̽���ʱ���Զ�ɾ��m_winThread[i]
        m_winThread[i]->m_bAutoDelete = FALSE;
        //�ָ��߳�����
        m_winThread[i]->ResumeThread();
      }
    }
    
    while (m_updateNum) {
      for (int i = 0; i < cnt; i++) {
        if (m_winThread[i] != NULL) {
          DWORD exitcode;
          GetExitCodeThread(m_winThread[i]->m_hThread, &exitcode);
          if (exitcode == STILL_ACTIVE) {
            logDbg("still run");
            int pos = m_updateDlg.m_updateGress.GetPos();
            if (pos < (idx + 1)*step - 48)
              m_updateDlg.m_updateGress.StepIt();
            continue;
          }

          delete m_winThread[i];
          m_winThread[i] = NULL;
          m_updateNum--;

          int ret = (int)exitcode;
          logDbg("ret=%d", ret);
          if (ret < 0) {
            m_list.SetItemText(i, 3, "����ʧ��");
          }
          else {
            m_list.SetItemText(i, 3, "�����ɹ�");
            // ����ʱ��2sʱ������Կ�������״̬
            //Sleep(2000);
            NetLib_Reboot(&m_hand[i]);
          }
          m_updateDlg.SetUpdatePos(++idx * step);
        }
      }

      Sleep(100);
    }
    
    ReleaseCapture();
    ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
    m_updateDlg.m_updateBtn.EnableWindow(1);
  }
  else {
    AfxMessageBox("����ģʽû��ʵ��");
  }

  return 0;
}

//֧�ִ��ں�����ģʽ
int CAlarmAppDlg::SetALarm(const alarm_config_t *alarm)
{
  int ret;

  if (m_radioVal == 0) {
    int cnt = m_list.GetItemCount();
    if (cnt != 1) {
      AfxMessageBox("�빴ѡһ̨�豸��ͨ������");
      return -1;
    }

    for (int i = 0; i < cnt; i++) {
      if (m_list.GetCheck(i) && m_hand[i] != NULL) {
        ret = NetLib_SetAlarm(&m_hand[i], alarm);
        break;
      }
    }

    if (ret < 0) {
      AfxMessageBox("ͨ������ʧ�ܣ�");
    }
  }
  else {
    AfxMessageBox("����ģʽû��ʵ��");
  }

  return ret;
}

int CAlarmAppDlg::GetAlarm(alarm_config_t *alarm)
{
  int ret;

  if (m_radioVal == 0) {
    int cnt = m_list.GetItemCount();
    if (cnt != 1) {
      AfxMessageBox("ֻ�ܹ�ѡһ̨�豸��ͨ����ѯ");
      return -1;
    }

    for (int i = 0; i < cnt; i++) {
      if (m_list.GetCheck(i) && m_hand[i] != NULL) {
        ret = NetLib_GetAlarm(&m_hand[i], alarm);
        break;
      }
    }

    if (ret < 0) {
      AfxMessageBox("ͨ����ȡʧ�ܣ�");
    }
  }
  else {
    AfxMessageBox("����ģʽû��ʵ��");
  }

  return ret;
}

void CAlarmAppDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);

  DDX_Radio(pDX, IDC_RADIO1, m_radioVal);
  DDV_MinMaxInt(pDX, m_radioVal, -1, 1);
  DDX_Control(pDX, IDC_COMBO2, m_comComb);
  DDX_Control(pDX, IDC_COMBO4, m_ipComb);
  DDX_Control(pDX, IDC_LIST4, m_list);
  DDX_Control(pDX, IDC_BUTTON2, m_searchBtn);

  DDX_Control(pDX, IDC_TAB1, m_tab);
  //DDX_Control(pDX, IDD_TESTDLG, m_testDlg);
  //DDX_Control(pDX, IDD_UPDATEDLG, m_updateDlg);
  //DDX_Control(pDX, IDD_SETNETWORKDLG, m_setNetDlg);
}

// ���οͻ�����esc����enter���˳�����  
BOOL CAlarmAppDlg::PreTranslateMessage(MSG* pMsg)
{
  // TODO: �ڴ����ר�ô����/����û���  
  if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)// ����esc��  
  {
    return TRUE;// �����κβ���  
  }
  if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)// ����enter��  
  {
    return TRUE;// �����κδ���  
  }
  return CDialogEx::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CAlarmAppDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()

  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAlarmAppDlg::OnTcnSelchangeTab1)
  ON_BN_CLICKED(IDC_BUTTON1, &CAlarmAppDlg::OnBnClickedButton1)
  ON_BN_CLICKED(IDC_RADIO1, &CAlarmAppDlg::OnBnClickedRadio1)
  ON_BN_CLICKED(IDC_RADIO2, &CAlarmAppDlg::OnBnClickedRadio2)
  ON_BN_CLICKED(IDC_BUTTON2, &CAlarmAppDlg::OnBnClickedButton2)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST4, &CAlarmAppDlg::OnLvnItemchangedList4)
  ON_WM_TIMER()
END_MESSAGE_MAP()

// CAlarmAppDlg ��Ϣ�������

BOOL CAlarmAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
#if USE_DEBUG
  Comm::InitConsoleWindow();
  logDbg("str = %s\n ", "Debug output goes to terminal\n");
#endif
  
  InitComComb();
  InitIPComb();
  InitListCtl();
  InitTabCtl();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAlarmAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAlarmAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAlarmAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// �����ǩҳ
void CAlarmAppDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
  int CurSel = m_tab.GetCurSel();

  for (int i = 0; i < TAB_DLG_NUM; i++)
    m_tabDlg[i]->ShowWindow(i == CurSel ? true : false);

  *pResult = 0;
}

//���ˢ�°�ť
void CAlarmAppDlg::OnBnClickedButton1()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  m_ipComb.ResetContent();
  InitIPComb();
}

//�������ģʽ��ť
void CAlarmAppDlg::OnBnClickedRadio1()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  m_radioVal = 0;
  m_ipComb.EnableWindow(1);
  m_list.EnableWindow(1);
  m_searchBtn.EnableWindow(1);
  m_setNetDlg.EnableWindow(1);
}

//�������ģʽ��ť
void CAlarmAppDlg::OnBnClickedRadio2()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  m_radioVal = 1;
  m_ipComb.EnableWindow(0);
  m_list.EnableWindow(0);
  m_searchBtn.EnableWindow(0);
  m_setNetDlg.EnableWindow(0);
}

//����豸������ť
void CAlarmAppDlg::OnBnClickedButton2()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  int cnt = m_list.GetItemCount();
  for (int i = 0; i < cnt; i++) {
    if (m_list.GetCheck(i))
      KillTimer(i);

    if (m_hand[i] != NULL)
      NetLib_Close(&m_hand[i]);
  }
  m_list.DeleteAllItems();

  search_st list[256];
  int len;
  SetCapture();
  ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
  NetLib_Search(list, &len, DEVICE_MAX_NUM, 1000);
  ReleaseCapture();
  ::SetCursor(::LoadCursor(NULL, IDC_ARROW));

  for (int i = 0; i < len; i++) {
    logDbg("[%s][%s][%s]", list[i].ip_addr, list[i].mac_addr, list[i].version);
    CString strid;
    strid.Format("%d", i);
    m_list.InsertItem(i, strid);
    m_list.SetItemText(i, 1, list[i].ip_addr);
    m_list.SetItemText(i, 2, list[i].version);
  }
}

//��ѡ��ȡ����ѡ�����豸��
void CAlarmAppDlg::OnLvnItemchangedList4(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  
  if ((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) /* old state : unchecked */
    && (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2)) /* new state : checked */
    )
  {
    logDbg("Item %d is checked\n", pNMLV->iItem);
    CString strip = m_list.GetItemText(pNMLV->iItem, 1);
    int ret = NetLib_Open(&m_hand[pNMLV->iItem], (LPCTSTR)strip, SERVER_PORT);
    if (ret < 0) {
      m_list.SetItemText(pNMLV->iItem, 3, "����ʧ��");
    }
    else {
      m_list.SetItemText(pNMLV->iItem, 3, "���ӳɹ�");
    }
    SetTimer(pNMLV->iItem, 1000, NULL);
  }
  else if ((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) /* old state : checked */
    && (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)) /* new state : unchecked */
    )
  {
    logDbg("Item %d is unchecked\n", pNMLV->iItem);
    KillTimer(pNMLV->iItem);
    if (m_hand[pNMLV->iItem] != NULL)
      NetLib_Close(&m_hand[pNMLV->iItem]);
    m_list.SetItemText(pNMLV->iItem, 3, "");
  }
  else
  {
    logDbg("Item %d does't change the check-status\n", pNMLV->iItem);
  }

  *pResult = 0;
}

void CAlarmAppDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
  // �豸��������
  if (nIDEvent >= 0 && nIDEvent < DEVICE_MAX_NUM) {
    if (m_hand[nIDEvent] != NULL) {
      // ����ʱ���ܷ�������
      if (!m_updateDlg.m_updateBtn.IsWindowEnabled())
        goto END;

      int ret = NetLib_HeartBeat(&m_hand[nIDEvent]);
      if (ret < 0) {
        NetLib_Close(&m_hand[nIDEvent]);
        //�б�״̬����ʾ
        m_list.SetItemText(nIDEvent, 3, "����ʧ��");
      }
    }
    else {
      CString strip = m_list.GetItemText(nIDEvent, 1);
      int ret = NetLib_Open(&m_hand[nIDEvent], (LPCTSTR)strip, SERVER_PORT);
      if (ret == 0) {
        m_list.SetItemText(nIDEvent, 3, "���ӳɹ�");
      }
    }
  }
  else if (nIDEvent == AUTO_TEST_TIME_ID) {
    int cnt = m_list.GetItemCount();
    for (int i = 0; i < cnt; i++)
      if (m_list.GetCheck(i) && m_hand[i] != NULL) {
        NetLib_SndAlarm(&m_hand[i], m_testDlg.GetTestChState(), CTestDlg::ALARM_NUM);
      }
  }

END:
  CDialogEx::OnTimer(nIDEvent);
}
