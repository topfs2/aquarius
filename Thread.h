#pragma once

#include <pthread.h>
#ifdef __ANDROID__
#include <jni.h>
#endif

class CThread
{
public:
	virtual ~CThread() { }

	void start();
	void join();

    void interrupt();

#ifdef __ANDROID__
    static JavaVM *vm;
#endif

protected:
	static void *staticRun(void *arg);
	virtual void run() = 0;

    bool m_running;

private:
	pthread_t m_thread;
};
