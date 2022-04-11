
#ifndef _CJMUTEX_H_
#define _CJMUTEX_H_


/*
	RECURSIVE MUTEX

	pthread_mutex_t Mutex;
	pthread_mutexattr_t Attr;

	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&Mutex, &Attr);


    One of the gurus behind pthreads, David Butenhof, 
	has an amusing recursive mutex rant at zaval.org/resources/library/butenhof1.html . 
	So yeah, recursive mutexes are usually an indication of a faulty design. 

    There are many correct uses for recursive mutexes, 
	but yes I would strongly advise a beginner against them...

    I think there're not so many correct uses for recursive mutexes that 
	cannot be redesigned to avoid them.       
 
   The main class of important uses for recursive mutexes 
   that I know of is when you have operations on a shared resource 
   that need to individually be atomic, but also want to allow 
   grouping several operations together as an atomic transaction. 
   The classic example is stdio with flockfile.	

*/

class CJCORE_API cjMutex
{
public:	
	cjMutex(cjBool recursive=cjFalse)  
	{ 
		if(recursive)
		{
			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&m_mutex, &attr);
		}
		else
		{
			pthread_mutex_init(&m_mutex, NULL); 
		}
	}

	~cjMutex() 
	{ 
		pthread_mutex_destroy(&m_mutex);	 
	}

	cjBool DoLock()
	{
		return pthread_mutex_lock(&m_mutex) == 0;
	}

	cjBool DoUnLock()
	{
		return pthread_mutex_unlock(&m_mutex) == 0;
	}

protected:
	pthread_mutex_t						m_mutex;
};


class CJCORE_API cjMutexYield
{
public:	
	cjMutexYield()
	{
		pthread_cond_init(&m_cond, NULL);
		pthread_mutex_init(&m_mutex, NULL);		
	}

	~cjMutexYield()
	{
		pthread_cond_destroy(&m_cond);
		pthread_mutex_destroy(&m_mutex);
	}

	inline cjBool DoLock()
	{
		return pthread_mutex_lock(&m_mutex) == 0;
	}

	inline cjBool DoYield()
	{
		return pthread_cond_wait(&m_cond, &m_mutex) == 0;
	}

	inline cjBool DoSignal()
	{
		return pthread_cond_signal(&m_cond) ? cjTrue : cjFalse;
	}

	inline cjBool DoUnLock()
	{
		return pthread_mutex_unlock(&m_mutex) == 0;
	}

protected:
	pthread_cond_t						m_cond;
	pthread_mutex_t						m_mutex;
};

#endif
