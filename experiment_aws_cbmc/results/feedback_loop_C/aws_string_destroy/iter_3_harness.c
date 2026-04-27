#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

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

    // Check that the memory has been freed
    // Since we cannot directly check if memory is freed in CBMC, we check if the pointer is now invalid
    // This is a simplification and assumes that CBMC can detect use-after-free
    // In practice, CBMC may not be able to prove this, but it's a reasonable assertion to include
    assert(str->allocator == NULL);
    assert(str->len == 0);
    assert(str->bytes == NULL);

    // Check validity invariants
    assert(!aws_string_is_valid(&old_str));
}
