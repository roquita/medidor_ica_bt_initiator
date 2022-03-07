idf_component_register(SRCS "concentrator.c" 
                            "radio/radio_callbacks.c" "radio/radio_init.c" "radio/radio_uart_io.c"
                            "scanner/scanner.c"
                            "screen/screen_table.c" "screen/screen_window.c" "screen/screen.c" 
                            "screen/ssd1306.c" "screen/ssd1306_spi.c" "screen/ssd1306_i2c.c"
                            "button/button_init.c" "button/button_callbacks.c"
                            "async.c" "signal.c"                            
                            INCLUDE_DIRS "." "radio" "scanner" "screen" "button")
