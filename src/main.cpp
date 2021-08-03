/*
 *
 */

#include "mgos.h"
#include "mgos_app.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"
#include "mgos_uart.h"

#include "mgos_esp32_mos_dxl.h"

#define LED_PIN 2
#define UART_NO 0

#define RGB_RED   (26)
#define RGB_GREEN (27)
#define RGB_BLUE  (28)


#define MODULE_ID (0x15)

mgos_timer_id dxl_task1_id;
mgos_timer_id led_task_id;
 
static DynamixelDevice * dxl_module = NULL;



void uart_init(uint8_t uart_no);

static void dxl_task1(void *arg) 
{
  uint8_t r,g,b;
  mgos_dxl_module_read(dxl_module, RGB_RED, &r);
  mgos_dxl_module_read(dxl_module, RGB_GREEN, &g);
  mgos_dxl_module_read(dxl_module, RGB_BLUE, &b);

  LOG(LL_INFO, ("LED: %d:%d:%d", r, g, b));

  if (r) {
    r = 0;
    b = 2;
  } else if (b) {
    b = 0;
    g = 2;
  } else {
    b = 0;
    r = 2;
    g = 0; 
  }

  mgos_dxl_module_write(dxl_module, RGB_RED, &r);
  mgos_dxl_module_write(dxl_module, RGB_GREEN, &g);
  mgos_dxl_module_write(dxl_module, RGB_BLUE, &b);

  (void) arg;
}


static void led_task(void *arg)
{
  mgos_gpio_toggle(LED_PIN);
  (void) arg;
}

//extern "C" 
enum mgos_app_init_result mgos_app_init(void) 
{
  uart_init(UART_NO);
  dxl_module = mgos_dxl_module_create(MODULE_ID);

  mgos_gpio_set_mode(LED_PIN, MGOS_GPIO_MODE_OUTPUT);
  dxl_task1_id = mgos_set_timer(1000 , MGOS_TIMER_REPEAT, dxl_task1, NULL);
  led_task_id = mgos_set_timer(333 , MGOS_TIMER_REPEAT, led_task, NULL);

  mgos_dxl_module_begin(57600);
 
  mgos_dxl_module_init(dxl_module);

  return MGOS_APP_INIT_SUCCESS;
}

void uart_init(uint8_t uart_no)
{
  struct mgos_uart_config ucfg;

  mgos_uart_config_set_defaults(uart_no, &ucfg);

  ucfg.baud_rate = 115200;
  ucfg.num_data_bits = 8;
  ucfg.parity = MGOS_UART_PARITY_NONE;
  ucfg.stop_bits = MGOS_UART_STOP_BITS_1;

  if (!mgos_uart_configure(uart_no, &ucfg)) {
    LOG(LL_ERROR, ("Failed to configure UART%d", uart_no));
  }
}

