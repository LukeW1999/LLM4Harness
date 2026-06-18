#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = ensure_string_is_allocated();
    char c_str[1024];
    c_str[sizeof(c_str) - 1] = '\0';
    aws_string_eq_c_str(str, c_str);
}
