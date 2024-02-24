idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES    timer_queue
                                spiffs_user
                                ds_nvs
                                ds_print_wifi_data)
