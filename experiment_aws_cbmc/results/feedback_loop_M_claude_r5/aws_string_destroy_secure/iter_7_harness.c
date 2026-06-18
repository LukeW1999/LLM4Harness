#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void harness(void) {
    struct aws_string *str = nondet_bool() ? ensure_string_is_allocated_bounded_length(16) : NULL;

    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    aws_string_destroy_secure(str);
}
