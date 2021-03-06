/**
  ******************************************************************************
  * @file    IO_Toggle/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f429i_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup IO_Toggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void Hardware_Init(void);
void Red_LED_On(void);
void Red_LED_Off(void);
void Green_LED_On(void);
void Green_LED_Off(void);
void ToggleLED1_Task(void*);
void ToggleLED2_Task(void*);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */


//张照博自己写的--START
int32_t Send_Sum=0;
int32_t Received_Sum=0;
   //建立队列 
xQueueHandle MyQueue; 
void Sender_Task(void *pvParameters)
{
    int32_t Send_Num = 1; 
    for( ;; )  
    {  
        vTaskDelay( 2 / portTICK_RATE_MS );
        if (Send_Num>10000)
        {
          Send_Num=1;
        }
        /* 向队列中填充内容 */  
        xQueueSendToBack( MyQueue, ( void* )&Send_Num, 0 );   
        *((int32_t*)pvParameters)+=Send_Sum;
        Send_Num++; 
     }
}

void Receiver_Task(void *pvParameters)
{
  int32_t  Received_Num = 0;  
    for( ;; )  
    {  
        /* 从队列中获取内容 */  
        if( xQueueReceive( MyQueue, &Received_Num, 1000 / portTICK_RATE_MS ) == pdTRUE)  
        {  
          
          *((int32_t*)pvParameters)+=Received_Num;
           Received_Num=*((int32_t*)pvParameters);
        }  
    }  
}

void Monitor_Task(void *pvParameters)
{
for(;;)
  {

  vTaskDelay( 10000 / portTICK_RATE_MS ); 
   Green_LED_Off();
  if(Send_Sum==Received_Sum)
    {
      Green_LED_On();
      Red_LED_Off();
      Send_Sum=0;
      Received_Sum=0;
    } 
  else 
    {
      Green_LED_Off();
      Red_LED_On();
      Send_Sum=0;
      Received_Sum=0;
    }
  }
}

//张照博自己写的--END

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */
    Hardware_Init();
    // 初始化硬件平台  
    //prvSetupHardware();  
    //创建全局变量
 MyQueue = xQueueCreate( 510 , sizeof( int32_t ) ); 
    // 建立任务  
    xTaskCreate( Sender_Task, ( signed portCHAR * ) "Sender_Task", configMINIMAL_STACK_SIZE,(void*)&Send_Sum, tskIDLE_PRIORITY+3, NULL );  
    xTaskCreate( Receiver_Task, ( signed portCHAR * ) "Receiver_Task", configMINIMAL_STACK_SIZE,(void*)&Received_Sum, tskIDLE_PRIORITY+4, NULL );   
    xTaskCreate( Monitor_Task, ( signed portCHAR * ) "Monitor_Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+5, NULL ); 
  //启动OS  
    vTaskStartScheduler();  
    return 0; 
 //我的第一次调试，需要去掉后面的
       /* Init and start tracing*/
         vTraceEnable(TRC_INIT);
         vTraceEnable(TRC_START);

 //       /* Create tasks */
 //       xTaskCreate(
	// 	  ToggleLED1_Task,                 /* Function pointer */
	// 	  "Task_LED1",                          /* Task name - for debugging only*/
	// 	  configMINIMAL_STACK_SIZE,         /* Stack depth in words */
	// 	  (void*) NULL,                     /* Pointer to tasks arguments (parameter) */
	// 	  tskIDLE_PRIORITY + 3UL,           /* Task priority*/
	// 	  NULL                              /* Task handle */
 //       );

 //       xTaskCreate(
	// 	  ToggleLED2_Task,                 /* Function pointer */
	// 	  "Task_LED2",                           Task name - for debugging only
	// 	  configMINIMAL_STACK_SIZE,         /* Stack depth in words */
	// 	  (void*) NULL,                     /* Pointer to tasks arguments (parameter) */
	// 	  tskIDLE_PRIORITY + 2UL,           /* Task priority*/
	// 	  NULL                              /* Task handle */
 //       );

	// /* Start the scheduler. */
	// vTaskStartScheduler();

	// /* If all is well, the scheduler will now be running, and the following line
	// will never be reached.  If the following line does execute, then there was
	// insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	// to be created.  See the memory management section on the FreeRTOS web site
	// for more details. */
	// for( ;; );
//我的第一次调试，去掉的位置结尾


}


/**
 * Hardware_Init: 
 */
void Hardware_Init(void)
{
        /* GPIOG Periph clock enable */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

        /* Configure PG13, PG14 in output pushpull mode */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOG, &GPIO_InitStructure);

}
/**
 * Red_LED_On: 
 */
void Red_LED_On(void)
{
//    GPIO_SetBits(GPIOG, GPIO_Pin_14);
    GPIOG->ODR |= 0x4000;
}

/**
 * Red_LED_Off: 
 */
void Red_LED_Off(void)
{
//    GPIO_ResetBits(GPIOG, GPIO_Pin_14);
  GPIOG->ODR &= 0xBFFF;
}

/**
 * Green_LED_On: 
 */
void Green_LED_On(void)
{
//    GPIO_SetBits(GPIOG, GPIO_Pin_13);
    GPIOG->ODR |= 0x2000;
}

/**
 * Green_LED_Off: 
 */
void Green_LED_Off(void)
{
//    GPIO_ResetBits(GPIOG, GPIO_Pin_13);
    GPIOG->ODR &= 0xDFFF;
}
/**
 * ToggleLED1_Task: Toggle LED1 via RTOS Timer
 */
void ToggleLED1_Task(void *pvParameters)
{
    int led = 0;  

    while (1) 
    {
        if(led == 0)
        {
            Red_LED_On();
            led = 1;
        } 
        else
        {
            Red_LED_Off();
            led = 0;
         }
        /*
        Delay for a period of time. vTaskDelay() places the task into
        the Blocked state until the period has expired.
        The delay period is spacified in 'ticks'. We can convert
        yhis in milisecond with the constant portTICK_RATE_MS.
        */
        vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

/**
 * ToggleLED2_Task: Toggle LED2 via RTOS Timer
 */
void ToggleLED2_Task(void *pvParameters)

{
    int led = 0;  
    while (1) 
    {
        if(led == 0)
        {
            Green_LED_On();
            led = 1;
        } 
        else
        {
            Green_LED_Off();
            led = 0;
         }
        /*
        Delay for a period of time. vTaskDelay() places the task into
        the Blocked state until the period has expired.
        The delay period is spacified in 'ticks'. We can convert
        yhis in milisecond with the constant portTICK_RATE_MS.
        */
        vTaskDelay(2000 / portTICK_RATE_MS);
  }
}

void vApplicationTickHook( void )
{
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
