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
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len < 1024);
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[len] = '\0';
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == true || result == false);
    }
}
