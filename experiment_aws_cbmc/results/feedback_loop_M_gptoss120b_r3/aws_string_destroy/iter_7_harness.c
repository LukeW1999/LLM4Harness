#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t extra_len = (size_t)nondet_uint64_t();
        __CPROVER_assume(extra_len <= MAX_BUFFER_SIZE);

        size_t total_size = sizeof(struct aws_string) + extra_len;
        struct aws_allocator *alloc = aws_default_allocator();
        str = (struct aws_string *)alloc->mem_acquire(alloc, total_size);
        __CPROVER_assume(str != NULL);

        str->len = extra_len;
        if (nondet_bool()) {
            str->allocator = alloc;
        } else {
            str->allocator = NULL;
        }
    }

    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;

    aws_string_destroy(str);

    if (old_allocator == NULL) {
        if (str != NULL) {
            assert(str->allocator == NULL);
            assert(str->len <= MAX_BUFFER_SIZE);
            assert(aws_string_is_valid(str));
        }
    }
}
