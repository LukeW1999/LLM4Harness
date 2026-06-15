#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 5

void aws_string_eq_c_str_harness() {
    // Non-deterministic nullness
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    const char *c_str = NULL;
    size_t c_len = 0; // length of c_str, including null terminator

    // Build str if non-null
    if (!str_is_null) {
        // Choose a non-deterministic length for the aws_string (bytes array)
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        // Allocate memory for the struct and the bytes array together
        struct aws_string *raw_str = malloc(sizeof(struct aws_string) + len);
        if (raw_str == NULL) return; // skip if malloc fails

        // Set fields
        raw_str->allocator = nondet_allocator(); // guaranteed non-null by helper
        raw_str->len = len;
        for (size_t i = 0; i < len; i++) {
            raw_str->bytes[i] = nondet_uint8_t();
        }

        // Cast to const for the function
        str = raw_str;

        // Assume the string is valid (passes standard validity check)
        __CPROVER_assume(aws_string_is_valid(str));
    }

    // Build c_str if non-null
    if (!c_str_is_null) {
        // Allocate a buffer for a null-terminated C string
        c_str = malloc(MAX_STRING_LEN + 1);
        if (c_str == NULL) return; // skip if malloc fails

        // Choose a non-deterministic length (excluding null terminator)
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        // Fill the buffer with non-deterministic characters
        for (size_t i = 0; i < c_len; i++) {
            ((char *)c_str)[i] = nondet_char();
        }
        // Null-terminate at position c_len
        ((char *)c_str)[c_len] = '\0';

        // Ensure that no earlier character is null (so the actual string length is exactly c_len)
        for (size_t i = 0; i < c_len; i++) {
            __CPROVER_assume(((char *)c_str)[i] != '\0');
        }

        // Assume the buffer is readable for c_len+1 bytes
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_len + 1));
    }

    // Save old state for immutability checks (only stored fields, not the content)
    struct aws_string *old_str = NULL;
    if (str != NULL) {
        old_str = malloc(sizeof(struct aws_string));
        if (old_str == NULL) return;
        // Deep copy of the struct (but not the bytes array because we compare separately)
        *old_str = *str;
    }

    // Call the function
    bool result = aws_string_eq_c_str(str, c_str);

    // Postcondition assertions

    // 1. If both are NULL, result is true
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    // 2. If exactly one is NULL, result is false
    if ((str == NULL) != (c_str == NULL)) {
        assert(result == false);
    }

    // 3. If both are non-NULL, result is true iff lengths and bytes match
    if (str != NULL && c_str != NULL) {
        // Compute expected result using str->len and known c_len, content equality
        bool content_equal = true;
        if (str->len != c_len) {
            content_equal = false;
        } else {
            for (size_t i = 0; i < str->len; i++) {
                if (str->bytes[i] != ((const uint8_t *)c_str)[i]) {
                    content_equal = false;
                    break;
                }
            }
        }
        assert(result == content_equal);
    }

    // 4. Unchanged fields: str (if non-NULL) remains unchanged
    if (str != NULL && old_str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        // Bytes array content should not change
        for (size_t i = 0; i < str->len; i++) {
            assert(str->bytes[i] == old_str->bytes[i]);
        }
    }

    // 5. Validity of str remains unchanged (if non-NULL)
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
