#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void harness(void) {
    struct aws_string *str = nondet_allocate_string_bounded_length(16);

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    aws_string_destroy_secure(str);
}
