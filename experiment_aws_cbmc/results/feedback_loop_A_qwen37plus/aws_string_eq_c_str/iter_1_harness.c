#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = NULL;
    if (!nondet_bool()) {
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[MAX_BUFFER_SIZE + 1];
        } *tmp = malloc(sizeof(*tmp));
        __CPROVER_assume(tmp != NULL);
        tmp->allocator = NULL;
        tmp->len = nondet_size_t();
        __CPROVER_assume(tmp->len <= MAX_BUFFER_SIZE);
        tmp->bytes[tmp->len] = '\0';
        str = (struct aws_string *)tmp;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    char *c_str = NULL;
    if (!nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == true || result == false);
    }
}
