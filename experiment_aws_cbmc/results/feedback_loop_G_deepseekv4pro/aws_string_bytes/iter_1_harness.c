#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STRING_LEN 64

void aws_string_bytes_harness() {
    /* 1. Create a valid aws_string with bounded length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Use NULL allocator to keep things simple; aws_string_is_valid allows it */
    struct aws_allocator *allocator = NULL;

    /* Total size: header + extra bytes beyond the initial byte to hold len+1 bytes */
    size_t total_size = sizeof(struct aws_string) + len;
    struct aws_string *str = malloc(total_size);
    __CPROVER_assume(str != NULL);

    /*
     * Set up the const fields via a non‑const look‑alike struct.
     * This bypasses the compiler's constness, which is acceptable in CBMC.
     */
    struct aws_string_init {
        struct aws_allocator *allocator;
        size_t len;
        uint8_t bytes[1];
    } init;
    init.allocator = allocator;
    init.len = len;
    memcpy(str, &init, sizeof(init));

    /* Fill the data bytes and the null terminator */
    uint8_t *bytes = (uint8_t *)((char *)str + offsetof(struct aws_string, bytes));
    for (size_t i = 0; i < len; i++) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[len] = 0; /* null terminator required by aws_string_is_valid */

    /* Now the string should be valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    struct aws_allocator *const old_allocator = str->allocator;
    const size_t old_len = str->len;
    uint8_t old_bytes[MAX_STRING_LEN];
    for (size_t i = 0; i < old_len; i++) {
        old_bytes[i] = bytes[i];
    }

    /* 3. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */

    /* The returned pointer must point to the bytes member of the string */
    assert(result == str->bytes);

    /* The string structure must remain unchanged (immutability) */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);

    /* The data bytes must remain unchanged */
    for (size_t i = 0; i < old_len; i++) {
        assert(bytes[i] == old_bytes[i]);
    }

    /* 5. Validity invariant must still hold */
    assert(aws_string_is_valid(str));
}
