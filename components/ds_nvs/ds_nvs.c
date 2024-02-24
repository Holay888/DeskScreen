#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "ds_nvs.h"
#include "ds_print_wifi_data.h"

static const char *TAG = "ds_nvs";

//创建全局枚举变量wifi_config
NVS_WIFI_INFO_E wifi_config_flag = NVS_WIFI_INFO_NULL;  

//保存wifi信息进入nvs
void ds_nvs_save_wifi_info(){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    //打开wificondig存储区，可读可写，成功返回到nvs_handle句柄
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle); 
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ;
    }
    wifi_config_flag = NVS_WIFI_INFO_SAVE;
    //设置wifi标志
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "wifi_flag", wifi_config_flag));
    //设置wifi账号
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "ssid", get_system_data().setting_ssid));
    //设置wifi密码
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "psw",  get_system_data().setting_psw));   
    //确保将更改的数据写入nvs中 
    ESP_ERROR_CHECK(nvs_commit(nvs_handle)); 
    //关闭句柄   
    nvs_close(nvs_handle);  
}

//从nvs中读取wifi信息
NVS_WIFI_INFO_E ds_nvs_read_wifi_info(){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return NVS_WIFI_INFO_ERROR;
    }
    uint8_t wifi_config = 0;
    //nvs_handle句柄，wifi_config键值，读取数据存储到wifi_config变量中
    ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, "wifi_flag", &wifi_config));
    //将局部变量wifi_config的值传递给全局变量wifi_config_flag
    wifi_config_flag = wifi_config;
    if(wifi_config_flag == NVS_WIFI_INFO_SAVE){
        ESP_LOGI(TAG,"has wifi config info");
        char ssid[32];
        char psw[64];
        size_t ssid_len = sizeof(ssid);
        size_t psw_len = sizeof(psw);
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len));  //指定nvs_handle句柄，key值，存储地址，长度
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "psw", psw, &psw_len));
        set_system_data_wifi_info(ssid,ssid_len,psw,psw_len);
        print_system_data_wifi_info();
    }else{
        ESP_LOGI(TAG,"wifi config info null");
    }
    nvs_close(nvs_handle);
    return wifi_config_flag;
}

void ds_nvs_init(){
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}