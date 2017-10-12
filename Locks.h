#pragma once
#include <pthread.h>

namespace threading
{
	class CMutex
	{
	public:
		CMutex();
		virtual ~CMutex();

		void acquire();
		void release();

	protected:
		pthread_mutex_t m_mutex;
	};

	class CCondition : public CMutex
	{
	public:
		CCondition();
		virtual ~CCondition();

		void wait();
        void timedWait(long msec);
		void notifyAll();

	protected:
		pthread_cond_t m_condition;
	};

	class CScopedMutex
	{
	public:
		CScopedMutex(CMutex &mutex);
		~CScopedMutex();

	private:
		CMutex m_mutex;
	};

	class CScopedCondition
	{
	public:
		CScopedCondition(CCondition &condition);
		~CScopedCondition();

		void wait();
		void notifyAll();

	private:
		CCondition m_condition;
	};
};
