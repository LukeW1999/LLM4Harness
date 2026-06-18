#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        str = ensure_string_is_allocated();
    }

    char *c_str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
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
