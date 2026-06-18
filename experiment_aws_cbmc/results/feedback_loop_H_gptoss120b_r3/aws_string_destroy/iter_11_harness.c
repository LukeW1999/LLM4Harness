#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        _Alignas(struct aws_string) uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)storage;

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str->len = len;

        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct {
        struct aws_allocator *allocator;
        size_t len;
    } old = {0};

    if (str != NULL) {
        old.allocator = str->allocator;
        old.len = str->len;
    }

    aws_string_destroy(str);

    if (str == NULL || old.allocator == NULL) {
        if (str != NULL) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            assert(aws_string_is_valid(str));
        }
    } else {
        assert(old.allocator != NULL);
    }

    if (str != NULL && old.allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
