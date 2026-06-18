#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

extern bool nondet_bool(void);
extern uint64_t nondet_uint64_t(void);

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_harness(void) {
    bool have_str = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;

    if (have_str) {
        size_t len = nondet_uint64_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        size_t total_size = sizeof(struct aws_string) + len + 1;
        str = (struct aws_string *)aws_mem_acquire(allocator, total_size);
        if (str) {
            str->allocator = allocator;
            str->len = len;

            old_allocator = str->allocator;
            old_len = str->len;
        }
    }

    aws_string_destroy(str);

    if (!have_str) {
        assert(str == NULL);
    } else {
        assert(old_allocator != NULL);
        assert(old_len <= MAX_BUFFER_SIZE);
    }
}
