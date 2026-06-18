#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = NULL;
    char c_str_arr[MAX_BUFFER_SIZE];
    c_str_arr[MAX_BUFFER_SIZE - 1] = '\0';
    char *c_str = NULL;

    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    if (!str_is_null) {
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        __CPROVER_assume(str->len < MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!c_str_is_null) {
        c_str = c_str_arr;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == aws_array_eq_c_str(str->bytes, str->len, c_str));
    }
}
