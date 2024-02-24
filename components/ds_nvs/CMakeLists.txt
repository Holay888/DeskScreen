file(GLOB_RECURSE SOURCES	./*.C
							)

idf_component_register(SRCS	${SOURCES}
			INCLUDE_DIRS
						"."
			REQUIRES driver
					 nvs_flash
					 ds_nvs
					 ds_print_wifi_data)