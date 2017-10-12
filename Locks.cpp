#include "Locks.h"
#include <sys/time.h>

threading::CMutex::CMutex()
{
	pthread_mutex_init(&m_mutex, NULL);
}

threading::CMutex::~CMutex()
{
	pthread_mutex_destroy(&m_mutex);
}

void threading::CMutex::acquire()
{
	pthread_mutex_lock(&m_mutex);
}

void threading::CMutex::release()
{
	pthread_mutex_unlock(&m_mutex);
}

threading::CCondition::CCondition()
{
	pthread_cond_init(&m_condition, NULL);
}

threading::CCondition::~CCondition()
{
	pthread_cond_destroy(&m_condition);
}

void threading::CCondition::wait()
{
	pthread_cond_wait(&m_condition, &m_mutex);
}

void threading::CCondition::timedWait(long msec)
{
    struct timespec timeToWait;
    struct timeval now;

    gettimeofday(&now,NULL);

    timeToWait.tv_sec = now.tv_sec;
    timeToWait.tv_nsec = (now.tv_usec + msec * 1000UL) * 1000UL;

    pthread_cond_timedwait(&m_condition, &m_mutex, &timeToWait);
}

void threading::CCondition::notifyAll()
{
	pthread_cond_broadcast(&m_condition);
}

threading::CScopedMutex::CScopedMutex(CMutex &mutex) : m_mutex(mutex)
{
	m_mutex.acquire();
}

threading::CScopedMutex::~CScopedMutex()
{
	m_mutex.release();
}

threading::CScopedCondition::CScopedCondition(CCondition &condition) : m_condition(condition)
{
	m_condition.acquire();
}

threading::CScopedCondition::~CScopedCondition()
{
	m_condition.release();
}

void threading::CScopedCondition::wait()
{
	m_condition.wait();
}

void threading::CScopedCondition::notifyAll()
{
	m_condition.notifyAll();
}
