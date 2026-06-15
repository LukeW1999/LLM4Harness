#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

#define MAX_STRING_SIZE 10

struct aws_string *allocate_string() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_SIZE);
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    str->len = len;
    str->bytes = (uint8_t *)(str + 1);
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = '\0';
    __CPROVER_assume(aws_string_is_valid(str));
    return str;
}

void aws_string_eq_harness() {
    struct aws_string *a = nondet_bool() ? NULL : allocate_string();
    struct aws_string *b = nondet_bool() ? NULL : allocate_string();

    bool result = aws_string_eq(a, b);

    // Postconditions
    if (a == NULL && b == NULL) {
        __CPROVER_assert(result == true, "both NULL => true");
    } else if (a == NULL || b == NULL) {
        __CPROVER_assert(result == false, "one NULL => false");
    } else {
        // Both non-NULL
        if (a->len != b->len) {
            __CPROVER_assert(result == false, "different lengths => false");
        } else {
            if (result) {
                for (size_t i = 0; i < a->len; i++) {
                    __CPROVER_assert(a->bytes[i] == b->bytes[i], "result true => bytes equal");
                }
            }
        }
        // Strings remain valid
        __CPROVER_assert(aws_string_is_valid(a), "a is valid");
        __CPROVER_assert(aws_string_is_valid(b), "b is valid");
    }
}
