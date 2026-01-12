#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/activity.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define LED_NODE DT_NODELABEL(led_gpio_control)

#if DT_NODE_EXISTS(LED_NODE)

static const struct gpio_dt_spec led_gpio = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static int led_control_init(void) {
    if (!gpio_is_ready_dt(&led_gpio)) {
        LOG_ERR("LED GPIO device not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&led_gpio, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure LED GPIO pin");
        return ret;
    }

    // Turn LED on at boot
    gpio_pin_set_dt(&led_gpio, 1);
    LOG_INF("LED control initialized, LED turned on");

    return 0;
}

static int led_activity_listener(const zmk_event_t *eh) {
    enum zmk_activity_state state = zmk_activity_get_state();

    if (state == ZMK_ACTIVITY_IDLE) {
        // Turn LED off when idle
        gpio_pin_set_dt(&led_gpio, 0);
        LOG_INF("Activity idle - LED turned off");
    } else if (state == ZMK_ACTIVITY_ACTIVE) {
        // Turn LED on when active
        gpio_pin_set_dt(&led_gpio, 1);
        LOG_INF("Activity active - LED turned on");
    }

    return 0;
}

ZMK_LISTENER(led_control, led_activity_listener);
ZMK_SUBSCRIPTION(led_control, zmk_activity_state_changed);

SYS_INIT(led_control_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif