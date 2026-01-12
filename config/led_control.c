#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/activity.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Get GPIO device and pin directly from devicetree
#define LED_GPIO_NODE DT_NODELABEL(pro_micro)
#define LED_PIN 8

#if DT_NODE_EXISTS(LED_GPIO_NODE)

static const struct device *gpio_dev;

static int led_control_init(void) {
    gpio_dev = DEVICE_DT_GET(LED_GPIO_NODE);
    
    if (!device_is_ready(gpio_dev)) {
        LOG_ERR("LED GPIO device not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure(gpio_dev, LED_PIN, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure LED GPIO pin");
        return ret;
    }

    // Turn LED on at boot
    gpio_pin_set(gpio_dev, LED_PIN, 1);
    LOG_INF("LED control initialized on pin %d, LED turned on", LED_PIN);

    return 0;
}

static int led_activity_listener(const zmk_event_t *eh) {
    enum zmk_activity_state state = zmk_activity_get_state();

    if (state == ZMK_ACTIVITY_IDLE) {
        // Turn LED off when idle
        gpio_pin_set(gpio_dev, LED_PIN, 0);
        LOG_INF("Activity idle - LED turned off");
    } else if (state == ZMK_ACTIVITY_ACTIVE) {
        // Turn LED on when active
        gpio_pin_set(gpio_dev, LED_PIN, 1);
        LOG_INF("Activity active - LED turned on");
    }

    return 0;
}

ZMK_LISTENER(led_control, led_activity_listener);
ZMK_SUBSCRIPTION(led_control, zmk_activity_state_changed);

SYS_INIT(led_control_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif