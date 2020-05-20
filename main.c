/*
 * CPE-555 Real-Time and Embedded Systems
 * Stevens Institute of Technology
 * Spring 2020
 *
 * FreeRTOS Template
 *
 * Name: Ryan Bajollari
 */

/* Standard includes */
#include <stdio.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t xQueue;
TaskHandle_t xTask1;
TaskHandle_t xTask2;

typedef struct {
	int min;
	int sec;
}clock_t;

void vTimeTask (void *pvTime);

void vDisplayTask ();

/* main application code */
int main( void )
{
	/* set STDOUT to be non-buffered to that printf() messages display immediately */
	setbuf(stdout, NULL);

	printf("Application started\n");

	clock_t input;

	printf("Enter Minutes: \n");
	scanf("%i", &input.min);

	printf("Enter Seconds: \n");
	scanf("%i", &input.sec);

	xTaskCreate (vTimeTask, "Time Task", 1000, &input, 1, &xTask1);

	vTaskStartScheduler();

	for ( ;; );
}

void vTimeTask (void* pvTime) {
	clock_t tx_output = *(clock_t*) pvTime;

	xTaskCreate (vDisplayTask, "Display Task", 1000, NULL, 1 , &xTask2);

	xQueue = xQueueCreate(10, sizeof(clock_t));

	while(1) {
		if ( ! xQueueSend(xQueue, &tx_output, 1000) )
		{
			printf("Failed to send to Queue\n");
		}
		if (tx_output.min == 0 && tx_output.sec == 0) {
			printf("Times up! ");
			vTaskDelete(xTask1);
		}
		if (tx_output.sec == 0) {
			--tx_output.min;
			tx_output.sec = 60;
		}
		--tx_output.sec;
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void vDisplayTask () {
	clock_t rx_output;

	while(1) {
		if (xQueueReceive(xQueue, &rx_output, 1000) ) {
			printf("Time Remaining: %i", rx_output.min);
			printf(":%02i\n", rx_output.sec);
		}
		else {
			printf("Failed to receive from Queue, ending task\n");
			vTaskDelete(xTask2);
		}
	}
}
