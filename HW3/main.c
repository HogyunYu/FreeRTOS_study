
/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

/*FreeRTOS kernal includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <semphr.h>
#include <timers.h>

/*local includes. */
#include "console.h"
#include "error.h"
#include "command.h"

// Sumulation of the CPU hardware sleeping mode
// Idle task hook, never erase
void vApplicationIdleHook( void ){
	usleep(15000);
}


#define ECHO_QUEUE_LENGTH 15

int main( void )
{
    xEventGroup = xEventGroupCreate();
    xEchoQueue = xQueueCreate(ECHO_QUEUE_LENGTH, sizeof(char*));
    console_init();

    vTaskStartScheduler();

    for (;;);
    return 0;
}

