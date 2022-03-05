idf_component_register(SRCS "concentrator.c" 
                            "radio/radio_callbacks.c" "radio/radio_init.c" 
                            "scanner/scanner.c"
                            "screen/screen_table.c" "screen/screen_window.c" "screen/screen.c"
                            "button/button_init.c" "button/button_callbacks.c"
                            "async.c" "signal.c"                            
                            INCLUDE_DIRS "." "radio" "scanner" "screen" "button")
