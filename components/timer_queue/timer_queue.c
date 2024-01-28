#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "timer_queue.h"

static const char *TAG = "TIMER APP";

//定义结构体
typedef struct {
    uint64_t timer_minute_count;
    uint64_t timer_second_count;
} timer_event_t;

timer_event_t g_timer_event;

QueueHandle_t timer_queue;

//中断回调事件
static bool IRAM_ATTR example_timer_on_alarm_cb_v1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    //定义结构体事件变量
    timer_event_t evt={};
    //向timer_queue队列传参，参数类型为timer_event_t结构体
    xQueueSendFromISR(timer_queue, &evt, &high_task_awoken);
    return (high_task_awoken == pdTRUE);
}

static void gptimer_init(void)
{
    //创建定时器
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    //注册回调函数
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb_v1,
    };
    //指定定时器句柄，要引起的回调函数，要向回调函数传递的值
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, timer_queue));

    //定时器使能
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    //设置警报动作
    gptimer_alarm_config_t alarm_config1 = {
        .alarm_count = 10000, // period = 10ms
        .flags.auto_reload_on_alarm = true, //重加载
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));

    //启动定时器
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

static void timer_example_evt_task(void)
{
    while (1) {
        timer_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
        g_timer_event.timer_minute_count ++;
        //60s 计算一次 刷新时间
        if(g_timer_event.timer_minute_count >= 6000){
            g_timer_event.timer_minute_count = 0;
            ESP_LOGI(TAG,"1Min is run");
        }
        g_timer_event.timer_second_count ++;
        //1s计算一次 
        if(g_timer_event.timer_second_count >= 100){
            g_timer_event.timer_second_count = 0;
            ESP_LOGI(TAG,"1S is run");
        }
    }
}

void ds_timer_init(void)
{
    g_timer_event.timer_minute_count = 0;   //分钟计数
    g_timer_event.timer_second_count = 0;   //秒钟计数
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));  
    gptimer_init();
    xTaskCreate(timer_example_evt_task, "timer_evt_task", 2048, NULL, 1, NULL);
    ESP_LOGI(TAG,"TASK创建成功");
}
