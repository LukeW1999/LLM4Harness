#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = ensure_string_is_allocated();
    char *c_str = ensure_c_str_is_allocated();

    bool result = aws_string_eq_c_str(str, c_str);

    assert(result == true || result == false);
}
