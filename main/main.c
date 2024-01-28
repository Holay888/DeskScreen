#include <stdio.h>
#include "timer_queue.h"
#include "spiffs_user.h"

void app_main(void)
{
    ds_timer_init();
    Spiffs_init();
    Spiffs_test();
    Spiffs_deinit();
}
