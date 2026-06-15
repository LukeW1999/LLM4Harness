#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Dummy allocator that does nothing on release */
static void dummy_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    (void)ptr;
}
static struct aws_allocator dummy_allocator = {
    .mem_acquire = NULL,
    .mem_release = dummy_release,
    .mem_realloc = NULL,
    .mem_destroy = NULL,
    .mem_get_name = NULL
};

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Stack‑allocated storage for the string and its bytes */
        struct {
            struct aws_string base;
            uint8_t bytes[MAX_BUFFER_SIZE];
        } storage;

        str = &storage.base;

        if (nondet_bool()) {
            str->allocator = &dummy_allocator;
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            storage.bytes[i] = nondet_uint8_t();
        }
    }

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    aws_string_destroy(str);

    if (old_str == NULL) {
        assert(str == NULL);
    } else if (old_allocator == NULL) {
        assert(str == old_str);
        assert(str->allocator == NULL);
        assert(str->len == old_len);
        assert(aws_string_is_valid(str));
    } else {
        assert(str == old_str);
        /* No further dereferencing after destroy with a non‑NULL allocator */
    }
}
