#include "Thread.h"

#ifdef __ANDROID__
JavaVM *CThread::vm = NULL;
#endif

void CThread::start()
{
	pthread_create(&m_thread, NULL, staticRun, this);
}

void CThread::join()
{
	pthread_join(m_thread, NULL);
}

void CThread::interrupt()
{
    m_running = false;
}

void *CThread::staticRun(void *arg)
{
	if (arg == NULL)
		return NULL;

	CThread *thread = (CThread *)arg;

#ifdef __ANDROID__
    JNIEnv *env = NULL;
    vm->AttachCurrentThread(&env, NULL);

    if (env)
#endif
    {
        thread->m_running = true;
    	thread->run();
    }

#ifdef __ANDROID__
    vm->DetachCurrentThread();
#endif
}
