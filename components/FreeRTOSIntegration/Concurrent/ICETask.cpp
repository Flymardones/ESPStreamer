#include "ICETask.hpp"

static void taskWrapper(void* task) {
	ICETask* currentTask = (ICETask*) task;
    currentTask->running = true;
	currentTask->runTask(currentTask->getParameters());

    //If we make it here, we stop the task, if we did not, FreeRTOS would crash.
    currentTask->stop();
}

//This is virtual so it will never get called. GCC requires it to exists in the cpp file for some reason.
ICETask::~ICETask() {}


void ICETask::setup(const char* pTaskName, uint16_t iStackSize, uint32_t iPriority) {
    this->pTaskName = pTaskName;
    this->iStackSize = iStackSize;
    this->iPriority = iPriority;
}

void ICETask::setup(void* pParameters, const char* pTaskName, uint16_t iStackSize, uint32_t iPriority) {
    this->pParameters = pParameters;
    this->pTaskName = pTaskName;
    this->iStackSize = iStackSize;
    this->iPriority = iPriority;
}

void ICETask::setup(void* pParameters, const char* pTaskName, uint16_t iStackSize, uint32_t iPriority, uint8_t core) {
    this->pTaskName = pTaskName;
    this->iStackSize = iStackSize;
    this->iPriority = iPriority;
    this->core = core;
}

bool ICETask::start() {
    BaseType_t returnValue = pdFALSE;
    if (core == 0 || core == 1) {
        returnValue = xTaskCreatePinnedToCore(taskWrapper, pTaskName, iStackSize, (void*) this, iPriority, NULL, core);
    } else {
        returnValue = xTaskCreate(taskWrapper, pTaskName, iStackSize, (void*) this, iPriority, &taskHandle);
    }

    if (returnValue != pdTRUE) {
        return false;
    }

    return true;
}

void ICETask::stop() {
    if (running) {
        vTaskDelete(taskHandle);
    }
}

void ICETask::suspend() {
    vTaskSuspend(taskHandle);
}

void* ICETask::getParameters() {
	return pParameters;
}

TaskHandle_t ICETask::getTaskHandle()
{
    return taskHandle;
}

void ICETask::setPriority(int priority) {
	vTaskPrioritySet(this->taskHandle, priority);
}

int ICETask::getPriority() {
	return static_cast<int>(uxTaskPriorityGet(this->taskHandle));
}


void setCurrentPriority(int priority) {
	vTaskPrioritySet(NULL, priority);
}

int getCurrentPriority() {
	return static_cast<int>(uxTaskPriorityGet(NULL));
}