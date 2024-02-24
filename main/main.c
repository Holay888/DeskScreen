#include <stdio.h>
#include <string.h>
#include "timer_queue.h"
#include "spiffs_user.h"
#include "ds_nvs.h"
#include "ds_print_wifi_data.h"

void app_main(void)
{
    ds_timer_init();
    Spiffs_init();
    Spiffs_test();
    Spiffs_deinit();

    char *ssid="Holay";
    char *psw="123456789";
    set_system_data_wifi_info(ssid,strlen(ssid),psw,strlen(psw));
    ds_nvs_init();
    ds_nvs_save_wifi_info();
    ds_nvs_read_wifi_info();
}
