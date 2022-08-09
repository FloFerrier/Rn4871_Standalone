#include "unity.h"
#include "rn4871.h"
#include "virtual_module.h"
#include "logs.h"

#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define BUFFER_LEN_MAX (255)

static struct rn4871_dev_s *test_device;

uint8_t rn4871UartTxAPI(uint8_t *pBuffer, uint16_t *bufferSize);
uint8_t rn4871UartRxAPI(uint8_t *pBuffer, uint16_t *bufferSize);
void rn4871DelayMsAPI(uint32_t delay);
void rn4871LogSenderAPI(char *log, int logLen);

void rn4871LogSenderAPI(char *log, int logLen) {
	printf("%s", log);
}

uint8_t rn4871UartTxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
    assert((NULL != pBuffer) || (NULL != bufferSize));
	//printf("[TX:%d] %s\r\n", *bufferSize, pBuffer);

	if(VIRTUAL_MODULE)
		virtualModuleReceiveData(pBuffer, *bufferSize);
	else
		printf("Real module : To do !\r\n");

    return CODE_RETURN_SUCCESS;
}

uint8_t rn4871UartRxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
    assert((NULL != pBuffer) || (NULL != bufferSize));
	//printf("[RX:%d] %s\r\n", *bufferSize, pBuffer);

	if(VIRTUAL_MODULE)
		virtualModuleSendData(pBuffer, bufferSize);
	else
		printf("Real module : To do !\r\n");

    return CODE_RETURN_SUCCESS;
}

void rn4871DelayMsAPI(uint32_t delay) {
	usleep(delay*1000);
}

/* Is run before every test, put unit init calls here. */
void setUp(void) {
	test_device = malloc(sizeof(struct rn4871_dev_s)); 
	test_device->uartTx = rn4871UartTxAPI;
	test_device->uartRx = rn4871UartRxAPI;
	test_device->delayMs = rn4871DelayMsAPI;
	rn4871SetForceFsmState(FSM_STATE_NONE);
	rn4871SetForceDataMode();
	virtualModuleSetForceDataMode();
}

/* Is run after every test, put unit clean-up calls here. */
void tearDown(void) {
	free(test_device);
}

void test_virtualModule(void) {
	char *input = malloc(sizeof(char)*(BUFFER_LEN_MAX+1));
	uint16_t inputLen = 0;
	char *output = malloc(sizeof(char)*(BUFFER_LEN_MAX+1));
	uint16_t outputLen = 0;
	char *expectedOutput = malloc(sizeof(char)*(BUFFER_LEN_MAX+1));
	uint16_t expectedOutputLen = 0;

	inputLen = snprintf(input, BUFFER_LEN_MAX, "$");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleReceiveData(input, inputLen);
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "CMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "Fake\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "Err\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "R,1\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "Rebooting\r\n%cREBOOT%c", '%', '%');
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "$");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleReceiveData(input, inputLen);
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "CMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "SF,1\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "Rebooting\r\n%cREBOOT%c", '%', '%');
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "$");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleReceiveData(input, inputLen);
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "CMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "V\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX,
		"RN4871 V1.40 7/9/2019 (c)Microship Technology Inc\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "D\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX,
		"BTA=001122334455\r\nName=RN4871-VM\r\nConnected=no\r\nAuthen=0\r\nFeatures=0000\r\nServices=80\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "SN,RN4871-Test\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "AOK\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "SS,80\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "AOK\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "SS,C0\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "AOK\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "PZ\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "AOK\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	inputLen = snprintf(input, BUFFER_LEN_MAX, "---\r\n");
	virtualModuleReceiveData(input, inputLen);
	virtualModuleSendData(output, &outputLen);
	expectedOutputLen = snprintf(expectedOutput, BUFFER_LEN_MAX, "AOK\r\nCMD>");
	TEST_ASSERT_EQUAL_STRING(expectedOutput, output);
	TEST_ASSERT_EQUAL_UINT16(expectedOutputLen, outputLen);

	free(input);
	free(output);
	free(expectedOutput);
}

void test_rn4871EnterCommandMode(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
}

void test_rn4871QuitCommandMode(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871QuitCommandMode(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871RebootModule(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871QuitCommandMode(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871QuitCommandMode(test_device));
}

void test_rn4871RebootModule(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871RebootModule(test_device));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871RebootModule(test_device));
}

void test_rn4871SetDeviceName(void) {
	char *deviceName = malloc(BUFFER_UART_LEN_MAX);
	int sizeDeviceName = snprintf(deviceName, BUFFER_UART_LEN_MAX, "test_device_name");
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871SetDeviceName(test_device, deviceName, (size_t)sizeDeviceName));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetDeviceName(test_device, deviceName, (size_t)sizeDeviceName));
	TEST_ASSERT_EQUAL_STRING("test_device_name", deviceName);
	free(deviceName);
}

void test_rn4871GetDeviceName(void) {
	char *deviceName = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871GetDeviceName(test_device, deviceName));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetDeviceName(test_device, deviceName));
	TEST_ASSERT_EQUAL_STRING("test_device_name", deviceName);
	free(deviceName);
}

void test_rn4871GetFirmwareVersion(void) {
	char *firmwareVersion = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871GetFirmwareVersion(test_device, firmwareVersion));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetFirmwareVersion(test_device, firmwareVersion));
	TEST_ASSERT_EQUAL_STRING("V1.40", firmwareVersion);
	free(firmwareVersion);
}

void test_rn4871GetMacAddress(void) {
	char *macAddress = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871GetMacAddress(test_device, macAddress));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetMacAddress(test_device, macAddress));
	TEST_ASSERT_EQUAL_STRING("001122334455", macAddress);
	free(macAddress);
}

void test_rn4871SetServices(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871SetServices(test_device, DEVICE_INFORMATION));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION | UART_TRANSPARENT));
}

void test_rn4871GetServices(void) {
	uint16_t services = 0x00;
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871GetServices(test_device, &services));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetServices(test_device, &services));
	TEST_ASSERT_EQUAL_UINT16(DEVICE_INFORMATION | UART_TRANSPARENT, services);
}

void test_rn4871IsOnTransparentUart(void) {
	bool result = false;
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871IsOnTransparentUart(test_device, &result));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, UART_TRANSPARENT));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871IsOnTransparentUart(test_device, &result));
	TEST_ASSERT_EQUAL_UINT8(result, true);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871IsOnTransparentUart(test_device, &result));
	TEST_ASSERT_EQUAL_UINT8(result, false);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION | UART_TRANSPARENT));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871IsOnTransparentUart(test_device, &result));
	TEST_ASSERT_EQUAL_UINT8(result, true);
}

void test_rn4871EraseAllGattServices(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_COMMAND_MODE, rn4871EraseAllGattServices(test_device));
	rn4871EnterCommandMode(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EraseAllGattServices(test_device));
}

void test_rn4871TransparentUartSendData(void) {
	char *data = malloc(sizeof(char)*(BUFFER_LEN_MAX+1));
	int dataLen = snprintf(data, BUFFER_LEN_MAX, "Test data to send with transparent UART");
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_STREAMING, rn4871TransparentUartSendData(test_device, data, dataLen));
	virtualModuleConnect(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_STREAMING, rn4871TransparentUartSendData(test_device, data, dataLen));
	virtualModuleStream(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871TransparentUartSendData(test_device, data, dataLen));
	virtualModuleDisconnect(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_STREAMING, rn4871TransparentUartSendData(test_device, data, dataLen));
	free(data);
}

void test_rn4871GetFsmState(void) {
	TEST_ASSERT_EQUAL(FSM_STATE_NONE, rn4871GetFsmState());
	virtualModuleConnect(test_device);
	TEST_ASSERT_EQUAL(FSM_STATE_CONNECTED, rn4871GetFsmState());
	virtualModuleStream(test_device);
	TEST_ASSERT_EQUAL(FSM_STATE_STREAMING, rn4871GetFsmState());
	virtualModuleDisconnect(test_device);
	TEST_ASSERT_EQUAL(FSM_STATE_IDLE, rn4871GetFsmState());
}

void test_transparentUartModeScenario1(void) {
	char *data = malloc(sizeof(char)*(BUFFER_LEN_MAX+1));
	int dataLen = snprintf(data, BUFFER_LEN_MAX, "Test data to send with transparent UART");
	char *buffer = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
	char deviceName[] = "test_uart_mode";
	int sizeDeviceName = strlen(deviceName);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetDeviceName(test_device, deviceName, sizeDeviceName));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetDeviceName(test_device, buffer));
	TEST_ASSERT_EQUAL_STRING(deviceName, buffer);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetFirmwareVersion(test_device, buffer));
	TEST_ASSERT_EQUAL_STRING("V1.40", buffer);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION | UART_TRANSPARENT));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871RebootModule(test_device));
	virtualModuleConnect(test_device);
	virtualModuleStream(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871TransparentUartSendData(test_device, data, dataLen));
	free(buffer);
	free(data);
}

void test_transparentUartModeScenario2(void) {
	char *data = malloc(sizeof(char)*(BUFFER_LEN_MAX+1));
	int dataLen = snprintf(data, BUFFER_LEN_MAX, "Test data to send with transparent UART");
	char *buffer = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, UART_TRANSPARENT));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871RebootModule(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
	bool result = false;
	rn4871IsOnTransparentUart(test_device, &result);
	TEST_ASSERT_EQUAL_UINT8(true, result);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871QuitCommandMode(test_device));
	virtualModuleConnect(test_device);
	virtualModuleStream(test_device);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871TransparentUartSendData(test_device, data, dataLen));
	free(buffer);
	free(data);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_virtualModule);
	RUN_TEST(test_rn4871EnterCommandMode);
	RUN_TEST(test_rn4871RebootModule);
	RUN_TEST(test_rn4871QuitCommandMode);
	RUN_TEST(test_rn4871GetFirmwareVersion);
	RUN_TEST(test_rn4871GetMacAddress);
	RUN_TEST(test_rn4871SetServices);
	RUN_TEST(test_rn4871SetDeviceName);
	RUN_TEST(test_rn4871GetDeviceName);
	RUN_TEST(test_rn4871GetServices);
	RUN_TEST(test_rn4871GetFsmState);
	RUN_TEST(test_rn4871EraseAllGattServices);
	RUN_TEST(test_rn4871IsOnTransparentUart);
	RUN_TEST(test_rn4871TransparentUartSendData);
	RUN_TEST(test_transparentUartModeScenario1);
	RUN_TEST(test_transparentUartModeScenario2);
	return UNITY_END();
}