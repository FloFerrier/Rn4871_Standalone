#include "rn4871-driver/rn4871.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define BUFFER_UART_MAX_LEN (255)

int serialPort;

uint8_t rn4871UartTxAPI(uint8_t *pBuffer, uint16_t *bufferSize);
uint8_t rn4871UartRxAPI(uint8_t *pBuffer, uint16_t *bufferSize);
void rn4871DelayMsAPI(uint32_t delay);

uint8_t rn4871UartTxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
	/* Check input buffer */
    if(NULL == pBuffer || NULL == bufferSize)
		return CODE_RETURN_UART_FAIL;

	/* Send data through Uart */
	if(0 >= write(serialPort, pBuffer, *bufferSize)) {
		  printf("Fail to send data : [%d] %s\r\n", *bufferSize, pBuffer);
	}

	/* Debug display */
	printf("[TX:%d] %s\r\n", *bufferSize, (size_t)pBuffer);

    return CODE_RETURN_SUCCESS;
}

uint8_t rn4871UartRxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
	/* Check input buffer */
    if(NULL == pBuffer || NULL == bufferSize)
		return CODE_RETURN_UART_FAIL;

	/* Receive data through Uart */
	memset(pBuffer, '\0', BUFFER_UART_MAX_LEN);
	*bufferSize = read(serialPort, pBuffer, BUFFER_UART_MAX_LEN);
	if (0 >= *bufferSize) {
      	printf("Fail to receive data: %s\r\n", strerror(errno));
  	}
	else {
		/* Debug display */
		printf("[RX:%d] %s\r\n", *bufferSize, pBuffer);
	}

    return CODE_RETURN_SUCCESS;
}

void rn4871DelayMsAPI(uint32_t delay) {
	usleep(delay*1000);
}

int main (void) {
	/* Configuration Serial Port */
	serialPort = open("/dev/ttyUSB0", O_RDWR);
	if(0 >= serialPort) {
		printf("Fail to open serial port ...\r\n");
		return -1;
	}
	printf("Serial port open !\r\n");

	struct termios tty;
	if(0 != tcgetattr(serialPort, &tty)) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return -1;
  	}

	printf("Get serial configuration with success !\r\n");

	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
	// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	tty.c_cc[VTIME] = 10; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	// Set baudrate at 115200 bauds
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	if(0 != tcsetattr(serialPort, TCSANOW, &tty)) {
		printf("Error %i from tcsetattr: %s\r\n", errno, strerror(errno));
		return -1;
	}

	printf("Set serial configuration with success !\r\n");

	/* RN4871 Usecase */
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
		._current_cmd = CMD_NONE,
		.fsm_state = FSM_STATE_INIT,
		.transparentUart = true,
	};

	if(CODE_RETURN_SUCCESS != rn4871SetConfig(&dev)) {
		printf("Set RN4871 config fail ...\r\n");
	}

	close(serialPort);
	printf("Serial port close with success !\r\n");
	return 0;
}
