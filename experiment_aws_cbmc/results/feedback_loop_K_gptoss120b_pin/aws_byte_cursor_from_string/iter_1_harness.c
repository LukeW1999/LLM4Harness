#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum length for the nondeterministically created string */
#define MAX_STRING_LEN 256

/* Helper to create a nondeterministic aws_string (or NULL) */
static struct aws_string *make_nondet_aws_string(void) {
    struct aws_string *s;
    bool is_null = __CPROVER_nondet_bool();

    if (is_null) {
        return NULL;
    }

    /* Allocate memory for the struct plus flexible array */
    size_t len = __CPROVER_nondet_uint();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate enough space: sizeof(struct aws_string) + (len - 1) extra bytes */
    s = (struct aws_string *)malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
    __CPROVER_assume(s != NULL);

    /* Set allocator to NULL to avoid deallocation side‑effects */
    s->allocator = NULL;

    /* Set length */
    *((size_t *)&s->len) = len;   /* cast away const for initialization */

    /* Initialize the byte array with nondeterministic data */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)s->bytes)[i] = __CPROVER_nondet_uchar();
    }

    return s;
}

/* Helper to compare two aws_string objects for equality of all fields */
static bool aws_string_equal(const struct aws_string *a, const struct aws_string *b) {
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }
    if (a->len != b->len) {
        return false;
    }
    for (size_t i = 0; i < a->len; ++i) {
        if (a->bytes[i] != b->bytes[i]) {
            return false;
        }
    }
    return true;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Create a nondeterministic source string (or NULL) */
    struct aws_string *src = make_nondet_aws_string();

    /* Preserve a copy of src for frame checking (if non‑NULL) */
    struct aws_string *src_copy = NULL;
    if (src != NULL) {
        size_t len = src->len;
        src_copy = (struct aws_string *)malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(src_copy != NULL);
        src_copy->allocator = src->allocator;
        *((size_t *)&src_copy->len) = len;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)src_copy->bytes)[i] = src->bytes[i];
        }
    }

    /* Call the function under verification */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Postcondition checks */
    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* The cursor should point to the string's byte array */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* Frame condition: src must be unchanged */
    if (src != NULL) {
        assert(aws_string_equal(src, src_copy));
    }

    /* Clean up */
    free(src);
    free(src_copy);
    return 0;
}
