#include "common.h"
#include "tty.h"
 

 static int TIMEOUT = 1000;
 
TTY::TTY()
	: m_Handle(0)
{
}


TTY::~TTY()
{
	Disconnect();
}


bool TTY::IsOK() const
{
	return (0 != m_Handle);
} 


bool TTY::Connect(const char * szPort, int baudrate)
{
	Disconnect();
 
	m_Handle = CreateFile(
		szPort, 
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		NULL);
 
	
	if (((HANDLE)-1) == m_Handle)
	{
		m_Handle = 0;
		return false;
	}

	SetCommMask(m_Handle, EV_RXCHAR);
	SetupComm(m_Handle, 1500, 1500);

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = TIMEOUT;
 
	if (FALSE == SetCommTimeouts(m_Handle, &CommTimeOuts))
	{
		m_Handle = 0;
		return false;
	}
 
	DCB ComDCM;
 
	memset(&ComDCM,0,sizeof(ComDCM));
	ComDCM.DCBlength = sizeof(DCB);
	GetCommState(m_Handle, &ComDCM);
	ComDCM.BaudRate = DWORD(baudrate);
	ComDCM.ByteSize = 8;
	ComDCM.Parity = NOPARITY;
	ComDCM.StopBits = ONESTOPBIT;
	ComDCM.fAbortOnError = TRUE;
	ComDCM.fDtrControl = DTR_CONTROL_DISABLE;
	ComDCM.fRtsControl = RTS_CONTROL_DISABLE;
	ComDCM.fBinary = TRUE;
	ComDCM.fParity = FALSE;
	ComDCM.fInX = FALSE;
	ComDCM.fOutX = FALSE;
	ComDCM.XonChar = 0;
	ComDCM.XoffChar = (unsigned char)0xFF;
	ComDCM.fErrorChar = FALSE;
	ComDCM.fNull = FALSE;
	ComDCM.fOutxCtsFlow = FALSE;
	ComDCM.fOutxDsrFlow = FALSE;
	ComDCM.XonLim = 128;
	ComDCM.XoffLim = 128;
 
	if (FALSE == SetCommState(m_Handle, &ComDCM))
	{
		CloseHandle(m_Handle);
		m_Handle = 0;
		return false;
	}
 
	return true;
}
 
 void TTY::Disconnect() {
 
        if(m_Handle != 0) {
                CloseHandle(m_Handle);
         m_Handle = 0;
        }
 
 }
 
 void TTY::Write(const vector<unsigned char>& data) {
 
        if(m_Handle == 0) {
                throw TTYException();
        }
 
        DWORD feedback;
        if(!WriteFile(m_Handle, &data[0], (DWORD)data.size(), &feedback, 0) || feedback != (DWORD)data.size()) {
                CloseHandle(m_Handle);
                m_Handle = 0;
                throw TTYException();
        }
 
        // In some cases it's worth uncommenting
        //FlushFileBuffers(m_Handle);
 
 }
 

 int TTY::Read(void * pBuffer, unsigned int maxBufferSize)
 {
	 if (0 == m_Handle)
		 return -1;

	DWORD byteCount = 0;
	
	if (TRUE == ReadFile(m_Handle, pBuffer, maxBufferSize, &byteCount, NULL))
	{
		return byteCount;
	}

	return -1;
	 /*
        if(m_Handle == 0) {
                throw TTYException();
        }
 
        DWORD begin = GetTickCount();
        DWORD feedback = 0;
 
        unsigned char* buf = &data[0];
        DWORD len = (DWORD)data.size();
 
        int attempts = 3;
        while(len && (attempts || (GetTickCount()-begin) < (DWORD)TIMEOUT/3)) {
 
                if(attempts) attempts--;
 
                if(!ReadFile(m_Handle, buf, len, &feedback, NULL)) {
                        CloseHandle(m_Handle);
                        m_Handle = 0;
                        throw TTYException();
                }
 
                assert(feedback <= len);
                len -= feedback;
                buf += feedback;
 
        }
 
        if(len) {
                CloseHandle(m_Handle);
                m_Handle = 0;
                throw TTYException();
        }
 */
 }