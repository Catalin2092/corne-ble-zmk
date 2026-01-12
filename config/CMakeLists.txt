if(CONFIG_ZMK_LED_CONTROL)
  target_sources(app PRIVATE led_control.c)
endif()