/*
 * ICETask.hpp
 *
 *  Created on: 04 Apr 2021
 *      Author: KNE
 */

#pragma once

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

/* This taskWrapper will be called from each task. It will start the task and
 * if for some reason the task returns, this function will catch it and the task will stop.
 * It also passes parameters to runTask if any.
 * This is a necessary component in order to call into c++ objects from FreeRTOS.
 */
//static void taskWrapper(void* task);


class ICETask {
private:
    void* pParameters = nullptr;
    const char* pTaskName = nullptr;
    configSTACK_DEPTH_TYPE iStackSize = 2048;
    UBaseType_t iPriority = 0;
    int core = -1;


    TaskHandle_t taskHandle;

public:
    bool running = false;
    /*
     * Sets up the ICETask to run as a standalone task. This is needed for a functional task.
     * Tasks will likely start even when this is not set up, but their priority will be 0 and name null.
     * pTaskName is for FreeRTOS debugging. Gives task a meaningful name.
     * iStackSize is the dynamically allocated stack size of the timer, in bytes.
     * iPriority is directly tied to the freertos priorities.
     */
    void setup(const char* pTaskName, uint16_t iStackSize, uint32_t iPriority);

    /*
     * This extends the normal task setup and allows for parameters to the task.
     * pParameters is the parameters for the function. This can be cast to what it actually is from within runTask.
     */
	void setup(void* pParameters, const char* pTaskName, uint16_t iStackSize, uint32_t iPriority);

    void setup(void* pParameters, const char* pTaskName, uint16_t iStackSize, uint32_t iPriority, uint8_t core);
	
    virtual ~ICETask();

	/*
     * Creates (and starts) the task in FreeRTOS.
     */
    bool start();

    /*
     * Deletes the task from FreeRTOS thereby stopping it.
     */
	void stop();

    /*
     * Suspends the task in FreeRTOS.
     */
	void suspend();

    /*
     * This function allows the passing of arguments to runTask. This is used by 
     */
	void* getParameters();

    TaskHandle_t getTaskHandle();
    
    /*
     * This function sets the priority of the task to the given number. In FreeRTOS, a higher number means a higher priority.
     */
    void setPriority(int priority);
    
    /*
     * This returns the priority of the task.
     */
    int getPriority();

    /* 
     * runTask is only able to take arguments while running as a task. 
     * If it is supposed to run as a timer it will be called without any usable arguments.
     */
    virtual void runTask(void* arguments) = 0;
};

typedef  ICETask    *LPICETASK;


/*
 * This function will set the priority of the calling task.
 * This is used for cases where a function may be called from a number of different tasks, but would like to set the priority higher.
 */
void setCurrentPriority(int priority);

/*
 * This function will get the priority of the calling task.
 */
int getCurrentPriority();