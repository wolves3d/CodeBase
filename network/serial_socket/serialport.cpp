#include "pch.h"
#include "serialport.h"

#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */


CSerialPort::CSerialPort()
	: m_file(-1)
{

}


bool CSerialPort::IsValid()
{
	return (-1 != m_file);
}


bool CSerialPort::Open(const char * port)
{
	if (false == IsValid())
	{
//		m_file = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
		m_file = open(port, O_RDWR | O_NOCTTY);

		if (-1 != m_file)
		{
			fcntl(m_file, F_SETFL, O_NONBLOCK/*FNDELAY*/);

			// Get the current options for the port...
			struct termios options;
			tcgetattr(m_file, &options);

// 9600 baud 
 cfsetispeed(&options, B9600);
 cfsetospeed(&options, B9600);


    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    //  Enable the receiver and set local mode...
    options.c_cflag |= (CLOCAL | CREAD);

    // no flow control
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines

    options.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    options.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 20;

/*
options.c_cflag &= ~CRTSCTS;    
options.c_cflag |= (CLOCAL | CREAD);                   
options.c_iflag |= (IGNPAR | IGNCR);                  
options.c_iflag &= ~(IXON | IXOFF | IXANY);          
options.c_oflag &= ~OPOST;

options.c_cflag &= ~CSIZE;            
options.c_cflag |= CS8;              
options.c_cflag &= ~PARENB;         
options.c_iflag &= ~INPCK;         
options.c_iflag &= ~(ICRNL|IGNCR);
options.c_cflag &= ~CSTOPB;      
options.c_iflag |= INPCK;
options.c_cc[VTIME] = 0.001;  //  1s=10   0.1s=1 *
options.c_cc[VMIN] = 0;
*/
			// Set the new options for the port...
			tcsetattr(m_file, TCSANOW, &options);

			return true;
		}
	}

	return false;
}


void CSerialPort::Close()
{
	if (true == IsValid())
	{
		close(m_file);
		m_file = -1;
	}
}


int CSerialPort::Send(const void * data, uint byteCount)
{
	if (true == IsValid())
	{
		return write(m_file, data, byteCount);
	}

	return 0;
}


int CSerialPort::Recv(void * buffer, uint maxByteCount)
{
	if (true == IsValid())
	{
		int res = read(m_file, buffer, maxByteCount);

//		if (res > 0)
		{
			return res;
		}
	}

	return 0;
}
