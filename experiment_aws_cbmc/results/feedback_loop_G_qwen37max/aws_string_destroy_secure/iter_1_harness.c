#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        uint8_t *mem = malloc(sizeof(struct aws_string) + len);
        str = (struct aws_string *)mem;
        if (str) {
            struct {
                struct aws_allocator *allocator;
                size_t len;
                uint8_t bytes[1];
            } *mutable_str = (void *)str;
            
            mutable_str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
            mutable_str->len = len;
        }
    }

    bool has_allocator = false;
    size_t old_len = 0;
    if (str) {
        has_allocator = (str->allocator != NULL);
        old_len = str->len;
    }

    aws_string_destroy_secure(str);

    if (str && !has_allocator) {
        assert(str->len == old_len);
        for (size_t i = 0; i < str->len; ++i) {
            assert(str->bytes[i] == 0);
        }
    }
}
