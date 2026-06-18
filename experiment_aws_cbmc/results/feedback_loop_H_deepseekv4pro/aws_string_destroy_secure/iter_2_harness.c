#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    bool is_null = nondet_bool();

    if (is_null) {
        aws_string_destroy_secure(NULL);
        assert(1);
        return;
    }

    /* Non-NULL case: allocate a string on the stack */
    /* The struct aws_string has a flexible array member bytes[] */
    /* We need to allocate enough stack space for the header + len + 1 */
    size_t len;
    __CPROVER_assume(len <= 1024);
    size_t size = sizeof(struct aws_string) + len + 1;
    uint8_t buf[size];
    struct aws_string *str = (struct aws_string *)buf;

    /* Initialize the string fields */
    str->len = len;
    str->allocator = aws_default_allocator();
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = '\0';

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postcondition: check that the bytes are zeroed */
    /* Since the function zeros the memory, we verify that */
    if (len > 0) {
        for (size_t i = 0; i < len; i++) {
            assert(str->bytes[i] == 0);
        }
    }
    assert(1);
}
