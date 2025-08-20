
// SMITerminalDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SMITerminal.h"
#include "SMITerminalDlg.h"
#include "afxdialogex.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSMITerminalDlg dialog



CSMITerminalDlg::CSMITerminalDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SMITERMINAL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSMITerminalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_edit);

}

BEGIN_MESSAGE_MAP(CSMITerminalDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CSMITerminalDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CSMITerminalDlg::OnNMCustomdrawList1)
	ON_EN_SETFOCUS(IDC_EDIT1, &CSMITerminalDlg::OnEnSetfocusEdit1)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT1, &CSMITerminalDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CSMITerminalDlg message handlers

BOOL CSMITerminalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.
	ModifyStyle(0, WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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


	m_commands[L"ECON"] = [this]() { HandleEcon(); };
	m_commands[L"NEW"] = [this]() {
		// Duplicate dialog
		CSMITerminalDlg* pNewDlg = new CSMITerminalDlg();
		pNewDlg->Create(IDD_SMITERMINAL_DIALOG);
		pNewDlg->ShowWindow(SW_SHOW);
		};
	
	m_commands[L"INDEX"] = [this]() { ShowIndex(); };
	m_commands[L"COMM"] = [this]() { ShowComm(); };
	m_commands[L"NEWS"] = [this]() { ShowNews(); };
	m_commands[L"RATE"] = [this]() { ShowRate(); };
	m_commands[L"FOREX"] = [this]() { ShowForex(); };
	m_commands[L"IPO"] = [this]() { ShowIPO(); };
	m_commands[L"EARN"] = [this]() { ShowEarnings(); };
	m_commands[L"EQMETRIC"] = [this]() { ShowEqMetric(); };
	m_commands[L"CLEAR"] = [this]() {
		m_listCtrl.DeleteAllItems();
		m_gridCount = 0;
		if (m_listCtrl.GetHeaderCtrl()->GetItemCount() > 1) {
			m_listCtrl.DeleteColumn(1);
		}
		};
	m_edit.ModifyStyle(0, ES_AUTOVSCROLL | ES_WANTRETURN); // allow return key

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_listFont.CreatePointFont(100, _T("Consolas"));
	SetBackgroundColor(RGB(0, 0, 0));
	m_listCtrl.SetFont(&m_listFont);

	m_listCtrl.SetBkColor(RGB(0, 0, 255));   // background black
	m_listCtrl.SetTextBkColor(RGB(0, 0, 0)); // text background also black
	m_listCtrl.SetTextColor(RGB(255, 255, 0)); // text green (optional for visibility)

	
	m_listCtrl.InsertColumn(0, L"Event", LVCFMT_LEFT, 900); 
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CHeaderCtrl* pHeader = m_listCtrl.GetHeaderCtrl();
	if (pHeader)
		pHeader->ShowWindow(SW_HIDE);

	
	m_edit.SetFont(&m_listFont);
	
	

	CString availableCommands =
		L"DIRECTORY:\r\n"
		L"NEW    = NEW DIALOG\r\n"
		L"ECON   = ECONOMIC DATA OVERVIEW\r\n"
		L"INDEX  = INDEX QUOTES\r\n"
		L"COMM   = COMMODITY FUT QUOTES\r\n"
		L"NEWS   = GENERAL NEWS\r\n"
		L"RATE   = TREASURY RATE\r\n"
		L"FOREX  = FOREX QUOTE\r\n"
		L"IPO    = IPO CALENDAR\r\n"
		L"EARN   = EARNINGS CALENDAR\r\n"
		L"EQMETRIC = EQUITY INFO\r\n\r\n";


	m_edit.SetWindowTextW(availableCommands);
	m_edit.SetFont(&m_listFont);

	m_protectedLength = availableCommands.GetLength();

	m_edit.SetSel(m_protectedLength, m_protectedLength);
	m_edit.SetFocus();

	return TRUE;  
}

void CSMITerminalDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


void CSMITerminalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CSMITerminalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
BOOL CSMITerminalDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CString text;
		m_edit.GetWindowTextW(text);
		text.Trim();

		int pos = text.ReverseFind(L'\n');
		CString command = (pos >= 0) ? text.Mid(pos + 1) : text;
		command.Trim();

		if (m_commands.find(command) != m_commands.end())
			m_commands[command]();
		else
			HandleUnknown(command);

		return TRUE; 
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSMITerminalDlg::HandleEcon()
{
	OnBnClickedButton1();
}

void CSMITerminalDlg::HandleUnknown(const CString& cmd)
{
	int row = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(),
		L"Unknown command: " + cmd);
}

void CSMITerminalDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}

void CSMITerminalDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
}
CStringW FetchEconomicCalendar()
{
    COleDateTime localNow = COleDateTime::GetCurrentTime();
    TIME_ZONE_INFORMATION tzInfo;
    GetTimeZoneInformation(&tzInfo);

    long localOffsetMinutes = -(long)tzInfo.Bias;
    double localOffsetHours = localOffsetMinutes / 60.0;
    COleDateTime utcNow = localNow - COleDateTimeSpan(0, (int)localOffsetHours, 0, 0);

    COleDateTime utcPlus1Now = utcNow + COleDateTimeSpan(0, 1, 0, 0);

    CStringW currentDate = utcPlus1Now.Format(L"%Y-%m-%d");

    CStringW apiPath;
    apiPath.Format(L"/api/v3/economic_calendar?from=%s&to=%s&apikey=JXJBxJf08MDcoGMU5yMYJoWNAq2r7csN",
        currentDate.GetString(), currentDate.GetString());

    CStringW result;
    HINTERNET hSession = WinHttpOpen(L"SMI Terminal/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return L"";

    HINTERNET hConnect = WinHttpConnect(hSession,
        L"financialmodelingprep.com",
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return L"";
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"GET",
        apiPath,
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return L"";
    }

    BOOL bResults = WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0, WINHTTP_NO_REQUEST_DATA, 0,
        0, 0);
    if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);

    DWORD dwSize = 0, dwDownloaded = 0;
    std::string response;
    if (bResults)
    {
        do
        {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (!dwSize) break;
            std::vector<char> buffer(dwSize + 1, 0);
            if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) break;
            response.append(buffer.data(), dwDownloaded);
        } while (dwSize > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return CStringW(CA2W(response.c_str(), CP_UTF8));
}

void CSMITerminalDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_listCtrl.GetSafeHwnd())
		m_listCtrl.MoveWindow(0, 0, cx, cy);

	CDialogEx::OnSize(nType, cx, cy);

	if (m_listCtrl.GetSafeHwnd())
	{
		m_listCtrl.MoveWindow(0, 0, cx, cy);

		
	}
}




void CSMITerminalDlg::OnEnSetfocusEdit1()
{
	
}

HBRUSH CSMITerminalDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_EDIT1) 
	{
		pDC->SetTextColor(RGB(255, 255, 0)); 
		pDC->SetBkColor(RGB(0, 0, 255));        
		return m_brushBlack;
	}

	return hbr;
}



CStringW FetchCommodityQuotes()
{
	CStringW result;

	HINTERNET hSession = WinHttpOpen(L"SMI Terminal/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession) return L"";

	HINTERNET hConnect = WinHttpConnect(hSession,
		L"financialmodelingprep.com",
		INTERNET_DEFAULT_HTTPS_PORT, 0);

	if (!hConnect) { WinHttpCloseHandle(hSession); return L""; }

	const CString symbols = L"ESUSD,RTYUSD,NQUSD,YMUSD,DXUSD,GCUSD,PAUSD,SIUSD,CLUSD,BZUSD,NGUSD,HOUSD,RBUSD,KEUSX,ZLUSD,ZOUSX,ZCUSX,CCUSD,DCUSD,CTUSX,LEUSX,DCUSD,OJUSX";

	CString url;
	url.Format(L"/api/v3/quote/%s?apikey=JXJBxJf08MDcoGMU5yMYJoWNAq2r7csN", symbols);

	HINTERNET hRequest = WinHttpOpenRequest(hConnect,
		L"GET",
		url,
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_SECURE);

	if (!hRequest) {
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return L"";
	}

	LPCWSTR headers = L"User-Agent: Mozilla/5.0\r\n";

	BOOL bResults = WinHttpSendRequest(hRequest,
		headers,
		-1L,
		WINHTTP_NO_REQUEST_DATA, 0,
		0, 0);

	if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);

	DWORD dwSize = 0, dwDownloaded = 0;
	std::string response;

	if (bResults)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
			if (!dwSize) break;

			std::vector<char> buffer(dwSize + 1, 0);

			if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) break;

			response.append(buffer.data(), dwDownloaded);

		} while (dwSize > 0);
	}

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	return CStringW(CA2W(response.c_str(), CP_UTF8));
}




CStringW FetchQuotes(const CString& symbols)
{
	CStringW result;

	HINTERNET hSession = WinHttpOpen(L"SMI Terminal/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession) return L"";

	HINTERNET hConnect = WinHttpConnect(hSession,
		L"financialmodelingprep.com",
		INTERNET_DEFAULT_HTTPS_PORT, 0);

	if (!hConnect) { WinHttpCloseHandle(hSession); return L""; }

	CString url;
	url.Format(L"/api/v3/quote/%s?apikey=JXJBxJf08MDcoGMU5yMYJoWNAq2r7csN", symbols);

	HINTERNET hRequest = WinHttpOpenRequest(hConnect,
		L"GET",
		url,
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_SECURE);

	if (!hRequest) {
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return L"";
	}

	LPCWSTR headers = L"User-Agent: Mozilla/5.0\r\n";

	BOOL bResults = WinHttpSendRequest(hRequest,
		headers,
		-1L,
		WINHTTP_NO_REQUEST_DATA, 0,
		0, 0);

	if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);

	DWORD dwSize = 0, dwDownloaded = 0;
	std::string response;

	if (bResults)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
			if (!dwSize) break;

			std::vector<char> buffer(dwSize + 1, 0);

			if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) break;

			response.append(buffer.data(), dwDownloaded);

		} while (dwSize > 0);
	}

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	return CStringW(CA2W(response.c_str(), CP_UTF8));
}






void CSMITerminalDlg::OnBnClickedButton1()
{
    if (m_gridCount >= 6) {
        AfxMessageBox(L"Maximum of 6 grids allowed.");
        return;
    }

    m_listCtrl.SetTextColor(RGB(255, 255, 0));

    CStringW jsonText = FetchEconomicCalendar();
    if (jsonText.IsEmpty()) {
        AfxMessageBox(L"Failed to fetch data.");
        return;
    }

    json j;
    try {
        CT2A utf8(jsonText, CP_UTF8);
        std::string jsonStr(utf8);
        j = json::parse(jsonStr);
    }
    catch (const std::exception& e) {
        CStringW err(L"JSON parse error: ");
        err += CA2W(e.what());
        AfxMessageBox(err);
        return;
    }

    struct EconEvent
    {
        CString time, event, country, prev, act, est;
        COleDateTime dt;
        bool hasActual;
    };
    std::vector<EconEvent> events;

    // Get current local time and timezone information
    COleDateTime now = COleDateTime::GetCurrentTime();
    TIME_ZONE_INFORMATION tzInfo;
    GetTimeZoneInformation(&tzInfo);

    for (auto& item : j)
    {
        EconEvent e;
        e.time = item.value("date", "").c_str();
        e.event = item.value("event", "").c_str();
        e.country = item.value("country", "").c_str();

        if (!item["previous"].is_null())
            e.prev.Format(L"%.3f", item["previous"].get<double>());
        else
            e.prev = L"-";

        if (!item["actual"].is_null()) {
            e.act.Format(L"%.3f", item["actual"].get<double>());
            e.hasActual = true;
        }
        else {
            e.act = L"-";
            e.hasActual = false;
        }

        if (!item["estimate"].is_null())
            e.est.Format(L"%.3f", item["estimate"].get<double>());
        else
            e.est = L"-";

        // Parse the UTC+1 time and convert to local time
        COleDateTime utcPlus1Time;
        if (utcPlus1Time.ParseDateTime(e.time))
        {
            // Convert from UTC+1 to UTC first (subtract 1 hour)
            COleDateTime utcTime = utcPlus1Time - COleDateTimeSpan(0, 1, 0, 0);

            // Convert from UTC to local time
            // tzInfo.Bias is in minutes, positive values are west of UTC
            long localOffsetMinutes = -(long)tzInfo.Bias;
            double offsetHours = localOffsetMinutes / 60.0;
            e.dt = utcTime + COleDateTimeSpan(0, (int)offsetHours, 0, 0);
        }
        else {
            e.dt = COleDateTime(); // Invalid date
        }

        events.push_back(e);
    }

    // Remove events with invalid dates
    events.erase(std::remove_if(events.begin(), events.end(),
        [](const EconEvent& e) { return e.dt.m_dt == 0; }), events.end());

    // Sort events by date/time
    std::sort(events.begin(), events.end(), [](const EconEvent& a, const EconEvent& b) {
        return a.dt < b.dt;
        });

    // Find events with actual values that are in the past (before current time)
    std::vector<EconEvent> pastEventsWithActual;
    std::vector<EconEvent> futureEvents;

    for (const auto& event : events) {
        if (event.dt < now && event.hasActual) {
            pastEventsWithActual.push_back(event);
        }
        else if (event.dt >= now) {
            futureEvents.push_back(event);
        }
    }

    // Get the 5 most recent events with actual values
    std::vector<EconEvent> selectedEvents;
    int recentActualCount = min(5, (int)pastEventsWithActual.size());
    if (recentActualCount > 0) {
        // Take the last 5 events (most recent) from pastEventsWithActual
        for (int i = pastEventsWithActual.size() - recentActualCount; i < pastEventsWithActual.size(); ++i) {
            selectedEvents.push_back(pastEventsWithActual[i]);
        }
    }

    // Add up to 5 future events
    int futureCount = min(5, (int)futureEvents.size());
    for (int i = 0; i < futureCount; ++i) {
        selectedEvents.push_back(futureEvents[i]);
    }

    // If we don't have enough events, fill with whatever is available
    if (selectedEvents.size() < 10) {
        // Add more past events with actual values if available
        int additionalPastNeeded = min(10 - (int)selectedEvents.size(),
            (int)pastEventsWithActual.size() - recentActualCount);
        for (int i = pastEventsWithActual.size() - recentActualCount - additionalPastNeeded;
            i < pastEventsWithActual.size() - recentActualCount && additionalPastNeeded > 0;
            ++i, --additionalPastNeeded) {
            selectedEvents.insert(selectedEvents.begin(), pastEventsWithActual[i]);
        }

        // Add more future events if still needed and available
        int additionalFutureNeeded = min(10 - (int)selectedEvents.size(),
            (int)futureEvents.size() - futureCount);
        for (int i = futureCount; i < futureCount + additionalFutureNeeded; ++i) {
            selectedEvents.push_back(futureEvents[i]);
        }
    }

    // Re-sort the selected events by time to maintain chronological order
    std::sort(selectedEvents.begin(), selectedEvents.end(), [](const EconEvent& a, const EconEvent& b) {
        return a.dt < b.dt;
        });

    // Calculate column widths for proper formatting
    int maxTime = 0, maxEvent = 0, maxCountry = 0, maxPrev = 0, maxAct = 0, maxEst = 0;
    for (const auto& e : selectedEvents)
    {
        maxTime = max(maxTime, e.time.GetLength());
        maxEvent = max(maxEvent, e.event.GetLength());
        maxCountry = max(maxCountry, e.country.GetLength());
        maxPrev = max(maxPrev, e.prev.GetLength());
        maxAct = max(maxAct, e.act.GetLength());
        maxEst = max(maxEst, e.est.GetLength());
    }

    auto PadRight = [](const CString& s, int width) -> CString
        {
            CString padded = s;
            while (padded.GetLength() < width)
                padded += L" ";
            return padded;
        };

    int columnIndex = (m_gridCount < 3) ? 0 : 1;

    if (columnIndex == 1 && m_listCtrl.GetHeaderCtrl()->GetItemCount() == 1) {
        m_listCtrl.InsertColumn(1, L"Event2", LVCFMT_LEFT, 900);
    }

    CString headerRow;
    headerRow = PadRight(L"Time", maxTime) + L" < " +
        PadRight(L"Event", maxEvent) + L" >< " +
        PadRight(L"CY", maxCountry) + L" >< " +
        PadRight(L"Prev", maxPrev) + L" | " +
        PadRight(L"Act", maxAct) + L" | " +
        PadRight(L"Est", maxEst) + L" > ";

    CString titleRow;
    CString currentTime;
    currentTime.Format(L"%s", now.Format(L"%Y-%m-%d %H:%M").GetString());
    titleRow = PadRight(currentTime, maxTime) + L" | " +
        PadRight(L"MacroEconomic Data Overview",
            maxEvent + maxCountry + maxPrev + maxAct + maxEst + 5 * 3);

    if (columnIndex == 0) {
        // Column 0: Insert at bottom
        int titleIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), titleRow);
        int headerIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), headerRow);

        for (const auto& e : selectedEvents)
        {
            CString rowText;
            rowText = PadRight(e.time, maxTime) + L" < " +
                PadRight(e.event, maxEvent) + L" >< " +
                PadRight(e.country, maxCountry) + L" >< " +
                PadRight(e.prev, maxPrev) + L" | " +
                PadRight(e.act, maxAct) + L" | " +
                PadRight(e.est, maxEst) + L" > ";

            m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), rowText);
        }
    }
    else {
        int startRow = FindNextAvailableRow();
        int rowsNeeded = (int)selectedEvents.size() + 2; //offset for headers

        int currentRowCount = m_listCtrl.GetItemCount();
        while (currentRowCount < startRow + rowsNeeded) {
            m_listCtrl.InsertItem(currentRowCount++, L"");
        }

        m_listCtrl.SetItemText(startRow, 1, titleRow);
        m_listCtrl.SetItemText(startRow + 1, 1, headerRow);

        int rowIndex = 0;
        for (const auto& e : selectedEvents)
        {
            CString rowText;
            rowText = PadRight(e.time, maxTime) + L" < " +
                PadRight(e.event, maxEvent) + L" >< " +
                PadRight(e.country, maxCountry) + L" >< " +
                PadRight(e.prev, maxPrev) + L" | " +
                PadRight(e.act, maxAct) + L" | " +
                PadRight(e.est, maxEst) + L" > ";

            m_listCtrl.SetItemText(startRow + 2 + rowIndex++, 1, rowText);
        }
    }

    m_gridCount++;
}

void CSMITerminalDlg::ShowComm()
{
    if (m_gridCount >= 6) {
        AfxMessageBox(L"Maximum of 6 grids allowed.");
        return;
    }

    m_listCtrl.SetTextColor(RGB(0, 255, 0));

    CStringW jsonText = FetchCommodityQuotes();
    if (jsonText.IsEmpty()) {
        AfxMessageBox(L"Failed to fetch commodity quotes.");
        return;
    }

    json j;
    try {
        CT2A utf8(jsonText, CP_UTF8);
        std::string jsonStr(utf8);
        j = json::parse(jsonStr);
    }
    catch (const std::exception& e) {
        CStringW err(L"JSON parse error: ");
        err += CA2W(e.what());
        AfxMessageBox(err);
        return;
    }

    struct Quote
    {
        CString symbol, last, volume, change;
    };
    std::vector<Quote> quotes;

    for (auto& item : j)
    {
        Quote q;
        q.symbol = item.value("symbol", "").c_str();

        if (!item["price"].is_null())
            q.last.Format(L"%.2f", item["price"].get<double>());
        else
            q.last = L"-";

        if (!item["volume"].is_null())
            q.volume.Format(L"%lld", item["volume"].get<long long>());
        else
            q.volume = L"-";

        if (!item["change"].is_null())
            q.change.Format(L"%.2f", item["change"].get<double>());
        else
            q.change = L"-";

        quotes.push_back(q);
    }

    int maxSymbol = 0, maxLast = 0, maxVolume = 0, maxChange = 0;
    for (auto& q : quotes) {
        maxSymbol = max(maxSymbol, q.symbol.GetLength());
        maxLast = max(maxLast, q.last.GetLength());
        maxVolume = max(maxVolume, q.volume.GetLength());
        maxChange = max(maxChange, q.change.GetLength());
    }

    auto PadRight = [](const CString& s, int width) -> CString
        {
            CString padded = s;
            while (padded.GetLength() < width)
                padded += L" ";
            return padded;
        };

    int columnIndex = (m_gridCount < 3) ? 0 : 1;

    if (columnIndex == 1 && m_listCtrl.GetHeaderCtrl()->GetItemCount() == 1) {
        m_listCtrl.InsertColumn(1, L"Event2", LVCFMT_LEFT, 900);
    }

    const int maxRowsPerColumn = 10;
    int numBlocks = (quotes.size() + maxRowsPerColumn - 1) / maxRowsPerColumn;

    CString header = PadRight(L"Symbol", maxSymbol) + L" | " +
        PadRight(L"Last", maxLast) + L" | " +
        PadRight(L"Vol", maxVolume) + L" | " +
        PadRight(L"Change", maxChange);

    if (columnIndex == 0) {
        int titleIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), L"Commodity Fut Quotes");
        int headerIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), header);

        for (int i = 0; i < maxRowsPerColumn; ++i)
        {
            CString rowText;

            for (int block = 0; block < numBlocks; ++block)
            {
                int quoteIndex = block * maxRowsPerColumn + i;
                if (quoteIndex >= quotes.size())
                    break;

                Quote& q = quotes[quoteIndex];

                CString colText;
                colText = PadRight(q.symbol, maxSymbol) + L" | " +
                    PadRight(q.last, maxLast) + L" | " +
                    PadRight(q.volume, maxVolume) + L" | " +
                    PadRight(q.change, maxChange);

                if (!rowText.IsEmpty())
                    rowText += L"   ";

                rowText += colText;
            }

            m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), rowText);
        }
    }
    else {
        int startRow = FindNextAvailableRow();
        int rowsNeeded = maxRowsPerColumn + 2; 

        int currentRowCount = m_listCtrl.GetItemCount();
        while (currentRowCount < startRow + rowsNeeded) {
            m_listCtrl.InsertItem(currentRowCount++, L"");
        }

        m_listCtrl.SetItemText(startRow, 1, L"Commodity Fut Quotes");
        m_listCtrl.SetItemText(startRow + 1, 1, header);

        for (int i = 0; i < maxRowsPerColumn; ++i)
        {
            CString rowText;

            for (int block = 0; block < numBlocks; ++block)
            {
                int quoteIndex = block * maxRowsPerColumn + i;
                if (quoteIndex >= quotes.size())
                    break;

                Quote& q = quotes[quoteIndex];

                CString colText;
                colText = PadRight(q.symbol, maxSymbol) + L" | " +
                    PadRight(q.last, maxLast) + L" | " +
                    PadRight(q.volume, maxVolume) + L" | " +
                    PadRight(q.change, maxChange);

                if (!rowText.IsEmpty())
                    rowText += L"   ";

                rowText += colText;
            }

            m_listCtrl.SetItemText(startRow + 2 + i, 1, rowText);
        }
    }

    m_gridCount++;
}

void CSMITerminalDlg::ShowForex()
{
    if (m_gridCount >= 6) {
        AfxMessageBox(L"Maximum of 6 grids allowed.");
        return;
    }

    m_listCtrl.SetTextColor(RGB(0, 255, 0));

    const CString fxSymbols = L"EURUSD,USDJPY,GBPUSD,USDCHF,USDCAD,AUDUSD,NZDUSD,"
        L"EURGBP,EURJPY,EURCHF,EURAUD,EURCAD,EURNZD,"
        L"GBPJPY,GBPCHF,GBPAUD,GBPCAD,GBPNZD,"
        L"AUDJPY,AUDNZD,AUDCAD,AUDCHF,"
        L"NZDJPY,NZDCHF,NZDCAD,"
        L"CADJPY,CADCHF,CHFJPY,"
        L"EURSGD,USDHKD";
    CStringW jsonText = FetchQuotes(fxSymbols);

    if (jsonText.IsEmpty()) {
        AfxMessageBox(L"Failed to fetch FX quotes.");
        return;
    }

    json j;
    try {
        CT2A utf8(jsonText, CP_UTF8);
        std::string jsonStr(utf8);
        j = json::parse(jsonStr);
    }
    catch (const std::exception& e) {
        CStringW err(L"JSON parse error: ");
        err += CA2W(e.what());
        AfxMessageBox(err);
        return;
    }

    struct Quote
    {
        CString symbol, last, volume, change;
    };
    std::vector<Quote> quotes;

    for (auto& item : j)
    {
        Quote q;
        q.symbol = item.value("name", "").c_str();

        if (!item["price"].is_null())
            q.last.Format(L"%.5f", item["price"].get<double>());
        else
            q.last = L"-";

        if (!item["volume"].is_null())
            q.volume.Format(L"%lld", item["volume"].get<long long>());
        else
            q.volume = L"-";

        if (!item["change"].is_null())
            q.change.Format(L"%.5f", item["change"].get<double>());
        else
            q.change = L"-";

        quotes.push_back(q);
    }

    int maxSymbol = 0, maxLast = 0, maxVolume = 0, maxChange = 0;
    for (auto& q : quotes) {
        maxSymbol = max(maxSymbol, q.symbol.GetLength());
        maxLast = max(maxLast, q.last.GetLength());
        maxVolume = max(maxVolume, q.volume.GetLength());
        maxChange = max(maxChange, q.change.GetLength());
    }

    auto PadRight = [](const CString& s, int width) -> CString
        {
            CString padded = s;
            while (padded.GetLength() < width)
                padded += L" ";
            return padded;
        };

    int columnIndex = (m_gridCount < 3) ? 0 : 1;

    if (columnIndex == 1 && m_listCtrl.GetHeaderCtrl()->GetItemCount() == 1) {
        m_listCtrl.InsertColumn(1, L"Event2", LVCFMT_LEFT, 900);
    }

    const int maxRowsPerColumn = 10;
    int numBlocks = (quotes.size() + maxRowsPerColumn - 1) / maxRowsPerColumn;

    CString header = PadRight(L"Symbol", maxSymbol) + L" | " +
        PadRight(L"Last", maxLast) + L" | " +
        PadRight(L"Vol", maxVolume) + L" | " +
        PadRight(L"Change", maxChange);

    if (columnIndex == 0) {
        // Column 0: Insert at bottom
        int titleIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), L"FX Quotes");
        int headerIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), header);

        for (int i = 0; i < maxRowsPerColumn; ++i)
        {
            CString rowText;
            for (int block = 0; block < numBlocks; ++block)
            {
                int quoteIndex = block * maxRowsPerColumn + i;
                if (quoteIndex >= quotes.size())
                    break;

                Quote& q = quotes[quoteIndex];

                CString colText;
                colText = PadRight(q.symbol, maxSymbol) + L" | " +
                    PadRight(q.last, maxLast) + L" | " +
                    PadRight(q.volume, maxVolume) + L" | " +
                    PadRight(q.change, maxChange);

                if (!rowText.IsEmpty())
                    rowText += L"   ";

                rowText += colText;
            }

            m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), rowText);
        }
    }
    else {
        // Column 1: Find next available slot from top
        int startRow = FindNextAvailableRow();
        int rowsNeeded = maxRowsPerColumn + 2; // +2 for title and header

        // Ensure we have enough rows
        int currentRowCount = m_listCtrl.GetItemCount();
        while (currentRowCount < startRow + rowsNeeded) {
            m_listCtrl.InsertItem(currentRowCount++, L"");
        }

        // Set title and header
        m_listCtrl.SetItemText(startRow, 1, L"FX Quotes");
        m_listCtrl.SetItemText(startRow + 1, 1, header);

        // Set data rows
        for (int i = 0; i < maxRowsPerColumn; ++i)
        {
            CString rowText;
            for (int block = 0; block < numBlocks; ++block)
            {
                int quoteIndex = block * maxRowsPerColumn + i;
                if (quoteIndex >= quotes.size())
                    break;

                Quote& q = quotes[quoteIndex];

                CString colText;
                colText = PadRight(q.symbol, maxSymbol) + L" | " +
                    PadRight(q.last, maxLast) + L" | " +
                    PadRight(q.volume, maxVolume) + L" | " +
                    PadRight(q.change, maxChange);

                if (!rowText.IsEmpty())
                    rowText += L"   ";

                rowText += colText;
            }

            m_listCtrl.SetItemText(startRow + 2 + i, 1, rowText);
        }
    }

    m_gridCount++;
}

void CSMITerminalDlg::ShowIndex()
{
    if (m_gridCount >= 6) {
        AfxMessageBox(L"Maximum of 6 grids allowed.");
        return;
    }

   
    m_listCtrl.SetTextColor(RGB(0, 255, 0));

    const CString indexSymbols = L"^SPX,^GSPC,^DJI,^IXIC,^NDX,^RUT,^FTSE,^GDAXI,^N225,^FCHI,"
        L"^STOXX50E,^HSI,000001.SS,^BSESN,^NSEI,^AXJO,^GSPTSE,^VIX,^VXN,^RVX";
    CStringW jsonText = FetchQuotes(indexSymbols);

    if (jsonText.IsEmpty()) {
        AfxMessageBox(L"Failed to fetch Index quotes.");
        return;
    }

    json j;
    try {
        CT2A utf8(jsonText, CP_UTF8);
        std::string jsonStr(utf8);
        j = json::parse(jsonStr);
    }
    catch (const std::exception& e) {
        CStringW err(L"JSON parse error: ");
        err += CA2W(e.what());
        AfxMessageBox(err);
        return;
    }

    struct Quote
    {
        CString symbol, last, volume, change;
    };
    std::vector<Quote> quotes;

    for (auto& item : j)
    {
        Quote q;
        q.symbol = item.value("name", "").c_str();

        if (!item["price"].is_null())
            q.last.Format(L"%.5f", item["price"].get<double>());
        else
            q.last = L"-";

        if (!item["volume"].is_null())
            q.volume.Format(L"%lld", item["volume"].get<long long>());
        else
            q.volume = L"-";

        if (!item["change"].is_null())
            q.change.Format(L"%.5f", item["change"].get<double>());
        else
            q.change = L"-";

        quotes.push_back(q);
    }

    int maxSymbol = 0, maxLast = 0, maxVolume = 0, maxChange = 0;
    for (auto& q : quotes) {
        maxSymbol = max(maxSymbol, q.symbol.GetLength());
        maxLast = max(maxLast, q.last.GetLength());
        maxVolume = max(maxVolume, q.volume.GetLength());
        maxChange = max(maxChange, q.change.GetLength());
    }

    auto PadRight = [](const CString& s, int width) -> CString
        {
            CString padded = s;
            while (padded.GetLength() < width)
                padded += L" ";
            return padded;
        };

    int columnIndex = (m_gridCount < 3) ? 0 : 1;

    if (columnIndex == 1 && m_listCtrl.GetHeaderCtrl()->GetItemCount() == 1) {
        m_listCtrl.InsertColumn(1, L"Event2", LVCFMT_LEFT, 900);
    }

    const int maxRowsPerColumn = 10;
    int numBlocks = (quotes.size() + maxRowsPerColumn - 1) / maxRowsPerColumn;
    CString header = PadRight(L"Symbol", maxSymbol) + L" | " +
        PadRight(L"Last", maxLast) + L" | " +
        PadRight(L"Vol", maxVolume) + L" | " +
        PadRight(L"Change", maxChange);

    if (columnIndex == 0) {
        int titleIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), L"Global Index Quotes");
        int headerIdx = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), header);

        for (int i = 0; i < maxRowsPerColumn; ++i)
        {
            CString rowText;
            for (int block = 0; block < numBlocks; ++block)
            {
                int quoteIndex = block * maxRowsPerColumn + i;
                if (quoteIndex >= quotes.size())
                    break;

                Quote& q = quotes[quoteIndex];

                CString colText;
                colText = PadRight(q.symbol, maxSymbol) + L" | " +
                    PadRight(q.last, maxLast) + L" | " +
                    PadRight(q.volume, maxVolume) + L" | " +
                    PadRight(q.change, maxChange);

                if (!rowText.IsEmpty())
                    rowText += L"   ";

                rowText += colText;
            }

            m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), rowText);
        }
    }
    else {
        int startRow = FindNextAvailableRow();
        int rowsNeeded = maxRowsPerColumn + 2; // +2 for title and header

        int currentRowCount = m_listCtrl.GetItemCount();
        while (currentRowCount < startRow + rowsNeeded) {
            m_listCtrl.InsertItem(currentRowCount++, L"");
        }

        m_listCtrl.SetItemText(startRow, 1, L"Global Index Quotes");
        m_listCtrl.SetItemText(startRow + 1, 1, header);

        for (int i = 0; i < maxRowsPerColumn; ++i)
        {
            CString rowText;
            for (int block = 0; block < numBlocks; ++block)
            {
                int quoteIndex = block * maxRowsPerColumn + i;
                if (quoteIndex >= quotes.size())
                    break;

                Quote& q = quotes[quoteIndex];

                CString colText;
                colText = PadRight(q.symbol, maxSymbol) + L" | " +
                    PadRight(q.last, maxLast) + L" | " +
                    PadRight(q.volume, maxVolume) + L" | " +
                    PadRight(q.change, maxChange);

                if (!rowText.IsEmpty())
                    rowText += L"   ";

                rowText += colText;
            }

            m_listCtrl.SetItemText(startRow + 2 + i, 1, rowText);
        }
    }

    m_gridCount++;
}

int CSMITerminalDlg::FindNextAvailableRow()
{
    int rowCount = m_listCtrl.GetItemCount();
    for (int i = 0; i < rowCount; ++i) {
        CString col1Text = m_listCtrl.GetItemText(i, 1);
        if (col1Text.IsEmpty()) {
            return i;
        }
    }
    return rowCount; 
}

void CSMITerminalDlg::OnNMCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
    *pResult = CDRF_DODEFAULT;

    switch (pLVCD->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        // Request notifications for items + subitems + postpaint
        *pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
        break;

    case CDDS_ITEMPREPAINT:
        // Request subitem notifications for each cell
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
        break;

    case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
    {
        int rowNumber = static_cast<int>(pLVCD->nmcd.dwItemSpec);
        int subItem = pLVCD->iSubItem;

        CString col0Text = m_listCtrl.GetItemText(rowNumber, 0);
        CString col1Text = m_listCtrl.GetItemText(rowNumber, 1);

        if (subItem == 0) // Column 0 rules
        {
            if (col0Text.Find(L"Commodity") != -1 ||
                col0Text.Find(L"Overview") != -1 ||
                col0Text.Find(L"Quotes") != -1)
            {
                pLVCD->clrTextBk = RGB(0, 255, 255);
                pLVCD->clrText = RGB(0, 0, 0);
            }
            else if (col0Text.Find(L"Symbol") != -1 ||
                col0Text.Find(L"MacroEconomic") != -1)
            {
                pLVCD->clrTextBk = RGB(255, 255, 0);
                pLVCD->clrText = RGB(0, 0, 0);
            }
            else if (col0Text.Find(L"FX") != -1)
            {
                pLVCD->clrTextBk = RGB(255, 0, 255);
                pLVCD->clrText = RGB(255, 255, 255);
            }
            else
            {
                pLVCD->clrTextBk = RGB(40, 40, 40);
                pLVCD->clrText = RGB(255, 255, 255);
            }
        }
        else if (subItem == 1) 
        {
            if (col1Text.IsEmpty())
            {
                pLVCD->clrTextBk = RGB(0, 0, 255);     
                pLVCD->clrText = RGB(255, 255, 255);  
            }
        }

        *pResult = CDRF_DODEFAULT;
        break;
    }

    case CDDS_POSTPAINT:
    {
        CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);

        CRect rcClient;
        m_listCtrl.GetClientRect(&rcClient);

        CPen pen(PS_SOLID, 3, RGB(0, 0, 255)); 
        CPen* pOldPen = pDC->SelectObject(&pen);

        CHeaderCtrl* pHeader = m_listCtrl.GetHeaderCtrl();
        if (pHeader)
        {
            int nCols = pHeader->GetItemCount();
            int x = 0;
            for (int col = 0; col < nCols; ++col)
            {
                CRect rcCol;
                pHeader->GetItemRect(col, &rcCol);
                x += rcCol.Width();

                pDC->MoveTo(x - 1, rcClient.top);
                pDC->LineTo(x - 1, rcClient.bottom);
            }
        }

        pDC->SelectObject(pOldPen);
        break;
    }

    default:
        break;
    }
}


void CSMITerminalDlg::ShowNews()
{
	
}

void CSMITerminalDlg::ShowRate()
{
	
}



void CSMITerminalDlg::ShowIPO()
{
	
}

void CSMITerminalDlg::ShowEarnings()
{

}

void CSMITerminalDlg::ShowEqMetric()
{

}

void CSMITerminalDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
