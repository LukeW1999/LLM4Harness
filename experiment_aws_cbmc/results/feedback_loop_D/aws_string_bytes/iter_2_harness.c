#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness() {
    struct aws_string str;
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)nondet_ptr();

    // Save old state
    struct aws_string old_str = str;

    const uint8_t *result = aws_string_bytes(&str);

    // Assert frame conditions
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(result == old_str.bytes);

    // Assert postconditions
    if (result != NULL) {
        assert(result == str.bytes);
    }

    // Assert validity invariants
    assert(aws_string_is_valid(&str));
}
