// === STEP 1: SUCCESS PATH ===
// aws_string_eq_c_str returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true:
//   - str: UNCHANGED (read-only comparison)
//   - c_str: UNCHANGED (read-only comparison)
//
// === STEP 2: FAILURE PATH ===
// When it returns false:
//   - str: UNCHANGED
//   - c_str: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string *):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// c_str (const char *):
//   - contents: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function handles NULL inputs gracefully (returns true if both NULL, false if one is NULL)
// If str is non-NULL, aws_string_is_valid(str) should hold before and after
// The function is purely a comparison - no mutations occur

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Stub for aws_array_eq_c_str if needed */

void aws_string_eq_c_str_harness(void) {
    /* Non-deterministically choose whether str and c_str are NULL */
    struct aws_string *str = NULL;
    const char *c_str = NULL;

    /* Bound the string length for tractability */
    size_t str_len;
    __CPROVER_assume(str_len <= 4);

    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate aws_string with bounded length */
        /* aws_string has a flexible array member: allocator, len, bytes[1] */
        /* We need to allocate sizeof(struct aws_string) + str_len bytes */
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);

        /* Initialize the fields - cast away const for initialization */
        *(struct aws_allocator **)&str->allocator = NULL; /* or nondet allocator */
        *(size_t *)&str->len = str_len;
        /* bytes are nondet (already nondet from malloc) */
        /* Ensure null terminator after bytes */
        ((uint8_t *)str->bytes)[str_len] = 0;
    }

    if (!c_str_is_null) {
        /* Allocate a c_str with bounded length */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= 4);
        char *c_str_buf = malloc(c_str_len + 1);
        __CPROVER_assume(c_str_buf != NULL);
        /* Ensure null terminator */
        c_str_buf[c_str_len] = '\0';
        c_str = c_str_buf;
    }

    /* Save old state */
    struct aws_string old_str_val;
    size_t old_len = 0;
    uint8_t old_bytes[5] = {0};
    
    if (str != NULL) {
        old_len = str->len;
        /* Save bytes */
        for (size_t i = 0; i <= str->len && i < 5; i++) {
            old_bytes[i] = str->bytes[i];
        }
    }

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* === ASSERTIONS === */

    /* Both NULL => true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* One NULL, other not => false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* Frame conditions: str is unchanged */
    if (str != NULL) {
        assert(str->len == old_len);
        /* Verify bytes are unchanged */
        for (size_t i = 0; i <= str->len && i < 5; i++) {
            assert(str->bytes[i] == old_bytes[i]);
        }
        /* Validity invariant */
        assert(aws_string_is_valid(str));
    }
}
