#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    struct aws_string *str = nondet_allocate_string();
    __CPROVER_assume(aws_string_is_valid(str));
    aws_string_destroy(str);
}
