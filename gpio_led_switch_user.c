#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#define LED_DEV "/dev/gpiodemo"
#define BTN_DEV "/dev/input/event5"  // adjust based on `dmesg` or `lsinput`

int main() {
    int btn_fd, led_fd;
    struct input_event ev;

    btn_fd = open(BTN_DEV, O_RDONLY);
    if (btn_fd < 0) {
        perror("open button");
        return 1;
    }

    led_fd = open(LED_DEV, O_WRONLY);
    if (led_fd < 0) {
        perror("open led");
        close(btn_fd);
        return 1;
    }

    while (1) {
        if (read(btn_fd, &ev, sizeof(ev)) > 0) {
            if (ev.type == EV_KEY && ev.code == 28) { // KEY_ENTER
                if (ev.value == 1) { 
                    write(led_fd, "0", 1);
                    printf("Button Released -> LED OFF\n");
                } else if (ev.value == 0) { 
                    write(led_fd, "1", 1);
                    printf("Button Pressed -> LED ON\n");
                }
            }
        }
    }

    close(led_fd);
    close(btn_fd);
    return 0;
}

