#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;
    struct aws_string old = {0};
    uint8_t old_bytes[MAX_LEN];
    size_t old_len = 0;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_LEN);

        /* stack‑allocated storage for the flexible‑array struct */
        struct {
            struct aws_string s;
            uint8_t data[MAX_LEN];
        } storage;

        str = &storage.s;

        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));

        old = *str;
        old_len = len;
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = str->bytes[i];
        }
    }

    aws_string_destroy(str);

    if (str && str->allocator) {
        /* memory was released; no further checks on `str` */
    } else {
        if (str) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            if (old_len > 0) {
                for (size_t i = 0; i < old_len; ++i) {
                    assert(str->bytes[i] == old_bytes[i]);
                }
            }
        }
        assert(aws_string_is_valid(str));
    }
}
