#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = NULL;
    if (__CPROVER_nondet_bool()) {
        str = ensure_string_is_allocated();
    }

    char *c_str = NULL;
    if (__CPROVER_nondet_bool()) {
        c_str = ensure_c_str_is_allocated();
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == true || result == false);
    }
}
