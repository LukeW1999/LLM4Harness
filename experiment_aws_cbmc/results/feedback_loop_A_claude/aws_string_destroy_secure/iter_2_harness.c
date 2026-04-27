#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = ensure_string_is_allocated_nondet_length();

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    aws_string_destroy_secure(str);
}
