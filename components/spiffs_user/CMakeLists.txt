file(GLOB_RECURSE SOURCES	./*.C
							)

idf_component_register(SRCS	${SOURCES}
			INCLUDE_DIRS
						"."
			REQUIRES driver
					 spiffs)