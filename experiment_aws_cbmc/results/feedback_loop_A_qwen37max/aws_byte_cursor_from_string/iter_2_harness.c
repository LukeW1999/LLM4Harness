#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src = NULL;
    bool src_is_null = nondet_bool();
    
    if (!src_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        size_t alloc_size = sizeof(struct aws_string) + len;
        src = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(src != NULL);
        
        /* Cast to a mutable structure to initialize const fields for CBMC */
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[1];
        } *mut_src = (void *)src;
        
        mut_src->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        mut_src->len = len;
        
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }
    
    assert(aws_byte_cursor_is_valid(&cursor));
}
