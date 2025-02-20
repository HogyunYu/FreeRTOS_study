
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

// Sumulation of the CPU hardware sleeping mode
// Idle task hook, never erase
void vApplicationIdleHook( void ){
	usleep(15000);
}

#define SWTimer_Period pdTICKS_TO_MS(3000)
QueueHandle_t xQueue1, xQueue2; 
SemaphoreHandle_t xSem = NULL;

static void vTaskLED1(void *pvParameters){
	uint8_t FlgToSend = 0;
	TickType_t xLastWakeTime;
	for (;;){
		// xLastWakeTime = xTaskGetTickCount();
		FlgToSend = 1;
		xQueueSendToBack(xQueue1,&FlgToSend,0);
		console_print("LED 1 task - LED 1 Task done. LED 1 ON\r\n");
		// xTaskDelayUntil(xLastWakeTime, pdMS_TO_TICKS(1000));
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void vTaskHandler(void *pvParameters){
	uint8_t FlgToSend = 0;
	BaseType_t xHigherPriorityTaskWoken;
	BaseType_t xStatus;
	for(;;){
		xStatus = xSemaphoreTake(xSem,portMAX_DELAY);
		if (xStatus==pdPASS) {
			FlgToSend = 1;
			xQueueSendToBack(xQueue2,&FlgToSend,0);
		}
		else {
			FlgToSend = 0;
			xQueueSendToBack(xQueue2,&FlgToSend,0);
			console_print("Handler Task - Fail to take Semaphore. LED 2 OFF\r\n");
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
	}
}

static void LEDstatus(void *pvParameters){
	uint8_t FlgReceived1, FlgReceived2;
	UBaseType_t uxQueueLength1, uxQueueLength2;
	TickType_t xLastWakeTime;
	BaseType_t xStatus1, xStatus2;
	for (;;){
		// xLastWakeTime = xTaskGetTickCount();
		uxQueueLength1 = uxQueueMessagesWaiting(xQueue1);
		uxQueueLength2 = uxQueueMessagesWaiting(xQueue2);
		console_print("data in Queue1 : %u\r\n",uxQueueLength1);
		console_print("data in Queue2 : %u\r\n",uxQueueLength2);
		
		xStatus1 = xQueueReceive(xQueue1, &FlgReceived1, 0);
		xStatus2 = xQueueReceive(xQueue2, &FlgReceived2, 0);
		if (xStatus1==pdPASS){
			if (FlgReceived1==1) console_print("LED 1 State : ON\r\n");
			else console_print("LED 1 State : OFF\r\n");
		}
		if (xStatus2==pdPASS){
			if (FlgReceived2==1) console_print("LED 2 State : ON\r\n");
			else console_print("LED 2 State : OFF\r\n");
		}
		else console_print("LED 2 State : OFF\r\n");
		// xTaskDelayUntil(xLastWakeTime, pdMS_TO_TICKS(1000));
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void vTimerCallback(TimerHandle_t xTimer){
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xSem,&xHigherPriorityTaskWoken);
	console_print("Timer on\r\n");
}

int main(void){
	console_init();
	BaseType_t SWTimerStart;
	TimerHandle_t SWTimer;

	xQueue1 = xQueueCreate(5,sizeof(uint8_t));  
	xQueue2 = xQueueCreate(5,sizeof(uint8_t));               
	xSem = xSemaphoreCreateBinary();
	SWTimer = xTimerCreate("SWTimer",SWTimer_Period,pdTRUE,0,vTimerCallback);
	if (SWTimer!=NULL){
		SWTimerStart = xTimerStart(SWTimer,0);
		if (SWTimerStart==pdPASS){
			xTaskCreate(LEDstatus,"Status",configMINIMAL_STACK_SIZE,NULL,1,NULL);
			xTaskCreate(vTaskLED1,"Task_LED1",configMINIMAL_STACK_SIZE,NULL,1,NULL);
			xTaskCreate(vTaskHandler,"handler",configMINIMAL_STACK_SIZE,NULL,1,NULL);

			vTaskStartScheduler();
		}
	}
	for (;;);
}

