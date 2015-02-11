//! @defgroup engine_group Engine

////////////////////////////////////////////////////////////////////////////////

#ifndef __IEngine_h_included__
#define __IEngine_h_included__

////////////////////////////////////////////////////////////////////////////////

#ifdef ENGINE_IMPL

	#define ENGINE_API __declspec( dllexport )

#else

	#define ENGINE_API __declspec( dllimport )

#endif // #ifdef ENGINE_IMPL

////////////////////////////////////////////////////////////////////////////////

// forward declaration
struct IResManager;

////////////////////////////////////////////////////////////////////////////////

#define DECL_COMMAND( CommandName )							\
															\
class Class##CommandName									\
{															\
public:														\
															\
	Class##CommandName()									\
	{														\
	if ( nNumCommands < nMaxCommands)						\
		{													\
		TCommand tCommand( #CommandName, Evaluate );		\
		g_pCommands[ nNumCommands ] = tCommand;				\
		++nNumCommands;										\
		}													\
	}														\
															\
	static void Evaluate( TStringStack & pArgs );			\
															\
} CommandName##instance;									\
	void Class##CommandName::Evaluate( TStringStack & pArgs )


typedef	CStack <CStr> TStringStack;

struct TBind
{
	dword	dwCode;
	CStr	sCommand;
};

struct TCommand
{
	inline TCommand()
	{
	}

	inline TCommand( const char * szName, void (*pCmd)( TStringStack & pArgs ) ) :
	sName	( szName ),
		Evaluate( pCmd )
	{
	}

	CStr sName;
	void (*Evaluate)( TStringStack & pArgs );
};


/*
================================================================================
// Name:
// Desc:
================================================================================
*/

enum EUIEvent
{
	UIE_UNKNOWN = INVALID_INDEX,
	UIE_INIT,
	UIE_SET_POS,
	UIE_KEYDOWN,
	//UIE_SET_POS,
};

enum EProcResult
{
	EPR_REMOVE_EVENT = 0,
	EPR_NOT_HANDLED,
	EPR_OK,
};

struct IUIEventHandler
{
	virtual EProcResult ProcEvent( EUIEvent eEvent, struct IBaseCtrl * pControl, void * pData ) = 0;
};

enum EControlClass
{
	ECC_WINDOW,
	ECC_BUTTON,
	ECC_EDIT,
	ECC_LISTVIEW
};


struct IBaseCtrl
{
	virtual void			SetPos		( const ivec2 & vPos )				= 0;
	virtual const ivec2	&	GetPos		() const							= 0;
	virtual void			SetSize		( const ivec2 & vSize )				= 0;
	virtual const ivec2 &	GetSize		() const							= 0;
	virtual void			Show		( bool bShow = true )				= 0;

	virtual void			SetText		( const char * szText )				= 0;
	//virtual void			AddEventHandler( IUIEventHandler * pHandler )	= 0;
};


/*
================================================================================
// Name:
// Desc:
================================================================================
*/
struct IImageManager
{
	virtual EResult	LoadImage( const char *szFileName, TImage & pOutImage ) = 0;
};

/*
================================================================================
// Name:
// Desc:
================================================================================
*/

struct ICamera
{
	virtual void Move		( const vec3 & vOffset ) = 0;
	virtual void Update		() = 0;
	virtual quat * GetQuat()	= 0;
	virtual vec3 * GetAngles() = 0;
	virtual const vec3 & GetPos() = 0;

	virtual const mat4 & GetViewMatrix() const = 0;
	virtual const mat4 & GetProjMatrix() const = 0;
	virtual const mat4 & GetViewProjMatrix() const = 0;

	virtual bool Unproject( vec3 * pOutRay, const ivec2 & vScreen ) = 0;
};


class CSceneObject;


/**
* Material declarations
*/
struct IMaterial
{
	virtual	void			Release			() = 0;
	virtual bool			Assign			() = 0;

	virtual void			BlendFunc		( EBlendFactor eSrc, EBlendFactor eDst ) = 0;

	virtual void			EnableAlphaBlend( bool bEnable = true ) = 0;
	virtual void			EnableAlphaTest	( bool bEnable = true ) = 0;
	virtual void			EnableDepthTest	( bool bEnable = true ) = 0;
	virtual void			EnableDepthWrite( bool bEnable = true ) = 0;

	virtual PTexture		GetTexture		( ETexture eTex ) = 0;
	virtual bool			SetTexture		( ETexture eTex, PTexture pTex ) = 0;

	virtual IShader *		GetShader		() = 0;
	virtual bool			SetShader		( IShader * pShader ) = 0;

	virtual void			AddSubMaterial	( IMaterial * pMaterial ) = 0;
	virtual IMaterial *		GetSubMaterial	( uint nSub ) = 0;
	virtual uint			GetSubCount		() const = 0;

	//! Установка нового занчения регистра(ов)

	//! Материал хранит кусок памяти который содержит копию регистров шейдера.
	//! Этот кусок памяти (все регистры) будет передан шейдеру перед отрисовкой в методе IMaterial::Assign
	//! Данный метод позволяет вносить изменения в вышеупомянутый кусок памяти.
	//! Метод SetRegisters только запоминает значения. Реальное выставление значений
	//! регистров происходит в методе IMaterial::Assign
	//! @param nUniform Индекс(ID) регистра
	//! @param pValues Массив значений
	//! @param nCount Количество элементов в массиве
	virtual void			SetRegisters	( uint nUniform, const vec4 * pValues, uint nCount ) = 0;
};


//! Переменная шейдера

//! На самом деле хранит в себе ссылку на шейдер и материал.
//! Логика использования следующая: создаем шейдер и материал. Затем создаем
//! переменную (CUniform) и привязываем (AttachToShader) её к шейдеру.
//! После этого переменную можно соеденять (Connect) с любым материалом
//! который использует тот же шейдер, что был указан при привязке...
//! Например:
/** @verbatim
	IShader * pUIShader = g_pRenderer->GetShader( UI_SHADER );
	CUniform uColorMod( "vColorMod", pUIShader );

	m_pRedWindowMaterial->SetShader( pUIShader );
	m_pWhiteWindowMaterial->SetShader( pUIShader );

	uColorMod.Connect( m_pRedWindowMaterial );
	uColorMod.SetValue( &vec4( 1, 0, 0, 1 ), 1 );

	uColorMod.Connect( m_pWhiteWindowMaterial );
	uColorMod.SetValue( &vec4( 1, 1, 1, 1 ), 1 );
@endverbatim */
//! @sa IMaterial IShader
//! @ingroup engine_group
class CUniform
{
	public:

	CUniform() :
		m_nID		( INVALID_INDEX ),
		m_pMaterial	( NULL )
	{
	}

	//! Создание экземпляра переменной с привязкой к шейдеру
	//! @sa CUniform::AttachToShader
	CUniform( const char * szName, IShader * pShader )
	{
		AttachToShader( szName, pShader );
	}

	//! Привязка переменной к шейдеру
	//! @param szName Имя переменной в коде шейдера
	//! @param pShader Указатель на объект шейдера
	//! @todo Нужно проверять корректность стыковки к шейдеру
	void AttachToShader( const char * szName, IShader * pShader )
	{
		m_pMaterial	= NULL;
		m_pShader	= NULL;

		if ( pShader )
		{
			m_pShader	= pShader;
			m_nID		= m_pShader->GetUniformID( szName );
			return;
		}

		m_nID = INVALID_INDEX;
	}

	void Connect( IMaterial * pMaterial )
	{
		m_pMaterial = pMaterial;

		if ( pMaterial )
		{
			if ( m_pShader != pMaterial->GetShader() )
			{
				DEBUG_ASSERT( !"invalid material!" );
				m_pMaterial = NULL;
				return;
			}
		}
	}

	//! Установка занчения переменной (регистра)

	//! Копирование указанных значений в регистры материала
	//! @param pValues Массив значений
	//! @param nCount Количество элементов в массиве
	//! @sa IMaterial::SetRegisters
	//! @todo Нужно установить минимум (1) для nCount
	void SetValue( const vec4 * pValues, uint nCount )
	{
		if ( m_pMaterial )
		{
			if ( INVALID_INDEX != m_nID )
			{
				m_pMaterial->SetRegisters( m_nID, pValues, nCount );
				return;
			}

			DEBUG_ASSERT( INVALID_INDEX != m_nID );
		}

		DEBUG_ASSERT( !"no material connected" );
	}

	private:

		IMaterial *	m_pMaterial;
		IShader *	m_pShader;
		uint		m_nID;
};

////////////////////////////////////////////////////////////////////////////////
/*
class CColorMod
{
public :

	CColorMod( IMaterial * pMaterial )
	{
		Attach( pMaterial );
	}

	bool Attach( IMaterial * pMaterial )
	{
		if ( NULL == pMaterial )
			return false;

		IShader * pShader = pMaterial->GetShader();

		if ( pShader )
		{
			m_nUniformID = pShader->GetConstantIndex( SA_COLOR_MOD );

			if ( INVALID_INDEX != m_nUniformID )
			{
				m_pMaterial = pMaterial;
				return true;
			}
		}

		m_pMaterial	 = NULL;
		m_nUniformID = INVALID_INDEX;
		return false;
	}

	void SetColorMod( const vec4 & vColorMod )
	{
		DEBUG_ASSERT( NULL != m_pMaterial );

		if ( m_pMaterial )
		{
			IShader * pShader = m_pMaterial->GetShader();
			m_pMaterial->SetUniform( m_nUniformID, vColorMod );
		}
	}

private:

	int			m_nUniformID;
	IMaterial*	m_pMaterial;
}*/


/*
*
*/
struct IMultiMaterial : IMaterial
{
	virtual IMaterial *	AddMaterial	()			= 0;
	virtual uint		GetCount	() const	= 0;
	virtual IMaterial * GetMaterial	()			= 0;
};


/*
*
*/
struct IShape
{
	enum EType
	{
		ST_INVALID = 0,
		ST_BONE,
		ST_PARTICLES,
		ST_MESH,
		ST_SKIN
	};

	virtual bool		LoadFromStream	( IFile * pFile )			= 0;
	virtual void		Render			( IMaterial * pMaterial )	= 0;
	virtual void		Release			()							= 0;
};


/*
*
*
struct IScene
{
	//virtual CSceneObject * LoadSceneObj( const char * szFileName ) = 0;
	virtual ICamera *	GetCamera() = 0;
	//virtual bool		QueueObject( class CSceneObject * pObject ) = 0;
	//virtual void		Render() = 0;

	virtual const mat4 & GetViewProjMatrix() const = 0;
	virtual void Update() = 0;
};*/


/**
* Закголовок ANIM файла
*/
struct TAnimFileHdr
{
	uint	nSign;				/// Подпись "ANIM"
	uint	nHeaderSize;		/// Размер заголовка
	uint	nVersion;			/// Версия файла
	uint	nNumObjects;		/// Кол-во анимированных объектов
	float	fAnimTime;			/// Время анимации ( секунды )
};


/**
* Максимальное количество костей
* влияющих на одну вершину
*/
#define MAX_BONES_PER_VERTEX 2


/**
* Информация о весе skinned вершины
*/
struct TSkinWeight
{
	uint	pBones[ MAX_BONES_PER_VERTEX ];
	float	pWeights[ MAX_BONES_PER_VERTEX ];
};


/**
* Закголовок Joint ( кость ) объекта
*/
struct TJointHdr
{
	enum EObjectConst
	{
		nMaxNameLen = 32
	};

	char szName[ nMaxNameLen ];	/// Имя кости
	uint nScaleKeyCount;		/// Кол-во scale ключей
	uint nRotKeyCount;			/// Кол-во rotation ключей
	uint nTransKeyCount;		/// Кол-во transform ключей
};


template <class TValue> struct CKey
{
	CKey()
	{
	}

	CKey( uint	fTimeVal, const TValue & val )
	{
		fTime = fTimeVal;
		value = val;
	}

	uint	fTime;
	TValue	value;
};

typedef CKey<vec3> CScaleKey;
typedef CKey<quat> CRotKey;
typedef CKey<vec3> CPosKey;


/**
* Закголовок MDL файла
*/
struct TModelFileHdr
{
	uint		nSign;				/// Подпись "MDL "
	uint		nHeaderSize;		/// Размер заголовка
	uint		nVersion;			/// Версия файла
	uint		nNumObjects;		/// Кол-во объектов
};


/**
*
*/
struct TObjectHdr
{
	enum EObjectConst
	{
		nMaxNameLen = 32
	};

	char		szName[ nMaxNameLen ];		// имя объекта
	char		szParent[ nMaxNameLen ];	// имя родителя

	vec3		vScale;
	quat		qRot;
	vec3		vPos;

	IShape::EType	eType;						// shape type
	uint			nShapeBlockSize;			// shape block size ( for skip ability )
};


/**
*
*/
struct TMeshData
{
	uint GetVertexCount() const
	{
		return tHeader.nVertexCount;
	}

	struct THeader
	{
		uint nHeaderSize;		/// Размер заголовка
		uint nVertexCount;		/// Количество вершин в mesh'е
		uint nVertexSize;		/// Размер одной вершиниы в байтах ( stride )
		uint nComponentCount;	/// Количество компонентов в вершине
		uint nIndexCount;		/// Количество индексов в mesh'е
		uint nSurfCount;		/// Количество материалов в mesh'е
	};

	THeader				tHeader;		/// Заголовок mesh'а
	TVertexComponent *	pComponents;	/// Массив компоненотов вершины
	void *				pVertices;		/// Массив вершин
	TSurface *			pSurfaces;		/// Массив материалов ( surface'ов )
};


/**
*
*/
class CObject
{
//	friend class CModel;

	//==========================================================================
	// Tip: Public methods & fields
	//==========================================================================
	public:

	CObject();
	~CObject();

	void			SetName					( const char * szName );
	const char *	GetName					();
	void			SetParent				( CObject * pParent );
	void			SetParentName			( const char * szName );
	CObject *		GetParent				();

//	bool			SetShape				( IShape * pShape );
//	IShape *		GetShape				();
//	bool			SetMaterial				( IMaterial * pMaterial );
//	IMaterial *		GetMaterial				();
	void			Render					();

	void			SetRot					( quat & qRot );
	void			SetRotAnim				( quat & qRot );
	void			SetPos					( vec3 & vPos );
	void			SetScale				( vec3 & vScale );

	bool			IsMatrixRebuildNeeded	( float fAnimTime );
	void			RebuildMatrix			( float fAnimTime );
	const mat4 &	GetMatrix				();

	void			Load( const char * szFile );

	//==========================================================================
	// Tip: Private methods & fields
	//==========================================================================
	//private:

	CStr			m_sName;
	CStr			m_sParentName;
	CObject *		m_pParent;

	vec3			m_vScale;		///
	quat			m_qRot;			///
	quat			m_qRotAnim;		///
	vec3			m_vPos;			///
	mat4			m_mMatrix;		///
	bool			m_bDirtyMatrix;	///

	mat4			m_mInvMatrix;	///

	float			m_fLastUpdateTime;
};


/**
*
*/
struct IModel
{
	virtual		~IModel				()						{};

	virtual void RebuildHierarchy	()						= 0;
	virtual void Render				( const mat4 & mWorld ) = 0;
	virtual void Load				( const char * szFile )	= 0;

	virtual const bbox & GetBBox	() = 0;
};


/*
*
*/
struct IMesh : IShape
{
	virtual void			Release			() = 0;

	virtual bool			SetVertexBuffer	( PVertexBuffer pVB ) = 0;
	virtual void			SetVertexDecl	( IVertexDecl * pDecl ) = 0;

	virtual size_t			AddSurface		( const char * szName, IIndexBuffer * pIB ) = 0;
	virtual size_t			GetSurfCount	() const = 0;
	virtual const char *	GetSurfName		( uint nSurf ) = 0;

	virtual void			Render			( IMaterial * pMaterial ) = 0;
	virtual void			RenderSurface	( uint nSurf, IMaterial *	pMaterial ) = 0;
};

typedef CSafePointer< IMesh > PMesh;


/*
*
*/
struct IParticleSystem
{
	virtual ~IParticleSystem() {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render( IMaterial * pMaterial ) = 0;
};


struct IEntity
{
	virtual void Release()						= 0;

	virtual void SetDir( const float fDir )		= 0;
	virtual void SetPos( const vec3 & vPos )	= 0;
	virtual const vec3 & GetPos() const	= 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual IModel *		GetModel	()		= 0;
	virtual const mat4 &	GetMatrix	() const= 0;
};


struct IWorldIter
{
	virtual bool		AddEntity( IEntity * pEnt ) = 0;

	// Установка итератора в начало поиска
	virtual bool		Reset	() = 0;

	virtual IEntity	*	Next	() = 0;
};


struct IEntityManager
{
	virtual void		SetIterator		( IWorldIter * pIterator )	= 0;
	virtual void		Update			()							= 0;
	virtual void		Render			()							= 0;
	virtual IEntity *	CreateEntity	( const char * szName )		= 0;
};

struct IUIManager;


//! Ландшафт
struct ILandscape
{
	virtual ~ILandscape() {};

	//! Загрузка карты высот для всего ландшафта
	virtual void Load( const char * szFileName ) = 0;

	//! Отрисовка видимой части ландшафта по "пирамиде" камеры
	virtual void Render( ICamera * pCam ) = 0;

	//! Отладочная функция TODO: её нужно выпилить
	virtual void SetMaterial( IMaterial * p ) = 0;

	//!
	virtual void Trace( ICamera * pCam, const ivec2 vMouse ) = 0;
};


/**
*
*/
struct IEngine : public IBaseIntf
{
	virtual EResult			Init		( HINSTANCE hInst ) = 0;

	virtual void			Update		() = 0;

	virtual float GetTime() const = 0;

	//virtual IBaseCtrl *		CreateCtrl( EControlClass eClass, IBaseCtrl * pParent = NULL ) = 0;
	virtual IUIManager *	GetUIManager() = 0;
	virtual IResManager *	GetResManager() = 0;
	virtual IEntityManager *GetEntityManager() = 0;
	virtual IMaterial *		GetDefMaterial	() = 0;

	//virtual IMesh *		LoadMesh( const char * szFile ) = 0;

	virtual IMesh *			CreateMesh		() = 0;
	virtual IMaterial *		CreateMaterial	() = 0;
	virtual IModel *		CreateModel		() = 0;
	virtual ILandscape *	CreateLandscape	() = 0;

	virtual ICamera *		CreateCamera	() = 0;
	virtual void			SetCamera		( ICamera * pCamera ) = 0;
	virtual const ICamera *	GetCamera		() = 0;
//	virtual IScene *		GetScene() = 0;

	virtual bool			LoadTexture		( PTexture & pOutTexture, const char *szFileName ) = 0;
	virtual IMaterial *		LoadMaterial( const char *szFileName ) = 0;
	virtual IParticleSystem * CreateParticleSys() = 0;

	virtual IEasyWin *		GetWindow	()										= 0;

	virtual	void	RegisterCommand	( const TCommand & tCmd )				= 0;
	virtual	void	Command			( const char * szCommandLine )			= 0;

	virtual void		Sprite2D( IMaterial * pMat, const ivec2 & vPos, const ivec2 & vSize, const vec2 * pUVPos = 0, const vec2 * pUVSize = 0 ) = 0;
};

#endif // #ifndef __IEngine_h_included__

////////////////////////////////////////////////////////////////////////////////