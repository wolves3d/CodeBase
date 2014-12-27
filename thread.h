#ifndef __thread_h_included__
#define __thread_h_included__

#include "pthread.h"
#include "signal.h"

#ifndef _WINDOWS

#include "unistd.h"
#define Sleep usleep

#endif

// -----------------------------------------------------------------------------

// Forward declarations
class CThread;

// -----------------------------------------------------------------------------

class CMutex
{
public:
	CMutex()
		: m_isLocked(false)
	{
		Init();
	}

	~CMutex()
	{
		assert(false == m_isLocked);
		pthread_mutex_destroy(&m_mutex);
	}

	void Lock()
	{
		pthread_mutex_lock(&m_mutex);
		m_isLocked = true;
	}

	void Unlock()
	{
		pthread_mutex_unlock(&m_mutex);
		m_isLocked = false;
	}

private:

	CMutex(const CMutex & otherMutex)
	{
		// not allowed
		assert(false);
	}

	void Init()
	{
		if (0 != pthread_mutex_init(&m_mutex, NULL))
		{
			assert(false);
		}
	}

	pthread_mutex_t 	m_mutex;
	bool				m_isLocked;
};

// -----------------------------------------------------------------------------

// Forward declaration
class CThreadTask;

struct IThreadTaskDelegate
{
	virtual void OnTaskCompleted(CThreadTask * task) = 0;
};

class CThreadTask
{
	friend class CThread;
	CThread * _parent;
	int _tag;
	IThreadTaskDelegate * _delegate;

public:

	CThreadTask(IThreadTaskDelegate * delegate = NULL)
		: _delegate(delegate)
		, _parent(NULL)
		, _tag(0)
	{}

	CThread * GetParent() { return _parent; }
	void SetTag(int tag) { _tag = tag; }
	int GetTag() const { return _tag; }
	void SetDelegate(IThreadTaskDelegate * delegate) { _delegate = delegate; }
	IThreadTaskDelegate * GetDelegate() { return _delegate; }

	virtual void OnRun() = 0;
};

typedef queue <CThreadTask *> TaskQueue;

// -----------------------------------------------------------------------------

struct ThreadMsg
{
	enum ECode
	{
		THREAD_QUIT_MESSAGE = 0,
		TASK_COMPLETED,
		TASK_QUEUE_COMPLETED,
		USER_MSG
	} code;

	CThreadTask * task;
	void * arg;

	ThreadMsg(ECode _code, CThreadTask * _task, void * _arg)
		: code(_code)
		, task(_task)
		, arg(_arg)
	{}
};

typedef vector <ThreadMsg> MessageVector;

class CThread // FIXME ‡Á‰ÂÎËÚ¸ Ì‡ CThread Ë CPulseThread
{
public:

	bool volatile	m_activityFlag;
	pthread_t		m_thread;
	
	TaskQueue		m_taskQueue;
	CMutex			m_taskQueueMutex;

	MessageVector	m_messageQueue;
	CMutex			m_messageQueueMutex;

	pthread_cond_t m_HibernateCondition;

	static void * ThreadFunc(void * thisArg)
	{
		CThread * _this = (CThread *)thisArg;
		_this->m_activityFlag = true;
		CThreadTask * currentTask = NULL;

		/*
		//Log("Thread %p started\n", thisPointer->GetID());
		void * result = _this->Main();
		return result;
		*/

		while (true)
		{
			bool lastTaskFlag = false;

			// Get next task (critical section)
			_this->m_taskQueueMutex.Lock();
			{
				if (_this->m_taskQueue.size() > 0)
				{
					currentTask = _this->m_taskQueue.front();
					_this->m_taskQueue.pop();

					lastTaskFlag = (NULL == currentTask);
				}
			}
			_this->m_taskQueueMutex.Unlock();

			// Run task (if any)
			if (NULL != currentTask)
			{
				currentTask->_parent = _this;
				currentTask->OnRun();
				currentTask->_parent = NULL;

				_this->PostMessage(ThreadMsg::TASK_COMPLETED, currentTask);
				currentTask = NULL;
			}

			if (true == lastTaskFlag)
				_this->PostMessage(ThreadMsg::TASK_QUEUE_COMPLETED, NULL);

			// Check exit flag
			if (false == _this->m_activityFlag)
				break;

			// Wait for task (sleep) here
			_this->Idle(10); // FIXME!
		}

		_this->OnExit();
		return NULL;
	}

	void OnExit()
	{
		pthread_cond_destroy(&m_HibernateCondition);
	}

	virtual void * Main()
	{
		assert(false); // should override
		return NULL;
	}

protected:

	void Idle(uint milliseconds = 0)
	{
		Sleep(milliseconds);
	}

public:

	CThread()
		: m_activityFlag(false)
	{
	}

	bool Init()
	{
		if (0 == pthread_cond_init(&m_HibernateCondition, NULL))
		{
			if (0 == pthread_create(&m_thread, NULL, ThreadFunc, this))
			{
				return true;
			}
		}

		return false;
	}

	void Cleanup(bool waitForExit = true)
	{
		if (true == m_activityFlag)
		{
			m_activityFlag = false;
			PushTask(NULL);

			if (true == waitForExit)
			{
				pthread_join(m_thread, NULL);
			}
		}
	}

	void PushTask(CThreadTask * task)
	{
 		// [critical section] insert in task queue
		m_taskQueueMutex.Lock();
		m_taskQueue.push(task);
		m_taskQueueMutex.Unlock();
// 		awake thread if cleep

		pthread_cond_signal(&m_HibernateCondition);
	}

	/*
	void Kill()
	{
		//pthread_cancel(m_thread);
		pthread_kill(m_thread, SIGTERM);
	}
	*/

	static int GetID()
	{
		pthread_t result = pthread_self();

		#ifdef _WINDOWS
			return (int)(result.p);
		#else
			return (int)result;
		#endif
	}

	void PostMessage(ThreadMsg::ECode code, CThreadTask * task, void * arg = NULL)
	{
		m_messageQueueMutex.Lock();
		m_messageQueue.push_back( ThreadMsg(code, task, arg) );
		m_messageQueueMutex.Unlock();
	}

	bool GetMessageQueue(MessageVector * outVector)
	{
		m_messageQueueMutex.Lock();

		const uint queueSize	= m_messageQueue.size();
		const bool haveMessages	= (0 != queueSize);

		if (true == haveMessages)
		{
			for (uint i = 0; i < queueSize; ++i)
				outVector->push_back( m_messageQueue[i] );

			m_messageQueue.clear();
		}

		m_messageQueueMutex.Unlock();
		return haveMessages;
	}
};

#endif // #ifndef __thread_h_included__