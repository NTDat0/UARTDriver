# UART Communication System on Raspberry Pi with Linux Driver & Python GUI

This project demonstrates UART communication between a Raspberry Pi and a computer using:

- A custom Linux kernel module (serdev-based driver)
- A Python GUI interface (Tkinter + PySerial)

## 📦 Features

### ✅ Kernel Module (C - Linux driver)

- Custom `serdev` driver named `Nhom2_Uart_Driver`.
- Handles incoming data from UART, strips unwanted characters, and echoes a cleaned version back.
- Logs messages using `printk()` for debugging.
- Device Tree Overlay support with compatible string `"brightlight,echodev"`.

### ✅ Python GUI (Tkinter)

- Simple and clean UI to send/receive UART data.
- Auto-scrolling log area shows communication history.
- Threaded listener for real-time incoming messages.
- Error-handling for connection issues.

### ✅ UART Communication Flow

1. User sends data via GUI.
2. Data is transmitted to Raspberry Pi over `/dev/ttyUSB0`.
3. Raspberry Pi's kernel module receives the data, cleans it, and sends it back.
4. GUI displays the echoed response in real time.

## 🛠 Requirements

- Raspberry Pi with Linux OS
- UART connected to `/dev/ttyUSB0`
- Python 3.x
- PySerial (`pip install pyserial`)
- Linux Kernel headers for building driver

## 🚀 How to Run

### 🔧 Build & Insert Kernel Module

```bash
make
sudo insmod serdev_echo.ko
```
