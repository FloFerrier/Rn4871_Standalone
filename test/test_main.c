#include "unity.h"

/* Is run before every test, put unit init calls here. */
void setUp (void) {

}

/* Is run after every test, put unit clean-up calls here. */
void tearDown (void) {

}

void emptyTest(void) {
	TEST_ASSERT_EQUAL_INT(0, 0);
}

int main (void) {
	UNITY_BEGIN();
	RUN_TEST(emptyTest); /* Run the test. */
	return UNITY_END();
}
