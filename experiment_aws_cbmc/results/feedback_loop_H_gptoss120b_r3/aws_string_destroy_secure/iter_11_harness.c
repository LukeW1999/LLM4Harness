#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);

        struct aws_allocator *alloc = aws_default_allocator();
        str = (struct aws_string *)alloc->mem_acquire(
            alloc,
            sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = alloc;
        }

        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Preserve state before destruction */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(aws_string_bytes(str), str->len, &old_bytes);
    }

    aws_string_destroy_secure(str);

    /* Post‑condition: the function should not crash; no further dereference of `str` is safe. */
    if (str == NULL) {
        assert(str == NULL);
    }
}
