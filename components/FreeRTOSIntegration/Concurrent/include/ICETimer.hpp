#pragma once

#include <map>
#include "ICETask.hpp"
#include "ICEAllocator.hpp"

/*
 * This wraps FreeRTOS timers to something that can be used in C++.
 * See the FreeRTOS documentation here: https://www.freertos.org/FreeRTOS-Software-Timer-API-Functions.html for more information.
 */ 

/*
 * This typedefs a TimerMap to be a map of FreeRTOS TimerHandle_t and our LPICETASK so they can be connected.
 * It also asks of the map to use our allocator so the heap space are allocated in FreeRTOS and managed instead of just floating around freely.
 */
typedef std::map<TimerHandle_t, LPICETASK, std::less<TimerHandle_t>, ICEAllocator<std::pair<const TimerHandle_t, LPICETASK>>> TimerMap;

/*
 * An ICETimer sets up a FreeRTOS timer to run a ICETask on a specific interval.
 * Important to note about FreeRTOS timers is that they usually have higher priority than the rest of the system.
 * Think of them as interrupts, the code they run need to be executed in a short amount of time and be concise.
*/
class ICETimer
 {
    private:
		/* This is a STATIC variable that handles the connection between FreeRTOS timers and c++ objects. It needs to be static. */
    static TimerMap timerTasklist;

    /* These aree local to each ICETimer */
    TimerHandle_t m_kTimerHandle;
    bool m_bAutoReload;


    public:
    /* 
     * This static function is the callback the FreeRTOS calls into. 
     * It will find the right ICETask that corresponds the timer handle and call the runTask method on that object.
     */
    static void timerCallback(TimerHandle_t timerHandle);

    /*
     * This static function is a dummy callback the FreeRTOS calls into.
     * It will find the right ICETask that corresponds the timer handle and do nothing.
     * Can be used as a simple timer with no callback action directly called necessarily
     */
    static void timerCallbackVoid(TimerHandle_t timerHandle);

    /* 
     * pcTimerName can be anything and is used solely for debugging FreeRTOS.
     * iPeriodMS defines how often the timer should run (in ms)
     * bAutoReload defines whether or not the timer should reload (and thereby keep running) or be a one-shot
     * pTask refers to a ICETask (or derived) object. The runTask of this object will be called on timer expiry.
     */
    ICETimer(const char* pcTimerName, int iPeriodMS, bool bAutoReload, LPICETASK pTask);
    ~ICETimer();
		
    /* Starts the timer */
    bool start();
    /* Waits iDelayMS milliseconds, then starts the timer */
	  bool start(int iDelayMS);

	  /* Stops the timer */
    bool stop();
    /* Waits iDelayMS milliseconds, then stops the timer */
	  bool stop(int iDelayMS);

    /* Resets the timer. See https://www.freertos.org/FreeRTOS-timers-xTimerReset.html */
    bool reset();

    /* Returns whether or not the timer is running. */
    bool isActive();
};
