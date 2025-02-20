#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <stdio.h>

#include <FreeRTOS.h>
#include <semphr.h>

#define MAX_INPUT_LENGTH 100

/**
 * 콘솔 입력 테스크 핸들
 */
TaskHandle_t xConsoleTaskHandle;

/**
 * 콘솔 출력 보호를 위한 뮤텍스
 */
SemaphoreHandle_t xConsoleMutex;

/**
 * 콘솔 입력 보호를 위한 뮤텍스
 */
SemaphoreHandle_t xInputMutex;

/**
 * 콘솔 프롬프트 출력
 */
void console_prompt(void);

/**
 * 콘솔 초기화
 */
void console_init(void);

/**
 * 콘솔 출력
 */
void console_printf( const char * fmt, ... );


#endif