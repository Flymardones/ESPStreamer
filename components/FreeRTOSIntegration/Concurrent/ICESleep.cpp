#include <ICESleep.hpp>

void ICESleep(int iMilliseconds) {
	int ticks = iMilliseconds / portTICK_PERIOD_MS;
	vTaskDelay(ticks);
}