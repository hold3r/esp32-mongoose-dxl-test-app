/*
 *
 */

#include "mgos.h"
#include "mgos_app.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"
#include "mgos_uart.h"
#include "mgos_esp32_mos_dxl.h"

#define LED_PIN   (2)
#define RGB_RED   (26)
#define RGB_GREEN (27)
#define RGB_BLUE  (28)
#define MODULE_ID (0x15)

mgos_timer_id dxl_task1_id;
mgos_timer_id led_task_id;
 
DynamixelDevice * rgb = NULL;


static void dxl_task1(void *arg) 
{
  static uint8_t temp = 0, r = 0, g = 0, b = 0;
  temp = mgos_dxl_module_read(rgb, RGB_RED);
  if (mgos_dxl_module_status(rgb) == 0) r = temp;

  temp = mgos_dxl_module_read(rgb, RGB_GREEN);
  if (mgos_dxl_module_status(rgb) == 0) g = temp;

  temp = mgos_dxl_module_read(rgb, RGB_BLUE);
  if (mgos_dxl_module_status(rgb) == 0) b = temp;


  LOG(LL_INFO, ("LED: %d:%d:%d", r, g, b));

  if (r) {        r = 0; b = 2;
  } else if (b) { b = 0; g = 2;
  } else { b = 0; r = 2; g = 0; 
  }

  mgos_dxl_module_write(rgb, RGB_RED, r);
  mgos_dxl_module_write(rgb, RGB_GREEN, g);
  mgos_dxl_module_write(rgb, RGB_BLUE, b);

  (void) arg;
}


static void led_task(void *arg)
{
  mgos_gpio_toggle(LED_PIN);
  (void) arg;
}


enum mgos_app_init_result mgos_app_init(void) 
{
  mgos_gpio_set_mode(LED_PIN, MGOS_GPIO_MODE_OUTPUT);

  rgb = mgos_dxl_module_create(MODULE_ID);
  mgos_dxl_master_begin(57600);
  mgos_dxl_module_init(rgb);


  dxl_task1_id = mgos_set_timer(1000 , MGOS_TIMER_REPEAT, dxl_task1, NULL);
  led_task_id = mgos_set_timer(333 , MGOS_TIMER_REPEAT, led_task, NULL);

  return MGOS_APP_INIT_SUCCESS;
}


