#include "ICETimer.hpp"

TimerMap ICETimer::timerTasklist;

void ICETimer::timerCallback(TimerHandle_t timerHandle) 
{
		timerTasklist.at(timerHandle)->runTask((void*) 0);
}

void ICETimer::timerCallbackVoid(TimerHandle_t timerHandle) 
{
	// Ignore Callback
}

ICETimer::ICETimer(const char* pcTimerName, int iPeriodMS, bool bAutoReload, LPICETASK pTask) 
{
    TickType_t iTicks = iPeriodMS / portTICK_PERIOD_MS;

    UBaseType_t kFreeRTOSBool = pdFALSE;

    if (bAutoReload) {
        kFreeRTOSBool = pdTRUE;
    }
	if (pTask == NULL) {
    	TimerHandle_t ktimerHandle = xTimerCreate(pcTimerName, iTicks, kFreeRTOSBool, (void*) 0, timerCallbackVoid);
	    m_kTimerHandle = ktimerHandle;
	} else {
    	TimerHandle_t ktimerHandle = xTimerCreate(pcTimerName, iTicks, kFreeRTOSBool, (void*) 0, timerCallback);
	    m_kTimerHandle = ktimerHandle;
		{
			//Handle old c++ versions too. We might want to strip the big chunk out if we decide not to use STM32CUBE at all.
			#if __cplusplus > 201402L
			timerTasklist.insert_or_assign(m_kTimerHandle, pTask);
			#else
			auto it = timerTasklist.find(m_kTimerHandle);
			if (it != timerTasklist.end()) // Another timer was found with same handle?
			{
				printf("WARNING: Two timers with the same handle was created. Latest timer name: %s\n", pcTimerName);
				it->second = pTask;
			} else {
				timerTasklist.insert(std::make_pair(m_kTimerHandle, pTask));
			}
			#endif
		}
	}
}

ICETimer::~ICETimer() {
    stop();

    auto it = timerTasklist.find(m_kTimerHandle);
    timerTasklist.erase(it);
}



bool ICETimer::start() {
	if (m_bAutoReload) {
		vTimerSetReloadMode(m_kTimerHandle, pdTRUE);
	}

	if (xTimerStart(m_kTimerHandle, 0) != pdPASS) {
		return false;
	}

	return true;
}

bool ICETimer::start(int iDelayMS) {
	if (m_bAutoReload) {
		vTimerSetReloadMode(m_kTimerHandle, pdTRUE);
	}


	int ticks = iDelayMS / portTICK_PERIOD_MS;

	if (xTimerStart(m_kTimerHandle, ticks) != pdPASS) {
		return false;
	}

	return true;
}

bool ICETimer::stop() {
	//Also stop the autoreload - else timer will reload after stopping.
	vTimerSetReloadMode(m_kTimerHandle, pdFALSE);

	if (xTimerStop(m_kTimerHandle, 0) != pdPASS) {
		return false;
	}

	return true;
}

bool ICETimer::stop(int iDelayMS) {
	int ticks = iDelayMS / portTICK_PERIOD_MS;

	if (xTimerStop(m_kTimerHandle, ticks) != pdPASS) {
		return false;
	}

	return true;
}

bool ICETimer::reset() {
	if (xTimerReset(m_kTimerHandle, 0) != pdPASS) {
		return false;
	}
	return true;
}

bool ICETimer::isActive() {
	if (xTimerIsTimerActive(m_kTimerHandle) == pdTRUE) {
		return true;
	}
	return false;
}