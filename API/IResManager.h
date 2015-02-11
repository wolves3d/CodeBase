////////////////////////////////////////////////////////////////////////////////

#ifndef __IResManager_h_included__
#define __IResManager_h_included__


//!

//!
//!
enum EResPriority
{
	ERP_LOW,
	ERP_HIGH
};


//!

//!
//!
struct TResource
{
	enum EResState
	{
		INVALID,		//< Ресурс только что создан (пусто)
		QUEUED,			//< Ресурс находится в очереди на загрузку
		LOADING,		//< Ресурс в данный момент загружается
		FAILED,			//< Загрузка ресурса провалилась
		READY_TO_USE,	//< Ресурс можно использовать (загрузка продолжается)
		LOADED			//< Ресурс полностью загружен
	};

	TResource() :
		pCreator	( NULL	),
		pResource	( NULL	),
		pFile		( NULL	),
		pBuffer		( NULL	),
		nSize		( 0		),
		nReadySize	( 0		),
		eState		( INVALID )
	{
	}

	CStr					sFileName;	/// Имя ресурса (путь к файлу)
	struct IResCreator *	pCreator;	//< Указатель на загрузчик
	void *					pResource;
	void *					pFile;		//< Указатель на файл
	byte *					pBuffer;	//< Файл в памяти (буффер)
	size_t					nSize;		//< Размер всего буффера
	size_t					nReadySize;	//< Размер загруженной части буффера
	EResState				eState;		//< Состояние загрузки
};


struct IResCreatorInfo
{
	const char * szInfo;
	const char * szExtension;
};


//!

//!
//!
struct IResCreator
{
	//! Запрос информации о загрузчике
	virtual const IResCreatorInfo * GetInfo () const = 0;

	//! Создаёт экземпляр ресурса, присваевает ему ссылку на значение по умолчанию
	virtual void CreateResInstance( TResource ** ppOutResource ) = 0;

	//! Создаёт буффер для считывания файла
	
	//! После того как создан экземпляр ресурса его необходимо инициализировать.
	//! В результате выполнения этого метода будет создан буффер размером не более
	//! размера файла (возможно меньше, всё зависит от конкретного загрузчика)
	//! Из отдельного потока в созданный буффер будет записано pOutMaxSize байт
	//! после чего будет вызван метод UpdateRes
	//! \param[in] pRes Укзатель на экземпляр ресурса.
	//! \param[in] nFileSize Размер файла
	//! \param[out] pOutMaxSize Размер создаваемого буффера ( 0 <= pOutMaxSize <= nFileSize )
	//! \return Указатель на память, в которую необходимо считывать файл.
	//! \remarks Запись в память будет осуществляться из отдельного потока, по этому с ней
	//! запрещено проводить любые операции везде кроме метода UpdateRes
	//! \sa UpdateRes
	virtual size_t InitRes ( TResource * pRes, size_t nFileSize ) = 0;

	//! Вызывает оброботку считанных данных

	//! Вызывается от 1 до N раз, во время загрузки ресурса
	//! \param[in] pRes Укзатель на экземпляр ресурса.
	//! \param[in] nBytesWritten Количество байт считанных с момента прошлого
	//! вызова UpdateRes
	//! \param[out] pOutMaxSize Максимальное количество байт которое можно записать
	//! в возвращаемый указатель.
	//! \return Указатель на память, в которую необходимо далее считывать файл.
	//! \remarks Если функция вернула NULL, то это означает прерывание загрузки.
	//! Такое происходит в двух случаях - когда файл считан полностью, или когда
	//! в ходе загрузки возникли ошибки. И в том и другом случае менеджер ресурсов
	//! уберёт эту задачу из потока загрузки.
	//! \sa InitRes
	virtual size_t UpdateRes( TResource * pRes, size_t nBytesWritten ) = 0;

	virtual void FinalizeRes( TResource * pRes ) = 0;
};


//!

//!
//!
struct IResManager : public IBaseIntf
{
	//! Регистрация нового загрузчика ресурсов (удалять нельзя)
	virtual void RegCreator( IResCreator * pCreator )						= 0;

	//! Не выходит пока не загрузит ресурс
	virtual bool Request( const char * szFile, TResource ** ppOutResource )		= 0;

	//! Ставит ресурс в очередь на загрузку и сразу выходит
	virtual uint Demand( const char * szFile, TResource ** ppOutResource, EResPriority ePriority = ERP_LOW ) = 0;

	//! Узнаем состояние ресурса
	virtual TResource::EResState GetResState( uint nQueueID ) const = 0;

	//! Удаление ресурса
	//! Если загрузка ресурса провалилась, то нужно подчистить за ним память
	//! и удалить все ссылки на него. Если ссылку (индекс) сохранить, то весьма
	//! вероятны магические последствия, т.к. по ссылке в последствии
	//! будет жить кто-то другой (в лучшем случае ссылка будет не верной)
	virtual void RemoveRes( uint nQueueID ) = 0;

	//! Метод обновления. Нужно вызывать каждый кадр (для создания ресурсов в главном потоке)
	virtual void Update() = 0;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #define __IResManager_h_included__

////////////////////////////////////////////////////////////////////////////////