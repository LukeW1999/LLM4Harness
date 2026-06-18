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
    char *c_str = NULL;

    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    if (!str_is_null) {
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;
        str->len = nondet_size_t();
        __CPROVER_assume(str->len < MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!c_str_is_null) {
        c_str = malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(c_str != NULL);
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
        c_str[c_str_len] = '\0';
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == true || result == false);
        if (str->len == 0) {
            assert(result == (c_str[0] == '\0'));
        }
    }

    free(str);
    free(c_str);
}
