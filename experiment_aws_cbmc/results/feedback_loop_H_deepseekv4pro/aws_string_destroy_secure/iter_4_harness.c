#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

#define MAX_LEN 1024

void aws_string_destroy_secure_harness() {
    bool is_null;

    if (is_null) {
        aws_string_destroy_secure(NULL);
        assert(1);
        return;
    }

    /* Non-NULL case: allocate a string on the stack using a wrapper struct */
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_LEN + 1];
    } string_buf;
    struct aws_string *str = &string_buf.s;

    /* Initialize the string fields */
    str->len = len;
    str->allocator = aws_default_allocator();
    for (size_t i = 0; i < len; i++) {
        uint8_t byte;
        str->bytes[i] = byte;
    }
    str->bytes[len] = '\0';

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postcondition: check that the bytes are zeroed */
    if (len > 0) {
        for (size_t i = 0; i < len; i++) {
            assert(str->bytes[i] == 0);
        }
    }
    assert(1);
}
