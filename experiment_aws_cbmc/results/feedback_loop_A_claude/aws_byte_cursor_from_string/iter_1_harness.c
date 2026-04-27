Looking at the function:

```c
aws_byte_cursor_from_string(const struct aws_string *src) {
    if (!src) {
        struct aws_byte_cursor cursor;
        AWS_ZERO_STRUCT(cursor);
        return cursor;
    }
    return aws_byte_cursor_from_array(aws_string_bytes(src), src->len);
}
```

**Analysis:**
1. **What changes on success?** Returns a `struct aws_byte_cursor` by value. If `src` is NULL, returns empty cursor (ptr=NULL, len=0). If `src` is valid, returns cursor pointing to `src->bytes` with `src->len`.
2. **Unchanged fields:** The input `src` string is not modified.
3. **Failure path:** No failure path (no error code), but NULL input returns empty cursor.
4. **Validity invariants:** The returned cursor should be valid. The input string should remain valid.

```c
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

/* Helper to allocate a valid aws_string non-deterministically */
struct aws_string *ensure_string_is_allocated(size_t max_len);

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or a valid string */
    bool src_is_null = nondet_bool();

    struct aws_string *src = NULL;

    if (!src_is_null) {
        /* Allocate a bounded aws_string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string header + len + 1 bytes */
        src = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(src != NULL);

        /* Initialize the string fields */
        /* We need to write to const fields - use a non-const pointer */
        struct aws_string *mutable_src = src;
        *(size_t *)(&mutable_src->len) = len;
        /* allocator can be NULL or non-NULL */
        *(struct aws_allocator **)(&mutable_src->allocator) = NULL;

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, result should be an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is valid, result should point to src->bytes with src->len */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);

        /* The input string must not have been modified */
        assert(aws_string_is_valid(src));

        /* The returned cursor should be valid */
        assert(aws_byte_cursor_is_valid(&result));
    }
}
```

Wait, I need to use the proper harness pattern with the available helpers. Let me rewrite properly:

```c
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness(void) {
    bool src_is_null = nondet_bool();
    struct aws_string *src = NULL;

    if (!src_is_null) {
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        src = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(src != NULL);

        *(size_t *)(&src->len) = len;
        *(struct aws_allocator **)(&src->allocator) = NULL;

        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_valid(&result));
    }
}
