#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <console.h>
#include <command.h>
#include <error.h>

static const char hash_prompt[]  = "\033[1;30m # ";

static const char backspace_char = '\b';
static const char space_char     = ' ';
static const char cr_char        = '\r';
static const char nl_char        = '\n';

static const char * user_prompt  = "hogyun";

void console_prompt(void)
{
    static const char col_start[]    = "\033[1;32m";
    static const char col_end[]      = "\033[0m";

    console_printf(col_start);
    console_printf(user_prompt);
    console_printf(hash_prompt);
    console_printf(col_end);

    fflush(stdout);
}

void console_printf( const char * fmt, ... )
{
    va_list vargs;

    va_start( vargs, fmt );

    xSemaphoreTake( xConsoleMutex, portMAX_DELAY );

    vprintf( fmt, vargs );
    
    xSemaphoreGive( xConsoleMutex );

    va_end( vargs );

}

void vTaskInput(void *pvParameters) {
    char input[MAX_INPUT_LENGTH];
    for (;;) {
        if (xSemaphoreTake(xInputMutex, portMAX_DELAY) == pdTRUE) {
            char *input_char = NULL;
            input_char = fgets(input, MAX_INPUT_LENGTH, stdin);
            
            if ( input_char != NULL) {
                input_char[strcspn(input_char, "\n")] = '\0';
                
                xSemaphoreGive(xInputMutex);
                execute_command(input_char);
            }
            xSemaphoreGive(xInputMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}



void console_init(void)
{
    xConsoleMutex = xSemaphoreCreateMutex();
    xInputMutex = xSemaphoreCreateMutex();

    console_prompt();
    

    if (xTaskCreate(vTaskInput, "Input Task", configMINIMAL_STACK_SIZE, NULL, 1, &xConsoleTaskHandle) != pdPASS) {
        console_printf("Error: Failed to create vTaskInput\n");
    }

}
