#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

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

        unsigned char storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;

        /* Initialize fields */
        str->allocator = allocator;
        str->len = len;

        old_allocator = str->allocator;
        old_len = str->len;

        /* Use NULL allocator to avoid deallocation of stack memory */
        str->allocator = NULL;
    }

    aws_string_destroy(str);

    if (!have_str) {
        assert(str == NULL);
    }

    if (str != NULL && old_allocator == NULL) {
        assert(str->allocator == NULL);
        assert(str->len == old_len);
    }
}
