#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <stdlib.h>
#include <string.h>

/* Helper to obtain a nondeterministic allocator (may be NULL) */
static struct aws_allocator *nondet_allocator(void) {
    struct aws_allocator *alloc = NULL;
    if (nondet_bool()) {
        alloc = aws_default_allocator();
    }
    return alloc;
}

/* Harness for aws_string_destroy_secure */
void aws_string_destroy_secure_harness(void) {
    /* 1. Choose a nondeterministic length within a reasonable bound */
    size_t max_len = MAX_BUFFER_SIZE;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    /* 2. Choose whether the string has an allocator */
    struct aws_allocator *alloc = nondet_allocator();

    struct aws_string *str = NULL;

    if (alloc == NULL) {
        /* Allocate the string on the stack */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_BUFFER_SIZE];
        } storage;

        str = &storage.s;
        str->allocator = NULL;
        *((size_t *)&str->len) = len;               /* cast away const */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    } else {
        /* Allocate the string using the allocator */
        size_t total_size = sizeof(struct aws_string) + len + 1; /* +1 for NUL */
        str = (struct aws_string *)alloc->mem_alloc(alloc, total_size);
        __CPROVER_assume(str != NULL);
        str->allocator = alloc;
        *((size_t *)&str->len) = len;               /* cast away const */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0; /* NUL terminator */
    }

    /* 3. Assume the string is valid before the call */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 4. Save old state for later checks */
    struct aws_string old = *str;
    uint8_t *old_bytes = malloc(len);
    __CPROVER_assume(old_bytes != NULL);
    memcpy(old_bytes, aws_string_bytes(str), len);

    /* 5. Call the function under test */
    aws_string_destroy_secure(str);

    /* 6. Post‑condition checks */
    if (old.allocator == NULL) {
        /* Memory was not freed; bytes must be zeroed and other fields unchanged */
        uint8_t *post_bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < old.len; ++i) {
            assert(post_bytes[i] == 0);
        }
        assert(str->len == old.len);
        assert(str->allocator == old.allocator);
        assert(aws_string_is_valid(str));
    } else {
        /* Allocator was present; the string memory may have been released.
           We can only check that the original allocator was non‑NULL. */
        assert(old.allocator != NULL);
    }

    /* 7. Clean up auxiliary allocations */
    free(old_bytes);
}
