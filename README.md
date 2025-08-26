```markdown
```
## GPIO Demo: Button and LED on Raspberry Pi 5

This repository contains a complete GPIO demo for Raspberry Pi 5, including:

 A **button overlay** that generates input events when a button is pressed.
 An **LED overlay and driver** to control an LED via a character device.
 A **user-space application** that reads button events and switches the LED on/off.

---

## Repository Structure

```
gpio_led_switch\
gpio_led_switch.dts           # Combined button + LED overlay
gpio_led_switch_driver.c      # Kernel module (LED character device driver)
gpio_led_switch_user.c        # User-space program (reads button, controls LED)
Makefile                      # Build rules
README.md                     # Documentation

````
---

## Features

1. **Button input detection**  
   - Uses GPIO17 as an active-low input.  
   - Generates EV_KEY events on `/dev/input/eventX`.

2. **LED control**  
   - Uses GPIO16 as an active-high output.  
   - Controlled via `/dev/gpiodemo` character device.

3. **User-space application**  
   - Reads button events from `/dev/input/eventX`.  
   - Writes `1` or `0` to `/dev/gpiodemo` to toggle the LED.  

---

## Setup Instructions

### Step 1: Compile the overlay

```bash
dtc -@ -I dts -O dtb -o gpio_led_switch.dtbo gpio_led_switch.dts
````

### Step 2: Remove any previous overlays

```bash
sudo dtoverlay -r 0
```

### Step 3: Load the new overlay

```bash
sudo dtoverlay gpio_led_switch.dtbo
```

### Step 4: Compile the driver

```bash
make
```

### Step 5: Load the kernel module

```bash
sudo insmod gpio_led_switch_driver.ko
```

### Step 6: Verify devices

1. **Button:**

```bash
grep -A5 "gpio-keys" /proc/bus/input/devices
```

2. **LED driver:**

```bash
ls /dev/gpiodemo
```

### Step 7: Test button events

```bash
sudo evtest /dev/input/eventX
```

* Pressing the button should generate `EV_KEY` events.

### Step 8: Test LED driver manually

```bash
echo 1 | sudo tee /dev/gpiodemo   # LED ON
echo 0 | sudo tee /dev/gpiodemo   # LED OFF
```

### Step 9: Run the user-space program

1. Compile the `gpio_led_switch_user.c` using `gcc`:

```bash
gcc gpio_led_switch_user.c -o gpio_led_switch_user
```

2. Run the program as root:

```bash
sudo ./gpio_led_switch_user
```

* Pressing the button will turn the LED **ON**, releasing it will turn it **OFF**.

---

## Notes

* This setup is tested on **Raspberry Pi 5**.
* The combined DTS overlay configures GPIO17 for input with internal pull-up and GPIO16 for LED output.
* Debounce interval for the button is set to 50 ms for reliable detection.

---

## Author

**Akhilesh Kakarla**
