
#include <FreeRTOS.h>
#include <queue.h>
#include <event_groups.h>

#include "console.h"
#include "error.h"

QueueHandle_t xEchoQueue;
EventGroupHandle_t xEventGroup;

TaskHandle_t xEventHandle;
TaskHandle_t xEvTask1Handle;
TaskHandle_t xEvTask2Handle;
TaskHandle_t xEvTask3Handle;
TaskHandle_t xEvTask4Handle;
TimerHandle_t xTaskKillTimer;

void execute_command(const char *command);
error_t help_command(void);
error_t time_command(void);
error_t task_command(void);

error_t echo_command(void);
void echo_task(void *pvParameters);

error_t start_view_command(void);
static void start_view_task(void *pvParameters);
static void prvTaskViewCallback(TimerHandle_t xTimer);

error_t event_command(void);
void event_create_task(void *pvParameters);
static void prvEventKillCallback(TimerHandle_t xTimer);

void event_task1(void *pvParameters);
void event_task2(void *pvParameters);
void event_task3(void *pvParameters);
void event_task4(void *pvParameters);
