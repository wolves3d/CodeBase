////////////////////////////////////////////////////////////////////////////////

#ifndef __IInput_h_included__
#define __IInput_h_included__

////////////////////////////////////////////////////////////////////////////////

enum EInputMsgType
{
	IMT_MOUSE_MOVE_X,
	IMT_MOUSE_MOVE_Y,
	IMT_MOUSE_LKEY,
	IMT_MOUSE_MKEY,
	IMT_KEY_DOWN,
	IMT_KEY_UP
};

////////////////////////////////////////////////////////////////////////////////

struct TInputMsg
{
	EInputMsgType	eType;
	dword			dwData;
};

////////////////////////////////////////////////////////////////////////////////

struct IInputHandler
{
	virtual bool ProcInputEvent( const TInputMsg & tData )					= 0;
};


/**
* Программный интерфейс для взаимодействия с подсистемой ввода
*/
struct IInput : public IBaseIntf
{
	virtual EResult			Init		( HINSTANCE hInst, HWND hWindow )	= 0;

	/**
	* Регистрация обработчика событий ввода
	* Замечание: новый обработчик ставиться в конец очереди
	*/
	virtual EResult			RegHandler	( IInputHandler * pHandler )		= 0;

	/**
	* Удаление зарегистрированного обработчика
	*/
	virtual void			DelHandler	( IInputHandler * pHandler )		= 0;

	/**
	* Обработка зарегистрированных обработчиков
	*/
	virtual void			ProcHandlers()									= 0;

	/**
	* Если есть сообщение, то возвращает истину.
	* Замечание: сначала обрабатываются события мышки, затем клавиатуры.
	*/
	virtual bool			GetInputMsg	( TInputMsg & tData )				= 0;
	virtual const char *	GetKeyboardState()								= 0;

	virtual char			CodeToChar	( dword dwDik ) const				= 0;
	virtual const char *	CodeToName	( dword dwDik ) const				= 0;
	virtual dword			NameToCode	( const char * szName ) const		= 0;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __IInput_h_included__

////////////////////////////////////////////////////////////////////////////////