#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        /* allocate a buffer large enough for the string structure plus payload */
        static unsigned char storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;

        size_t extra_len = (size_t)nondet_uint64_t();
        __CPROVER_assume(extra_len <= MAX_BUFFER_SIZE);

        str->len = extra_len;

        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* ensure a null terminator exists within the allocated buffer */
        if (extra_len < MAX_BUFFER_SIZE) {
            ((char *)str->bytes)[extra_len] = '\0';
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
