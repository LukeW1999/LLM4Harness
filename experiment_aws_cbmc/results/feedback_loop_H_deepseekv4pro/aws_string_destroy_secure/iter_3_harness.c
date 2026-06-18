#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LEN 1024

void aws_string_destroy_secure_harness() {
    bool is_null;

    if (is_null) {
        aws_string_destroy_secure(NULL);
        assert(1);
        return;
    }

    /* Non-NULL case: allocate a string on the stack */
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);
    uint8_t buf[sizeof(struct aws_string) + MAX_LEN + 1];
    struct aws_string *str = (struct aws_string *)buf;

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
