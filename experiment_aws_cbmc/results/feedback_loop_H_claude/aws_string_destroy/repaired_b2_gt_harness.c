#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    struct aws_string *str = ensure_string_is_allocated_nondet_length();
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    aws_string_destroy(str);
}
