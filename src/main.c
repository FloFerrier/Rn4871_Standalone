#include "rn4871_api.h"
#include "virtual_module.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>

#define BUFFER_UART_MAX_LEN (255)
#define BUFFER_LEN_MAX (255)

#define FILENAME_SERIAL_PORT "/dev/ttyUSB0"

int serialPort;
FILE *log_file;

uint8_t rn4871UartTxAPI(char *pBuffer, uint16_t *bufferSize);
uint8_t rn4871UartRxAPI(char *pBuffer, uint16_t *bufferSize);
void rn4871DelayMsAPI(uint32_t delay);
void rn4871LogSenderAPI(char *log, int logLen);

void rn4871LogSenderAPI(char *log, int logLen) {
	if(NULL != log_file) {
		fprintf(log_file, "%s", log);
	}
}

uint8_t rn4871UartTxAPI(char *pBuffer, uint16_t *bufferSize) {
    assert((NULL != pBuffer) || (NULL != bufferSize));
	//printf("[TX:%d] %s\r\n", *bufferSize, pBuffer);

	if(VIRTUAL_MODULE) {
		virtualModuleReceiveData((char*)pBuffer, *bufferSize);
	}
	else {
		ssize_t sizeWrite = write(serialPort, pBuffer, *bufferSize);
		if(0 >= sizeWrite) {
			printf("Fail to send data : [%d] %s\r\n", *bufferSize, pBuffer);
			return CODE_RETURN_UART_FAIL;
		}
	}

    return CODE_RETURN_SUCCESS;
}

uint8_t rn4871UartRxAPI(char *pBuffer, uint16_t *bufferSize) {
    assert((NULL != pBuffer) || (NULL != bufferSize));

	memset(pBuffer, '\0', BUFFER_UART_MAX_LEN);
	if(VIRTUAL_MODULE) {
		virtualModuleSendData((char*)pBuffer, bufferSize);
	}
	else {
		*bufferSize = read(serialPort, pBuffer, BUFFER_UART_MAX_LEN);
	}

	if (0 >= *bufferSize) {
      	printf("Fail to receive data: %s\r\n", strerror(errno));
		return CODE_RETURN_UART_FAIL;
  	}
	//printf("[RX:%d] %s\r\n", *bufferSize, pBuffer);

    return CODE_RETURN_SUCCESS;
}

void rn4871DelayMsAPI(uint32_t delay) {
	usleep(delay*1000);
}

int main (void) {

	printf("Logs are stored on the file : %s\r\n", LOG_FILE_NAME);
	log_file = fopen(LOG_FILE_NAME, "w");
	if(NULL == log_file) {
		printf("Open log file fail ...\r\n");
	}

	if(VIRTUAL_MODULE) {
		printf("Virtual module selected !\r\n");
	}
	else {
		printf("Real module selected !\r\n");
		printf("Serial port configuration\r\n");
		serialPort = open(FILENAME_SERIAL_PORT, O_RDWR);
		if(0 >= serialPort) {
			printf("Fail to open serial port ...\r\n");
			fclose(log_file);
			return -1;
		}
		printf("Serial port open !\r\n");
		struct termios tty;
		if(0 != tcgetattr(serialPort, &tty)) {
			printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
			fclose(log_file);
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
			fclose(log_file);
			return -1;
		}

		printf("Set new serial configuration with success !\r\n");
	}

	/* RN4871 Usecase */
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
	};

	uint8_t ret = rn4871EnterCommandMode(&dev);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to enter on command mode ...\r\n");
		close(serialPort);
		fclose(log_file);
		return 0;
	}
	printf("RN4871 is on command mode\r\n");

	const char deviceName[21] = "test_main";
	ret = rn4871SetDeviceName(&dev, deviceName, strlen(deviceName));
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to set device name ...%d\r\n", ret);
	}

	char realDeviceName[21] = "";
	ret = rn4871GetDeviceName(&dev, realDeviceName);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to get device name ...%d\r\n", ret);
	}

	if(0 == strcmp(deviceName, realDeviceName)) {
		printf("RN4871 module is now name : %s\r\n", realDeviceName);
	}
	else {
		printf("New device name is not setting correctly ...\r\n");
	}

	char firmwareVersion[BUFFER_LEN_MAX+1] = "";
	ret = rn4871GetFirmwareVersion(&dev, firmwareVersion);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to get firmware version ...%d\r\n", ret);
	}
	else {
		printf("RN4871 module firmware version : %s\r\n", firmwareVersion);
	}

	char macAddress[BUFFER_LEN_MAX+1] = "";
	ret = rn4871GetMacAddress(&dev, macAddress);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to get mac address ...%d\r\n", ret);
	}
	else {
		printf("RN4871 module mac address : %s\r\n", macAddress);
	}

	ret = rn4871SetServices(&dev, DEVICE_INFORMATION | UART_TRANSPARENT);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to set services ...%d\r\n", ret);
	}
	else {
		printf("RN4871 module services as Transparent UART\r\n");
	}

	ret = rn4871RebootModule(&dev);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to reboot module ...%d\r\n", ret);
	}
	printf("RN4871 module reboot with success\r\n");

	ret = rn4871EnterCommandMode(&dev);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to enter on command mode ...\r\n");
		close(serialPort);
		fclose(log_file);
		return 0;
	}
	printf("RN4871 is on command mode\r\n");
	bool modeIsTransparentUart = false;
	ret = rn4871IsOnTransparentUart(&dev, &modeIsTransparentUart);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail on rn4871IsOnTransparentUart function ...\r\n");
		close(serialPort);
		fclose(log_file);
		return 0;
	}
	if(!modeIsTransparentUart) {
		printf("RN4871 module is not on Transparent Uart mode ...\r\n");
		close(serialPort);
		fclose(log_file);
		return 0;
	}
	ret = rn4871QuitCommandMode(&dev);
	if(CODE_RETURN_SUCCESS != ret) {
		printf("Fail to quit on command mode ...\r\n");
		close(serialPort);
		fclose(log_file);
		return 0;
	}

	/* Wait connection and streaming by an external device */
	char *dataToRecv = malloc(sizeof(char)*(255+1));
	uint16_t dataToRecvLen = 0;
	while(FSM_STATE_STREAMING != rn4871GetFsmState()) {
		dev.uartRx(dataToRecv, &dataToRecvLen);
		virtualModuleConnect(&dev);
		virtualModuleStream(&dev);
		printf("DEBUG FSM state : %d\r\n", rn4871GetFsmState());
	}
	free(dataToRecv);

	char *dataToSend = malloc(sizeof(char)*(255+1));
	int sizeToSend = snprintf(dataToSend, 255, "Test data to send");
	if(CODE_RETURN_SUCCESS != rn4871TransparentUartSendData(&dev, dataToSend, sizeToSend)) {
		printf("Error to transmit data through transparent uart ...\r\n");
	}
	else {
		printf("Success to transmit data through transparent uart !\r\n");
	}

	free(dataToSend);
	close(serialPort);
	fclose(log_file);
	printf("Serial port close with success !\r\n");
	return 0;
}