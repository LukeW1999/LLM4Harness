#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = ensure_string_is_allocated();
    char *c_str = ensure_c_str_is_allocated(1024);
    aws_string_eq_c_str(str, c_str);
}
