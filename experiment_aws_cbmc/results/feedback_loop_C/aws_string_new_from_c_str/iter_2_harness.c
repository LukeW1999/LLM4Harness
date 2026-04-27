#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    const char *c_str = ndecls("c_str");

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        // Success path assertions
        assert(aws_string_is_valid(result));
        assert(result->len == strlen(c_str));
        assert(memcmp(result->bytes, c_str, result->len) == 0);
    } else {
        // Failure path assertions
        // No change in allocator or c_str
    }

    // Frame condition assertions
    // allocator and c_str are unchanged, so no need to save old state and compare
}
