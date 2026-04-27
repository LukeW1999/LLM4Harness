#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t len = nd_size_t();
    char *c_str = can_fail_malloc(len + 1); // Allocate space for the string and null terminator

    if (c_str != NULL) {
        __CPROVER_assume(len <= MAX_STRING_LEN); // Assuming MAX_STRING_LEN is defined somewhere
        __CPROVER_assume(__CPROVER_forall {size_t i; (i < len) ==> (c_str[i] != '\0')});
        c_str[len] = '\0'; // Ensure null termination

        __CPROVER_assume(__CPROVER_forall {size_t i; (i < len) ==> (c_str[i] >= 32 && c_str[i] <= 126)}); // Printable characters

        struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

        if (result != NULL) {
            // Success path assertions
            assert(aws_string_is_valid(result));
            assert(result->len == len);
            assert(memcmp(result->bytes, c_str, result->len) == 0);
        }

        // Freeing the original c_str should not interfere with the aws_string's internal allocation
        free(c_str); // Clean up allocated memory
    }
}
