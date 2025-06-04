#ifndef QUEUE_SENSORES_H
#define QUEUE_SENSORES_H

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xQueueToxicos;
extern QueueHandle_t xQueueInflamaveis;

#endif