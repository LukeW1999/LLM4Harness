#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

// Mock allocator functions
size_t deallocation_count = 0;
void *last_deallocated_ptr = NULL;

void mock_deallocate(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    last_deallocated_ptr = ptr;
    deallocation_count++;
}

struct aws_allocator mock_allocator = {
    .mem_acquire = malloc,
    .mem_release = mock_deallocate,
    .mem_realloc = realloc,
    .mem_calloc = calloc,
};

void aws_string_destroy_secure_harness() {
    /* parameters */
    struct aws_string *str;

    /* allocate and initialize str */
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);
    str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    str->allocator = &mock_allocator;
    str->len = len;
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = 0; // null terminator

    /* save old state */
    struct aws_string old = *str;

    /* operation under verification */
    aws_string_destroy_secure(str);

    /* assertions */
    // Check that the data bytes were zeroed out
    for (size_t i = 0; i < old.len; i++) {
        assert(str->bytes[i] == 0);
    }

    // Assert that the deallocation count is 1 and the last deallocated pointer is the original str
    assert(deallocation_count == 1);
    assert(last_deallocated_ptr == (void *)str);
}
