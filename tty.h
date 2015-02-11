#ifndef __tty_h_included__
#define __tty_h_included__

// За основу было взято
// http://ru.wikibooks.org/wiki/COM-%D0%BF%D0%BE%D1%80%D1%82_%D0%B2_Windows_(%D0%BF%D1%80%D0%BE%D0%B3%D1%80%D0%B0%D0%BC%D0%BC%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5)


struct ITTY
{
	virtual bool IsOK() const = 0;

	virtual bool Connect(const char * szPort, int baudrate) = 0;
	virtual void Disconnect() = 0;

	virtual void Write(const vector<unsigned char>& data) = 0;
	virtual int	Read(void * pBuffer, unsigned int maxBufferSize) = 0;
};

 
struct TTY // win_platform implementation
	: ITTY
{ 
    TTY();
    virtual ~TTY();
 
    virtual bool IsOK() const;
 
    virtual bool Connect(const char * szPort, int baudrate);
    virtual void Disconnect();
 
    virtual void Write(const vector<unsigned char>& data);
    virtual int	Read(void * pBuffer, unsigned int maxBufferSize);
 
    HANDLE m_Handle;
};
 

struct TTYException
{
};

#endif // #ifndef __tty_h_included__