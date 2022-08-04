#include "unity.h"
#include "rn4871.h"
#include "virtual_module.h"

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

uint8_t rn4871UartTxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
    assert((NULL != pBuffer) || (NULL != bufferSize));
	printf("[TX:%d] %s\r\n", *bufferSize, pBuffer);

	if(VIRTUAL_MODULE)
		uartRxVirtualModule(pBuffer, *bufferSize);
	else
		printf("Real module : To do !\r\n");

    return CODE_RETURN_SUCCESS;
}

uint8_t rn4871UartRxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
    assert((NULL != pBuffer) || (NULL != bufferSize));
	printf("[RX:%d] %s\r\n", *bufferSize, pBuffer);

	if(VIRTUAL_MODULE)
		uartTxVirtualModule(pBuffer, bufferSize);
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
}

/* Is run after every test, put unit clean-up calls here. */
void tearDown(void) {
	free(test_device);
}

void test_virtualModule(void) {
	uint8_t pInput[BUFFER_LEN_MAX+1] = "";
	uint16_t inputSize = 0;
	uint8_t pOutput[BUFFER_LEN_MAX+1] = "";
	uint16_t outputSize = 0;
	TEST_ASSERT_EQUAL_STRING("", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen(""), outputSize);

	strcpy(pInput, "Fake\r\n");
	inputSize = strlen("Fake\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	strcpy(pInput, "$");
	inputSize = strlen("$");
	uartRxVirtualModule(pInput, inputSize);
	uartRxVirtualModule(pInput, inputSize);
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("CMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("CMD>"), outputSize);

	strcpy(pInput, "R,1\r\n");
	inputSize = strlen("R,1\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Rebooting\r\n%REBOOT%", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Rebooting\r\n%REBOOT%"), outputSize);

	strcpy(pInput, "SF,1\r\n");
	inputSize = strlen("SF,1\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Rebooting\r\n%REBOOT%", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Rebooting\r\n%REBOOT%"), outputSize);

	strcpy(pInput, "SF,3\r\n");
	inputSize = strlen("SF,3\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	strcpy(pInput, "S-,blabla\r\n");
	inputSize = strlen("S-,blabla\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	strcpy(pInput, "SN,RN4871-Test\r\n");
	inputSize = strlen("SN,RN4871-Test\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	strcpy(pInput, "GN\r\n");
	inputSize = strlen("GN\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("RN4871-Test\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("RN4871-Test\r\nCMD>"), outputSize);

	strcpy(pInput, "SN,RN4871-0790\r\n");
	inputSize = strlen("SN,RN4871-0790\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	strcpy(pInput, "SS,80\r\n");
	inputSize = strlen("SS,80\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	strcpy(pInput, "SS,C0\r\n");
	inputSize = strlen("SS,C0\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	strcpy(pInput, "PZ\r\n");
	inputSize = strlen("PZ\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	/* UUID correct */
	strcpy(pInput, "PS,000102030405060708090A0B0C0D0E0F\r\n");
	inputSize = strlen("PS,000102030405060708090A0B0C0D0E0F\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	/* UUID too long */
	strcpy(pInput, "PS,0000102030405060708090A0B0C0D0E0F\r\n");
	inputSize = strlen("PS,0000102030405060708090A0B0C0D0E0F\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* UUID too short */
	strcpy(pInput, "PS,102030405060708090A0B0C0D0E0F\r\n");
	inputSize = strlen("PS,102030405060708090A0B0C0D0E0F\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* UUID hexa incorrect */
	strcpy(pInput, "PS,000102030405060Z08090A0B0C0D0E0F\r\n");
	inputSize = strlen("PS,000102030405060Z08090A0B0C0D0E0F\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* too info */
	/*strcpy(pInput, "PS,000102030405060708090A0B0C0D0E0F,00\r\n");
	inputSize = strlen("PS,000102030405060708090A0B0C0D0E0F,00\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);*/

	/* UUID, prop, size correct */
	strcpy(pInput, "PC,00112233445566778899AABBCCDDEEFF,00,00\r\n");
	inputSize = strlen("PC,00112233445566778899AABBCCDDEEFF,00,00\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	/* UUID incorrect but prop, size correct */
	strcpy(pInput, "PC,ZZ112233445566778899AABBCCDDEEFF,00,00\r\n");
	inputSize = strlen("PC,ZZ112233445566778899AABBCCDDEEFF,00,00\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* UUID, size correct but prop incorrect */
	strcpy(pInput, "PC,00112233445566778899AABBCCDDEEFF,0000,00\r\n");
	inputSize = strlen("PC,00112233445566778899AABBCCDDEEFF,0000,00\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* UUID, size correct but prop incorrect */
	strcpy(pInput, "PC,00112233445566778899AABBCCDDEEFF,Z0,00\r\n");
	inputSize = strlen("PC,00112233445566778899AABBCCDDEEFF,Z0,00\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* UUID, prop correct but size incorrect */
	strcpy(pInput, "PC,00112233445566778899AABBCCDDEEFF,00,0000\r\n");
	inputSize = strlen("PC,00112233445566778899AABBCCDDEEFF,00,0000\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* UUID, size correct but prop incorrect */
	strcpy(pInput, "PC,00112233445566778899AABBCCDDEEFF,00,Z0\r\n");
	inputSize = strlen("PC,00112233445566778899AABBCCDDEEFF,00,Z0\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* Handle, value correct */
	strcpy(pInput, "SHW,1234,AB\r\n");
	inputSize = strlen("SHW,1234,AB\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("AOK\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("AOK\r\nCMD>"), outputSize);

	/* Handle incorrect and value correct */
	strcpy(pInput, "SHW,Z234,AB\r\n");
	inputSize = strlen("SHW,Z234,AB\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* Handle correct and value incorrect */
	strcpy(pInput, "SHW,1234,ZB\r\n");
	inputSize = strlen("SHW,1234,ZB\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* Handle incorrect and value correct */
	strcpy(pInput, "SHW,01234,AB\r\n");
	inputSize = strlen("SHW,01234,AB\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* Handle correct and value incorrect */
	strcpy(pInput, "SHW,1234,00AB\r\n");
	inputSize = strlen("SHW,1234,00AB\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* Handle correct */
	strcpy(pInput, "SHR,1234\r\n");
	inputSize = strlen("SHR,1234\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("12\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("12\r\nCMD>"), outputSize);

	/* Handle too long */
	strcpy(pInput, "SHR,001234\r\n");
	inputSize = strlen("SHR,001234\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* Handle too short */
	strcpy(pInput, "SHR,0\r\n");
	inputSize = strlen("SHR,0\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);

	/* Handle incorrect */
	strcpy(pInput, "SHR,123Z\r\n");
	inputSize = strlen("SHR,123Z\r\n");
	uartRxVirtualModule(pInput, inputSize);
	uartTxVirtualModule(pOutput, &outputSize);
	TEST_ASSERT_EQUAL_STRING("Err\r\nCMD>", pOutput);
	TEST_ASSERT_EQUAL_UINT16(strlen("Err\r\nCMD>"), outputSize);
}

void test_rn4871EnterCommandMode(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
}

void test_rn4871RebootModule(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871RebootModule(test_device));
}

void test_rn4871SetDeviceName(void) {
	char *deviceName = malloc(BUFFER_UART_LEN_MAX);
	int sizeDeviceName = snprintf(deviceName, BUFFER_UART_LEN_MAX, "test_device_name");
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetDeviceName(test_device, deviceName, (size_t)sizeDeviceName));
	TEST_ASSERT_EQUAL_STRING("test_device_name", deviceName);
	free(deviceName);
}

void test_rn4871GetDeviceName(void) {
	char *deviceName = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetDeviceName(test_device, deviceName));
	TEST_ASSERT_EQUAL_STRING("RN4871-0790", deviceName);
	free(deviceName);
}

void test_rn4871GetFirmwareVersion(void) {
	char *firmwareVersion = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetFirmwareVersion(test_device, firmwareVersion));
	TEST_ASSERT_EQUAL_STRING("V1.40", firmwareVersion);
	free(firmwareVersion);
}

void test_rn4871SetServices(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION | UART_TRANSPARENT));
}

void test_rn4871EraseAllGattServices(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EraseAllGattServices(test_device));
}

void test_rn4871TransparentUartSendData(void) {
	char *data = malloc(sizeof(char)*(BUFFER_LEN_MAX+1));
	int dataLen = snprintf(data, BUFFER_LEN_MAX, "Test data to send with transparent UART");

	/*
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_STREAMING, rn4871TransparentUartSendData(test_device, data, dataLen));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_STREAMING, rn4871TransparentUartSendData(test_device, data, dataLen));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_STREAMING, rn4871TransparentUartSendData(test_device, data, dataLen));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_NO_STREAMING, rn4871TransparentUartSendData(test_device, data, dataLen));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871TransparentUartSendData(test_device, data, dataLen));
	*/

	free(data);
}

void test_rn4871GetFsmState(void) {
	TEST_ASSERT_EQUAL(FSM_STATE_NONE, rn4871GetFsmState());

	char connected[] = "\%CONNECT,0,AABBCCDDEEFF\%";
	uint16_t size = strlen(connected);
	test_device->uartRx(connected, &size);
	//virtualModuleConnect(test_device);
	TEST_ASSERT_EQUAL(FSM_STATE_CONNECTED, rn4871GetFsmState());

	//virtualModuleStream(test_device);
	//TEST_ASSERT_EQUAL(FSM_STATE_STREAMING, rn4871GetFsmState());

	//virtualModuleDisconnect(test_device);
	//TEST_ASSERT_EQUAL(FSM_STATE_IDLE, rn4871GetFsmState());
}

void test_transparentUartMode(void) {
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
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871TransparentUartSendData(test_device, data, dataLen));
	free(buffer);
	free(data);
}

void test_gattMode(void) {
	char *buffer = malloc(BUFFER_UART_LEN_MAX);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EnterCommandMode(test_device));
	char deviceName[] = "test_gatt_mode";
	int sizeDeviceName = strlen(deviceName);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetDeviceName(test_device, deviceName, sizeDeviceName));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetDeviceName(test_device, buffer));
	TEST_ASSERT_EQUAL_STRING(deviceName, buffer);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871GetFirmwareVersion(test_device, buffer));
	TEST_ASSERT_EQUAL_STRING("V1.40", buffer);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetServices(test_device, DEVICE_INFORMATION));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871EraseAllGattServices(test_device));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871RebootModule(test_device));
	free(buffer);
}

int main(void) {
	UNITY_BEGIN();
	//RUN_TEST(test_virtualModule);
	//RUN_TEST(test_rn4871EnterCommandMode);
	//RUN_TEST(test_rn4871RebootModule);
	//RUN_TEST(test_rn4871GetDeviceName);
	//RUN_TEST(test_rn4871GetFirmwareVersion);
	//RUN_TEST(test_rn4871SetServices);
	//RUN_TEST(test_rn4871SetDeviceName);
	//RUN_TEST(test_rn4871TransparentUartSendData);
	RUN_TEST(test_rn4871GetFsmState);
	//RUN_TEST(test_transparentUartMode);
	//RUN_TEST(test_gattMode);
	return UNITY_END();
}