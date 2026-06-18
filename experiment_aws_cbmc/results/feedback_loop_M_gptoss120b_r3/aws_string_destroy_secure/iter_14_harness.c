#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    _Bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 1024);

        uint8_t data[1024];
        for (size_t i = 0; i < len; ++i) {
            data[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(aws_default_allocator(), data, len);
        __CPROVER_assume(str != NULL);

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            /* Ensure the bytes match the original data before destruction */
            assert(bytes[i] == data[i]);
        }

        aws_string_destroy_secure(str);

        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        aws_string_destroy_secure(str);
        assert(1);
    }
}
