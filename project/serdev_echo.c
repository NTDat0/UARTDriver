#include <linux/module.h>
#include <linux/init.h>
#include <linux/serdev.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define RESPONSE_MAX_LENGTH 256
#define RECEIVED_BUFFER_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NONAME GNU/Linux");
MODULE_DESCRIPTION("A simple loopback driver for a UART port with custom response");

static int serdev_echo_probe(struct serdev_device *serdev);
static void serdev_echo_remove(struct serdev_device *serdev);

static char received_buffer[RECEIVED_BUFFER_SIZE];
static size_t buffer_size = 0;
static struct serdev_device *global_serdev;

static int serdev_echo_recv(struct serdev_device *serdev, const unsigned char *buffer, size_t size) {
    size_t i;
    char *cleaned_message;
    char *end;


    for (i = 0; i < size; i++) {
        // Validate the byte range to ensure we only handle printable characters or newlines/carriage returns
        if ((buffer[i] >= 32 && buffer[i] <= 126) || buffer[i] == '\n' || buffer[i] == '\r') {
            if (buffer_size < RECEIVED_BUFFER_SIZE - 1) {
                received_buffer[buffer_size++] = buffer[i];
            }
        }

        // When a newline or carriage return is encountered, treat it as the end of the message
        if (buffer[i] == '\n' || buffer[i] == '\r') {
            received_buffer[buffer_size] = '\0';  // Null-terminate the received string
            cleaned_message = received_buffer;

            // Trim leading whitespace, newlines, and carriage returns
            while (*cleaned_message && (*cleaned_message == ' ' || *cleaned_message == '\n' || *cleaned_message == '\r')) {
                cleaned_message++;
            }

            // Trim trailing whitespace, newlines, and carriage returns
            end = cleaned_message + strlen(cleaned_message) - 1;
            while (end >= cleaned_message && (*end == '\n' || *end == '\r' || *end == ' ')) {
                *end-- = '\0';
            }

            printk(KERN_INFO "serdev_echo - Cleaned message: \"%s\"\n", cleaned_message);

            // Echo back the cleaned message
            if (serdev_device_write_buf(global_serdev, cleaned_message, strlen(cleaned_message)) < 0) {
                printk(KERN_ERR "serdev_echo - Failed to send response\n");
            } else {
                printk(KERN_INFO "serdev_echo - Sent response: \"%s\"\n", cleaned_message);
            }

            buffer_size = 0; // Reset buffer after processing
        }
    }

    return size;
}

static const struct serdev_device_ops serdev_echo_ops = {
    .receive_buf = serdev_echo_recv,
};

static struct of_device_id serdev_echo_ids[] = {
    { .compatible = "brightlight,echodev" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, serdev_echo_ids);

static struct serdev_device_driver serdev_echo_driver = {
    .probe = serdev_echo_probe,
    .remove = serdev_echo_remove,
    .driver = {
        .name = "Nhom2_Uart_Driver",
        .of_match_table = serdev_echo_ids,
    },
};

static int serdev_echo_probe(struct serdev_device *serdev) {
    int status;

    printk(KERN_INFO "serdev_echo - Probing...\n");
    serdev_device_set_client_ops(serdev, &serdev_echo_ops);
    status = serdev_device_open(serdev);
    if (status) {
        printk(KERN_ERR "serdev_echo - Error opening serial port!\n");
        return -status;
    }
    serdev_device_set_baudrate(serdev, 115200);
    global_serdev = serdev;
    return 0;
}

static void serdev_echo_remove(struct serdev_device *serdev) {
    printk(KERN_INFO "serdev_echo - Removing...\n");
    serdev_device_close(serdev);
}

static int __init my_init(void) {
    printk(KERN_INFO "serdev_echo - Loading driver...\n");
    if (serdev_device_driver_register(&serdev_echo_driver)) {
        printk(KERN_ERR "serdev_echo - Error loading driver\n");
        return -1;
    }
    return 0;
}

static void __exit my_exit(void) {
    printk(KERN_INFO "serdev_echo - Unloading driver...\n");
    serdev_device_driver_unregister(&serdev_echo_driver);
}

module_init(my_init);
module_exit(my_exit);

