////////////////////////////////////////////////////////////////////////////////

#ifndef __Console_h_included__
#define __Console_h_included__

////////////////////////////////////////////////////////////////////////////////

class CConsole : public CWindow
{
	public :

		CConsole();

		void Render()
		{
			CWindow::Render();
		}

	private :

		CEdit * m_pCmdLine;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __Console_h_included__

////////////////////////////////////////////////////////////////////////////////