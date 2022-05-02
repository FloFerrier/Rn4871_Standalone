#include "unity.h"
#include "rn4871-driver/rn4871.h" /* The unit to be tested. */
#include "rn4871-driver/virtual_module.h" /* The unit to be tested. */

#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_LEN_MAX (255)

uint8_t rn4871UartTxAPI(uint8_t *pBuffer, uint16_t *bufferSize);
uint8_t rn4871UartRxAPI(uint8_t *pBuffer, uint16_t *bufferSize);
void rn4871DelayMsAPI(uint32_t delay);

uint8_t rn4871UartTxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
	/* Check input buffer */
    if(NULL == pBuffer || NULL == bufferSize)
		return CODE_RETURN_UART_FAIL;

	printf("[TX:%d] %s\r\n", *bufferSize, pBuffer);

	if(VIRTUAL_MODULE)
		uartRxVirtualModule(pBuffer, *bufferSize);
	else
		printf("Real module : To do !\r\n");

    return CODE_RETURN_SUCCESS;
}

uint8_t rn4871UartRxAPI(uint8_t *pBuffer, uint16_t *bufferSize) {
	/* Check input buffer */
    if(NULL == pBuffer || NULL == bufferSize)
		return CODE_RETURN_UART_FAIL;

	if(VIRTUAL_MODULE)
		uartTxVirtualModule(pBuffer, bufferSize);
	else
		printf("Real module : To do !\r\n");

	printf("[RX:%d] %s\r\n", *bufferSize, pBuffer);

    return CODE_RETURN_SUCCESS;
}

void rn4871DelayMsAPI(uint32_t delay) {
	usleep(delay*1000);
}

/* Is run before every test, put unit init calls here. */
void setUp(void) {

}

/* Is run after every test, put unit clean-up calls here. */
void tearDown(void) {

}

void test_rn4871SendCmd(void) {
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_ERROR, rn4871SendCmd(NULL, CMD_NONE, NULL));
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
		._current_cmd = CMD_NONE,
		.fsm_state = FSM_STATE_INIT,
		.transparentUart = false,
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_CMD_UNKNOWN, rn4871SendCmd(&dev, CMD_NONE, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_MODE_ENTER, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_MODE_QUIT, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_REBOOT, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_GET_DEVICE_NAME, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_DUMP_INFOS, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_GET_VERSION, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_CLEAR_ALL_SERVICES, NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_RESET_FACTORY, "%d", 1));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_SET_BT_NAME, "test"));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_SET_DEVICE_NAME, "test"));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_SET_SERVICES, "%X", 0xC0));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_CREATE_PRIVATE_SERVICE, "0102030405060708090A0B0C0D0E0F"));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_CREATE_PRIVATE_CHARACTERISTIC, "112233445566778899AABBCCDDEEFF,%X,%X", 0x1A, 0x0B));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_SERVER_READ_CHARACTERISTIC, "%X", 0x0000));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SendCmd(&dev, CMD_SERVER_WRITE_CHARACTERISTIC, "%X,%X", 0x0000, 0x00));
}

void test_rn4871ResponseProcess(void) {
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
		._current_cmd = CMD_NONE,
		.fsm_state = FSM_STATE_INIT,
		.transparentUart = false,
	};
	TEST_ASSERT_EQUAL_INT(FSM_STATE_INIT, dev.fsm_state);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_ERROR, rn4871ResponseProcess(&dev, NULL, 0));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_ERROR, rn4871ResponseProcess(&dev, "Err\r\nCMD>", strlen("Err\r\nCMD>")));
	rn4871SendCmd(&dev, CMD_MODE_ENTER, NULL);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871ResponseProcess(&dev, "AOK\r\nCMD>", strlen("AOK\r\nCMD>")));
	rn4871SendCmd(&dev, CMD_GET_DEVICE_NAME, NULL);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871ResponseProcess(&dev, "RN4871-0790\r\nCMD>", strlen("RN4871-0790\r\nCMD>")));
	rn4871SendCmd(&dev, CMD_GET_VERSION, NULL);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871ResponseProcess(&dev, "V1.40 7/9/2019 (c)Microship Technology Inc\r\nCMD>", strlen("V1.40 7/9/2019 (c)Microship Technology Inc\r\nCMD>")));
	rn4871SendCmd(&dev, CMD_REBOOT, NULL);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871ResponseProcess(&dev, "Rebooting\r\n%REBOOT%", strlen("Rebooting\r\n%REBOOT%")));
	TEST_ASSERT_EQUAL_INT(FSM_STATE_IDLE, dev.fsm_state);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871ResponseProcess(&dev, "%CONNECTED%", strlen("%CONNECTED%")));
	TEST_ASSERT_EQUAL_INT(FSM_STATE_CONNECTED, dev.fsm_state);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871ResponseProcess(&dev, "%STREAM_OPEN%", strlen("%STREAM_OPEN%")));
	TEST_ASSERT_EQUAL_INT(FSM_STATE_STREAMING, dev.fsm_state);
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871ResponseProcess(&dev, "%DISCONNECT%", strlen("%DISCONNECT%")));
	TEST_ASSERT_EQUAL_INT(FSM_STATE_IDLE, dev.fsm_state);
}

void test_virtualModule(void) {
	uartRxVirtualModule(NULL, 0);
	uartTxVirtualModule(NULL, NULL);

	uint8_t pInput[256] = "";
	uint16_t inputSize = 0;
	uint8_t pOutput[256] = "";
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

void test_rn4871SetConfig(void) {
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
		._current_cmd = CMD_NONE,
		.fsm_state = FSM_STATE_INIT,
		.transparentUart = true,
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_ERROR, rn4871SetConfig(NULL));
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetConfig(&dev));
	TEST_ASSERT_EQUAL_INT(FSM_STATE_IDLE, dev.fsm_state);
}

void test_rn4871TransparentUartSendData(void) {
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
		._current_cmd = CMD_NONE,
		.fsm_state = FSM_STATE_INIT,
		.transparentUart = true,
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871SetConfig(&dev));
	TEST_ASSERT_EQUAL_INT(FSM_STATE_IDLE, dev.fsm_state);
	dev.fsm_state = FSM_STATE_STREAMING;
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, rn4871TransparentUartSendData(&dev, "30313233343536373839404142", strlen("30313233343536373839404142")));
}

void test_gattProfile(void) {
	struct char_param_s char1 = {
    	.uuid = "00112233445566778899AABBCCDDEEFF",
    	.properties = 0x00,
    	.size = 0x00,
    	.value = "0",
	};

	struct service_param_s service = {
    	.uuid = "000102030405060708090A0B0C0D0E0F",
    	.char_nb = 1,
    	.pChar[0] = &char1,
	};

	struct profile_param_s profile = {
		.service_nb = 1,
    	.pService[0] = &service,
	};
	TEST_ASSERT_EQUAL_STRING("000102030405060708090A0B0C0D0E0F", profile.pService[0]->uuid);
	TEST_ASSERT_EQUAL_STRING("00112233445566778899AABBCCDDEEFF", profile.pService[0]->pChar[0]->uuid);
	TEST_ASSERT_EQUAL_UINT8(0x00, profile.pService[0]->pChar[0]->properties);
	profile.pService[0]->pChar[0]->properties = CHAR_PROP_WRITE | CHAR_PROP_NOTIFY;
	TEST_ASSERT_EQUAL_UINT8(0x18, profile.pService[0]->pChar[0]->properties);
}

void test_gattServices(void) {
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
		._current_cmd = CMD_NONE,
		.fsm_state = FSM_STATE_INIT,
		.transparentUart = false,
	};

	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_ERROR, _createCustomService(NULL, NULL));

	struct service_param_s service_uuid_too_short = {
    	.uuid = "0102",
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_UUID_INCORRECT, _createCustomService(&dev, &service_uuid_too_short));

	struct service_param_s service_uuid_with_incorrect_char = {
    	.uuid = "$00102030405060708090A0B0C0D0E0F",
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_UUID_INCORRECT, _createCustomService(&dev, &service_uuid_with_incorrect_char));

	struct service_param_s service1 = {
    	.uuid = "000102030405060708090A0B0C0D0E0F",
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, _createCustomService(&dev, &service1));
}

void test_gattCharacteristics(void) {
	struct rn4871_dev_s dev = {
		.uartTx = rn4871UartTxAPI,
		.uartRx = rn4871UartRxAPI,
		.delayMs = rn4871DelayMsAPI,
		._current_cmd = CMD_NONE,
		.fsm_state = FSM_STATE_INIT,
		.transparentUart = false,
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_ERROR, _createCustomChar(NULL, NULL));

	struct char_param_s char1_uuid_too_long = {
    	.uuid = "000112233445566778899AABBCCDDEEFF",
    	.properties = 0x00,
    	.size = 0x00,
    	.value = "00",
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_UUID_INCORRECT, _createCustomChar(&dev, &char1_uuid_too_long));

	struct char_param_s char1 = {
    	.uuid = "00112233445566778899AABBCCDDEEFF",
    	.properties = CHAR_PROP_READ | CHAR_PROP_WRITE,
    	.size = 0x01,
    	.value = "12",
	};
	TEST_ASSERT_EQUAL_UINT8(CODE_RETURN_SUCCESS, _createCustomChar(&dev, &char1));
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_rn4871SendCmd);
	RUN_TEST(test_rn4871ResponseProcess);
	RUN_TEST(test_virtualModule);
	RUN_TEST(test_rn4871SetConfig);
	RUN_TEST(test_rn4871TransparentUartSendData);
	RUN_TEST(test_gattProfile);
	RUN_TEST(test_gattServices);
	RUN_TEST(test_gattCharacteristics);
	return UNITY_END();
}