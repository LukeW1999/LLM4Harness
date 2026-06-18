#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness() {
    struct aws_string *str = ensure_aws_string_is_allocated_nondet_length();
    aws_string_destroy_secure(str);
}
