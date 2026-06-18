#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    struct aws_allocator *allocator = aws_default_allocator();

    if (nondet_bool()) {
        size_t total_size = sizeof(struct aws_string) + MAX_BUFFER_SIZE;
        str = (struct aws_string *)allocator->mem_acquire(allocator, total_size);
        __CPROVER_assume(str != NULL);

        size_t extra_len = (size_t)nondet_uint64_t();
        __CPROVER_assume(extra_len <= MAX_BUFFER_SIZE);
        str->len = extra_len;
        str->allocator = allocator;

        if (extra_len < MAX_BUFFER_SIZE) {
            ((char *)str->bytes)[extra_len] = '\0';
        }
    }

    aws_string_destroy(str);
}
