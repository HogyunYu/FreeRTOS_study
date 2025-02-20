

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
void vApplicationIdleHook( void )
{
    usleep( 15000 );
}

// Task 1 정의
void vTask1( void *pvParameters )
{
	const char *pcTaskName1 = "Task 1: 1sec period11\r\n";
	const char *pcTaskName2 = "Task 1: 1sec period22\r\n";
	const char *pcTaskName3 = "Task 1: 1sec period33\r\n";

	for( ;; )
	{
		console_print( pcTaskName1 );
		console_print( pcTaskName2 );
        	vTaskDelay( 1000 );
		console_print( pcTaskName3 );
	}
}
// Task 2 정의
void vTask2( void *pvParameters )
{
	const char *pcTaskName = "Task 2: 2sec period\r\n";

	for( ;; )
	{
		console_print( pcTaskName );
        	vTaskDelay( 2000 );
	}
}

int main( void )
{
    console_init(); 

	xTaskCreate( vTask1, "Task 1", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	xTaskCreate( vTask2, "Task 2", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    
	vTaskStartScheduler();
	for( ;; );
}
