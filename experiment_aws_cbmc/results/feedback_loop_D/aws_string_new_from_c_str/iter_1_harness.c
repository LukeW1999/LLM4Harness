// === STEP 1: SUCCESS PATH ===
// When aws_string_new_from_c_str returns a non-NULL value:
//   - The returned struct aws_string* points to a valid aws_string with allocator set to allocator, len set to strlen(c_str), and bytes set to c_str

// === STEP 2: FAILURE PATH ===
// When aws_string_new_from_c_str returns NULL:
//   - The allocator remains unchanged
//   - The c_str remains unchanged

// === STEP 3: FRAME CONDITIONS ===
// allocator (struct aws_allocator*):
//   - allocator: UNCHANGED always
// c_str (const char*):
//   - c_str: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(returned_value): YES (if non-NULL)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    const char *c_str = (const char *)malloc(sizeof(char) * 100); // Arbitrary size for c_str
    __CPROVER_assume(c_str != NULL);

    struct aws_allocator old_allocator = *allocator;
    const char *old_c_str = c_str;

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        assert(result->allocator == allocator);
        assert(result->len == strlen(c_str));
        assert(memcmp(result->bytes, c_str, result->len) == 0);
        assert(aws_string_is_valid(result));
    } else {
        assert(*allocator == old_allocator);
        assert(c_str == old_c_str);
    }

    free((void *)c_str);
    free(allocator);
}
