#include "rn4871_api.h"

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

uint8_t rn4871UartTxCb(char *pBuffer, uint16_t *bufferSize);
uint8_t rn4871UartRxCb(char *pBuffer, uint16_t *bufferSize);
void rn4871DelayMsCb(uint32_t delay);
void rn4871LogSender(char *log, int logLen);

void rn4871LogSender(char *log, int logLen)
{
	if(NULL != log_file)
	{
		fprintf(log_file, "%s", log);
	}
}

uint8_t rn4871UartTxCb(char *pBuffer, uint16_t *bufferSize)
{
    assert((NULL != pBuffer) || (NULL != bufferSize));
	//printf("[TX:%d] %s\r\n", *bufferSize, pBuffer);

	ssize_t sizeWrite = write(serialPort, pBuffer, *bufferSize);
	if(0 >= sizeWrite)
	{
		//printf("Fail to send data : [%d] %s\r\n", *bufferSize, pBuffer);
		return CODE_RETURN_UART_FAIL;
	}

    return CODE_RETURN_SUCCESS;
}

uint8_t rn4871UartRxCb(char *pBuffer, uint16_t *bufferSize)
{
    assert((NULL != pBuffer) || (NULL != bufferSize));

	memset(pBuffer, '\0', BUFFER_UART_MAX_LEN);
	*bufferSize = read(serialPort, pBuffer, BUFFER_UART_MAX_LEN);

	if (0 >= *bufferSize)
	{
		//printf("Fail to receive data: bufferSize=%d\r\n", *bufferSize);
		return CODE_RETURN_UART_FAIL;
  	}
	//printf("[RX:%d] %s\r\n", *bufferSize, pBuffer);

    return CODE_RETURN_SUCCESS;
}

void rn4871DelayMsCb(uint32_t delay)
{
	usleep(delay*1000);
}

int main (void)
{
	printf("---- RN4871 Standalone ----\r\n");
	printf("Logs are stored on the file : %s\r\n", LOG_FILE_NAME);
	//log_file = fopen(LOG_FILE_NAME, "w");
	/*if(NULL == log_file)
	{
		printf("Open log file fail ...\r\n");
	}*/

	printf("Serial port configuration\r\n");
	serialPort = open(FILENAME_SERIAL_PORT, O_RDWR);
	if(0 >= serialPort)
	{
		printf("Fail to open serial port ...\r\n");
		//fclose(log_file);
		return -1;
	}

	printf("Serial port open !\r\n");
	struct termios tty;
	if(0 != tcgetattr(serialPort, &tty))
	{
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		//fclose(log_file);
		return -1;
	}

	printf("Get serial configuration with success !\r\n");

	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= CREAD | CLOCAL;
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHOE;
	tty.c_lflag &= ~ECHONL;
	tty.c_lflag &= ~ISIG;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	tty.c_oflag &= ~OPOST;
	tty.c_oflag &= ~ONLCR;
	tty.c_cc[VTIME] = 10;
	tty.c_cc[VMIN] = 0;
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	if(0 != tcsetattr(serialPort, TCSANOW, &tty))
	{
		printf("Error %i from tcsetattr: %s\r\n", errno, strerror(errno));
		//fclose(log_file);
		return -1;
	}

	printf("Set new serial configuration with success !\r\n");

	struct rn4871_conf_s config =
	{
		.deviceName = "RN4871-standalone",
		.services = DEVICE_INFORMATION | UART_TRANSPARENT,
	};

	struct rn4871_dev_s module =
	{
		.uartTx = rn4871UartTxCb,
		.uartRx = rn4871UartRxCb,
		.delayMs = rn4871DelayMsCb,
	};

	uint8_t ret = rn4871Init(&module);
	if(CODE_RETURN_SUCCESS != ret)
	{
		printf("Fail to enter on command mode ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
		close(serialPort);
		//fclose(log_file);
		return 0;
	}

	ret = rn4871EnterCommandMode(&module);
	if(CODE_RETURN_SUCCESS != ret)
	{
		printf("Fail to enter on command mode ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
		close(serialPort);
		//fclose(log_file);
		return 0;
	}
	printf("RN4871 is on command mode\r\n");

	ret = rn4871SetConfig(&module, &config);
	if(CODE_RETURN_SUCCESS != ret)
	{
		printf("Fail to set config ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
	}

	ret = rn4871RebootModule(&module);
	if(CODE_RETURN_SUCCESS != ret)
	{
		printf("Fail to reboot module ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
		close(serialPort);
		return 0;
	}
	printf("RN4871 is rebooting !\r\n");

	ret = rn4871EnterCommandMode(&module);
	if(CODE_RETURN_SUCCESS != ret)
	{
		printf("Fail to enter on command mode ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
		/* Workaround with a retry ... */
		ret = rn4871EnterCommandMode(&module);
		if(CODE_RETURN_SUCCESS != ret)
		{
			printf("Fail to enter on command mode ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
			close(serialPort);
			return 0;
		}
	}
	printf("RN4871 is on command mode\r\n");

	ret = rn4871GetConfig(&module, &config);
	if(CODE_RETURN_SUCCESS != ret)
	{
		printf("Fail to get config ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
	}
	else
	{
		printf("------ Configuration ------\r\n");
		printf("Device name: %s\r\n", config.deviceName);
		printf("Firmware version: %s\r\n", config.firmwareVersion);
		printf("Mac address: %s\r\n", config.macAddress);
		printf("Services: 0x%02X\r\n", config.services);
		printf("---------------------------\r\n");
	}

	ret = rn4871QuitCommandMode(&module);
	if(CODE_RETURN_SUCCESS != ret)
	{
		printf("Fail to quit command mode ...[%s]\r\n", rn4871GetErrorCodeStr(ret));
		close(serialPort);
		return 0;
	}
	printf("Quit command mode with success !\r\n");

	printf("Now, wait streaming state !\r\n");
	char receivedData[BUFFER_LEN_MAX+1] = "";
	uint16_t receivedDataLen = 0;
	while(FSM_STATE_STREAMING != rn4871GetFsmState(&module))
	{
		/* Wait a little :P */
		rn4871WaitReceivedData(&module, receivedData, &receivedDataLen);
	}

	char dataToSend[BUFFER_LEN_MAX+1] = "";
	uint16_t dataToSendLen = 0;
	uint32_t counter = 0;
	while(FSM_STATE_STREAMING == rn4871GetFsmState(&module))
	{
		dataToSendLen = snprintf(dataToSend, BUFFER_LEN_MAX, "Counter:%d", counter);
		rn4871TransparentUartSendData(&module, dataToSend, dataToSendLen);
		rn4871WaitReceivedData(&module, receivedData, &receivedDataLen);
		++counter;
		sleep(4);
	}

	printf("Streaming is close !\r\n");
	close(serialPort);
	//fclose(log_file);
	printf("Serial port close with success !\r\n");
	return 0;
}