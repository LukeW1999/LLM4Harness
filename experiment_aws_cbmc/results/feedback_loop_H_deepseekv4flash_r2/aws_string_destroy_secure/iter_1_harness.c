#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministic choice: NULL or a valid string */
    struct aws_string *str = nondet_bool() ? NULL : malloc(sizeof(struct aws_string));
    struct aws_allocator *old_allocator = NULL;
    bool is_null = (str == NULL);

    if (str != NULL) {
        /* For a valid aws_string, we need contiguous memory: header + bytes.
         * We simulate that by allocating a larger block and casting.
         */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);  /* MAX_STRING_LEN is defined in the proof environment */
        struct aws_string *full = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(full != NULL);
        /* Initialize the const fields via cast to non-const pointers */
        *((struct aws_allocator **)&full->allocator) = nondet_bool() ? NULL : aws_default_allocator();
        *((size_t *)&full->len) = len;
        /* Fill the bytes with non-deterministic but valid data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)full->bytes)[i] = nondet_uint8_t();
        }
        /* The null-terminator is at offset len, but we don't need to set it because
         * aws_string_is_valid only checks that len bytes are readable.
         */
        __CPROVER_assume(aws_string_is_valid(full));
        str = full;
    }

    /* Save relevant state before the call */
    if (str != NULL) {
        old_allocator = str->allocator;
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postconditions:
     * 1. If the input was NULL, nothing changes.
     * 2. If the input was non-null and its allocator was NULL, the string is still valid.
     * 3. If the input was non-null and its allocator was non-null, the memory is freed.
     *    We cannot assert validity, but we assert that the function does not crash.
     *    CBMC will verify memory safety automatically.
     */
    if (is_null) {
        /* Nothing to assert, but we can assert that the function does not crash (implicit) */
        assert(1);
    } else {
        if (old_allocator == NULL) {
            /* Allocator was NULL, so the string was statically allocated; it must remain valid */
            assert(aws_string_is_valid(str));
        }
        /* else: dynamic memory was freed; no further assertions possible */
    }
}
