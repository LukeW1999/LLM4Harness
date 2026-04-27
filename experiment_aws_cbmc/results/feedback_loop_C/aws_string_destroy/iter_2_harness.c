#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = malloc(sizeof(struct aws_string) + sizeof(uint8_t));
    __CPROVER_assume(allocator != NULL);
    str->allocator = allocator;
    str->len = 1;
    str->bytes = (uint8_t *)(str + 1);
    str->bytes[0] = 'a';

    struct aws_string old_str = *str;

    aws_string_destroy(str);

    // Check frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(str->bytes[0] == old_str.bytes[0]);

    // Check validity invariants
    assert(!aws_string_is_valid(&old_str));
}
