#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        size_t alloc_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        *(size_t *)&(str->len) = len;
        *(struct aws_allocator **)&(str->allocator) = NULL;
    }

    char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
        c_str = (char *)malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[c_len] = '\0';
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }
}
