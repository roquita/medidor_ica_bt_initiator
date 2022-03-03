idf_component_register(SRCS "concentrator.c" 
                            "radio/radio_callbacks.c" "radio/radio_init.c" 
                            "scanner/scanner.c"
                            "main_async.c"
                              INCLUDE_DIRS "." "radio" "scanner")
