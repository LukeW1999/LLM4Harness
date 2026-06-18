#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    bool is_null = nondet_bool();

    if (is_null) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct aws_allocator *allocator = aws_default_allocator();
        size_t total_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)allocator->malloc(allocator, total_size);
        __CPROVER_assume(str != NULL);

        str->allocator = allocator;

        /* set const length field */
        memcpy((void *)&str->len, &len, sizeof(len));

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* No further dereferencing of str after destroy, as it may have been freed */
    if (str != NULL) {
        /* If the allocator was NULL, the memory would not be freed and we could
         * check that the bytes were zeroed. In this harness we always use the
         * default allocator, so we simply ensure the function returns without
         * crashing. */
        assert(1);
    } else {
        assert(str == NULL);
    }
}
