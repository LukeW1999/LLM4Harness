#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

#define AWS_OP_SUCCESS 0

int aws_round_up_to_power_of_two(size_t n, size_t *result);
bool aws_is_power_of_two(size_t n);

void aws_round_up_to_power_of_two_harness(void) {
    size_t n;
    __CPROVER_assume(n <= 1024);

    size_t result = 0;
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
            assert(aws_is_power_of_two(result));
            assert((result >> 1) < n);
        }
    } else {
        assert(result == old_result);
    }
}
