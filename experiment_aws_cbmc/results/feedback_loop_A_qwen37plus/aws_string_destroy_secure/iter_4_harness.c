#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        str = ensure_string_is_valid();
        __CPROVER_assume(aws_string_is_valid(str));
    }
    aws_string_destroy_secure(str);
}
