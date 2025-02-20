

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Local includes. */
#include "console.h" // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨

TaskHandle_t xTask2Handle = NULL;

void vApplicationIdleHook( void )
{
    usleep( 15000 );
}

// Task 1 정의
void vTask1( void *pvParameters )
{
	const char *pcTaskName = "zzzzzz\r\n";

	for( ;; )
	{
		console_print( pcTaskName );
        	vTaskDelay( 1000 );
	}
}
// Task 2 정의
void vTask2( void *pvParameters )
{
	const char *pcTaskName = "Alarm!\r\n";

	for( ;; )
	{
		console_print( pcTaskName );
        	vTaskResume(xTask2Handle);
        	vTaskDelay( 5000 );
	}
}
// Task 3 정의
void vTask3( void *pvParameters )
{
	const char *pcTaskName = "5min..\r\n";

	for( ;; )
	{
		console_print( pcTaskName );
        	vTaskSuspend(xTask2Handle);
	}
}

int main( void )
{
    console_init(); 

	xTaskCreate( vTask1, "Task 1", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	xTaskCreate( vTask2, "Task 2", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
	xTaskCreate( vTask3, "Task 3", configMINIMAL_STACK_SIZE, NULL, 2, &xTask2Handle );
    
	vTaskStartScheduler();
	for( ;; );
}
