#include <proof_helpers/make_common_data_structures.h>

struct aws_allocator *aws_default_allocator(void);
bool aws_string_is_valid(const struct aws_string *str);
void aws_string_destroy(struct aws_string *str);

void aws_string_destroy_harness(void) {
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        const size_t max_len = 16;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        unsigned char storage[sizeof(struct aws_string) + max_len];
        str = (struct aws_string *)storage;

        str->len = len;

        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    aws_string_destroy(str);

    assert(str == old_str);

    if (str == NULL || old_allocator == NULL) {
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            assert(aws_string_is_valid(str));
        }
    } else {
        /* allocator was non‑NULL; memory may have been released. No further checks. */
    }
}
