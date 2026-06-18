#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_BUFFER_SIZE);
    
    struct aws_string *str = nondet_bool() ? NULL : malloc(sizeof(struct aws_string) + len);
    
    struct aws_allocator *saved_alloc = NULL;
    size_t saved_len = 0;
    
    if (str != NULL) {
        struct aws_allocator *alloc = nondet_bool() ? NULL : aws_default_allocator();
        *(struct aws_allocator **)&str->allocator = alloc;
        *(size_t *)&str->len = len;
        
        uint8_t *bytes_mut = (uint8_t *)str->bytes;
        bytes_mut[len] = 0;
        
        saved_alloc = str->allocator;
        saved_len = str->len;
    }

    bool should_free = (str != NULL && saved_alloc != NULL);

    aws_string_destroy(str);

    if (!should_free && str != NULL) {
        assert(aws_string_is_valid(str));
        assert(str->allocator == saved_alloc);
        assert(str->len == saved_len);
    }
}
