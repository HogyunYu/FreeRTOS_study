
/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/timeb.h>

/*FreeRTOS kernal includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <event_groups.h>

/*local includes. */
#include "console.h"
#include "error.h"
#include "command.h"


void execute_command(const char *command)
{
    error_t error;
    if (strcmp(command, "help") == 0)
    {
        error = help_command();
        console_prompt();
    }
    else if (strcmp(command, "time") == 0)
    {
        error = time_command();
        console_prompt();
    }
    else if (strcmp(command, "task") == 0)
    {
        error = task_command();
        console_prompt();
    }
    else if (strcmp(command, "event") == 0)
    {
        error = event_command();
        console_prompt();
    }
    else if (strcmp(command, "echo") == 0)
    {
        error = echo_command();
        console_prompt();
    }
    else if (strcmp(command, "start_view") == 0)
    {
        error = start_view_command();
    }
    else
    {
        console_printf("ERROR: 등록된 커맨드가 아닙니다.\n");
        console_prompt();
    }
    /*
    if (error != OK)
    {
        console_printf("ERROR CODE: %d\n", error);
        console_prompt();
    }
    */
}

error_t help_command(void){
    console_printf("help : 현재 등록된 커멘드와, 커맨드에 대한 설명을 출력\r\n");
    console_printf("time : 현재 시간을 출력\r\n");
    console_printf("task : 현재 등록된 task 중류\r\n");
    console_printf("event : 이벤트 비트 설정\r\n");
    console_printf("echo : 입력값을 콘솔창에 출력\r\n");
    console_printf("start_view : 5초 동안 현재 시간과 수행되고 있는 task 출력, 입력 제한\r\n");
    return OK;
}

error_t time_command(void){
    struct timeb milli_now;
    struct tm* now;
    float sec;

    ftime(&milli_now);
    now = localtime(&milli_now.time);
    sec = now->tm_sec + milli_now.millitm / 1000.;

    console_printf("현재 시간은 %04d/%02d/%02d %02d:%02d:%.4f 입니다.\r\n", 
        1900 + now->tm_year, now->tm_mon + 1, now->tm_mday, 
        now->tm_hour, now->tm_min, sec);
    return OK;
}

error_t task_command(void){
    TaskStatus_t *pxTaskStatArr;
    volatile UBaseType_t uxArrSz;
    
    eTaskState TaskStatIdx;
    UBaseType_t uxPrior;
    unsigned long ulPeriod;


    char *states[5] = {"Running","Ready","Blocked","Suspended","Deleted"};

    //*pcWriteBuffer = 0x00;
    uxArrSz = uxTaskGetNumberOfTasks();
    // console_printf("current task : %d\r\n",uxArrSz);
    pxTaskStatArr = pvPortMalloc(uxArrSz*sizeof(TaskStatus_t));
    uxArrSz = uxTaskGetSystemState(pxTaskStatArr, uxArrSz, NULL);
    console_printf("%12s    |  %6s  |%9s |%9s   |\r\n","NAME","PERIOD","PRIORITY","STATE");
    for (int i=0; i<uxArrSz; i++){
        TaskStatIdx = pxTaskStatArr[i].eCurrentState;
        if (TaskStatIdx<3){
            ulPeriod = pxTaskStatArr[i].ulRunTimeCounter;
            uxPrior = pxTaskStatArr[i].uxBasePriority;
            console_printf("%14s  |%6lu    |%6d    |%10s  |\r\n",
                            pxTaskStatArr[i].pcTaskName, 
                            ulPeriod, uxPrior,
                            states[TaskStatIdx]);
        }
    }

    // taskPrior = uxTaskGetSystemState(TaskStatArr, 10, )

    // eTaskState TaskState;
    // char *states[5] = {"Running","Ready","Blocked","Suspended","Deleted"};
    // TaskState = eTaskGetState(xEvTask3Handle);
    // console_printf("current state : %s\r\n",states[TaskState]);
    // console_printf("test\r\n");

    return OK;
}


TaskHandle_t xEchoTaskHandle;

error_t echo_command(void){
    error_t EchoErrFlg;
    error_t *ErrEcho;
    xTaskCreate(echo_task,"echo",256,ErrEcho,3,&xEchoTaskHandle);
    return *ErrEcho;
}

void echo_task(void *pvParameters){
    char echo[MAX_INPUT_LENGTH];
    char *StringToSend;
    char *StringToReceive;

    error_t *ErrEcho;
    ErrEcho = (error_t *)pvParameters;

    if (xSemaphoreTake(xInputMutex, portMAX_DELAY)==pdTRUE){
        char* echo_char = NULL;
        console_printf("\r콘솔에 출력할 메세지를 입력하시오 : ");
        taskENTER_CRITICAL();
        echo_char = fgets(echo, MAX_INPUT_LENGTH, stdin);
        taskEXIT_CRITICAL();
        fflush(stdin);
    
        if (echo_char!=NULL){
            echo_char[strcspn(echo_char, "\n")] = '\0';
            StringToSend = echo_char;
            xSemaphoreGive(xInputMutex);
            if (xQueueSend(xEchoQueue,&echo_char,portMAX_DELAY)==pdPASS){
                if (xQueueReceive(xEchoQueue,&StringToReceive,portMAX_DELAY)==pdPASS){
                    console_printf("입력한 메세지 : %s\r\n",StringToReceive);
                    *ErrEcho = OK;
                }
                else *ErrEcho = ERROR_COMMAND_ECHO;
            }
            else *ErrEcho = ERROR_COMMAND_ECHO;
        }
        else *ErrEcho = ERROR_COMMAND_ECHO;
        xSemaphoreGive(xInputMutex);
    }
    else *ErrEcho = ERROR_COMMAND_ECHO;
    vTaskDelay(pdMS_TO_TICKS(100));
    vTaskDelete(xEchoTaskHandle);
}


error_t event_command(void){
    TickType_t getCurrentTick;
    error_t *EventErr;
    getCurrentTick = xTaskGetTickCount();
    xTaskKillTimer = xTimerCreate("EventTaskKill", pdMS_TO_TICKS(10000), pdFALSE,0,prvEventKillCallback);
    xTaskCreate(event_create_task,"event",configMINIMAL_STACK_SIZE,EventErr,2,&xEventHandle);
    return *EventErr;
    // return OK;
}

static void prvEventKillCallback(TimerHandle_t xTimer){
    // console_printf("Timer Callback activated\r\n");
    vTaskDelete(xEvTask1Handle);
    vTaskDelete(xEvTask2Handle);
    vTaskDelete(xEvTask3Handle);
    vTaskDelete(xEvTask4Handle);
    xTimerDelete(xTaskKillTimer,0);
}


#define BIT_0 (1<<0)
#define BIT_1 (1<<1)
#define BIT_2 (1<<2)
#define BIT_3 (1<<3)


void event_create_task(void *pvParameters){    
    char BitInput[MAX_INPUT_LENGTH];
    int EventBit = 0, IS_BIT = 1, err_flg = 0;
    BaseType_t xTaskKillTimerStart;

    error_t *EventErr;
    EventErr = (error_t *)pvParameters;

    if (xSemaphoreTake(xInputMutex, portMAX_DELAY)==pdTRUE){
        char* BitInput_char = NULL;
        console_printf("\r설정할 이벤트 비트를 4 bit 이진수로 입력하세요 : ");
        taskENTER_CRITICAL();
        BitInput_char = fgets(BitInput, MAX_INPUT_LENGTH, stdin);
        taskEXIT_CRITICAL();
        fflush(stdin);
        // xSemaphoreGive(xInputMutex);

        BitInput_char[strcspn(BitInput_char, "\n")] = '\0';
        if (strlen(BitInput_char)==4){
            for (int i=0; i<4; i++){
                if (BitInput_char[i]=='0') EventBit += 0<<i;
                else if (BitInput_char[i]=='1') EventBit += 1<<i;
                else IS_BIT *= 0;
            }
            if (IS_BIT){
                EventBits_t uxBits = xEventGroupSetBits(xEventGroup, EventBit);
                *EventErr = OK;
                xTaskKillTimerStart = xTimerStart(xTaskKillTimer,0);
        
                xTaskCreate(event_task1,"task1",configMINIMAL_STACK_SIZE,NULL,1,&xEvTask1Handle);
                xTaskCreate(event_task2,"task2",configMINIMAL_STACK_SIZE,NULL,1,&xEvTask2Handle);
                xTaskCreate(event_task3,"task3",configMINIMAL_STACK_SIZE,NULL,1,&xEvTask3Handle);
                xTaskCreate(event_task4,"task4",configMINIMAL_STACK_SIZE,NULL,1,&xEvTask4Handle);
            }
            else err_flg = 1;
        }
        else err_flg = 1;
        xSemaphoreGive(xInputMutex);
    }
    if (err_flg){
        console_printf("ERROR: 4 bit 이진수를 입력하세요!\r\n");
        *EventErr = ERROR_COMMAND_EVENT;
    }
    xSemaphoreGive(xInputMutex);
    vTaskDelay(pdMS_TO_TICKS(100));
    vTaskDelete(xEventHandle);
}

void event_task1(void *pvParameters){
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits(xEventGroup, BIT_0, pdTRUE, pdTRUE, portMAX_DELAY);
    console_printf("Event Task 1: 이벤트 1이 설정되었습니다.\r\n");
    for (;;){

        // console_printf("Task1 alive\r\n");
        // vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void event_task2(void *pvParameters){
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits(xEventGroup, BIT_1, pdTRUE, pdTRUE, portMAX_DELAY);
    console_printf("Event Task 2: 이벤트 2가 설정되었습니다.\r\n");
    for (;;){

    }
}

void event_task3(void *pvParameters){
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits(xEventGroup, BIT_2, pdTRUE, pdTRUE, portMAX_DELAY);
    console_printf("Event Task 3: 이벤트 3이 설정되었습니다.\r\n");
    for (;;){

    }
}

void event_task4(void *pvParameters){
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits(xEventGroup, BIT_3, pdTRUE, pdTRUE, portMAX_DELAY);
    console_printf("Event Task 4: 이벤트 4가 설정되었습니다.\r\n");
    for (;;){
    }
}


error_t start_view_command(void){
    TimerHandle_t xTaskViewTimer;
    error_t *StartViewErr;

    xTaskCreate(start_view_task,"Start View Task",configMINIMAL_STACK_SIZE,StartViewErr,2,NULL);
    // xTaskViewTimer = xTimerCreate("TaskView", pdMS_TO_TICKS(1000), pdTRUE,0,prvTaskViewCallback);
    return *StartViewErr;
}


static void start_view_task(void *pvParameters){
    struct timeb milli_now;
    struct tm* now;
    float sec;
    int timerCnt = 0;
    error_t *StartViewErr;
    StartViewErr = (error_t *)pvParameters;

    if (xSemaphoreTake(xInputMutex, portMAX_DELAY)==pdPASS){
        for(;;){
            if (timerCnt>=5){
                console_printf("출력이 종료되었습니다. 지금부터 입력이 허용됩니다.\r\n");
                *StartViewErr = OK;
                console_prompt();
                xSemaphoreGive(xInputMutex);
                vTaskDelete(NULL);
            }
            ftime(&milli_now);
            now = localtime(&milli_now.time);
            sec = now->tm_sec + milli_now.millitm / 1000.;

            console_printf("-%04d/%02d/%02d %02d:%02d:%.3f\r\n", 
                            1900 + now->tm_year, now->tm_mon + 1, now->tm_mday, 
                            now->tm_hour, now->tm_min, sec);
            
            task_command();
            console_printf("\r\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            timerCnt++;
        }
    }
    else *StartViewErr = ERROR_COMMAND_START_VIEW;
}
