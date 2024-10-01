#include "unity/unity.h"
#include "../twc.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_vdrop(void)
{
    ip_t ip = {.current.val = 1};
    double resistance = 1.0;
    TEST_ASSERT_EQUAL(1, calc_vdrop(&ip, &resistance));
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_vdrop);
    return UNITY_END();
}
