#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    bool is_null = nondet_bool();
    
    if (!is_null) {
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (str != NULL) {
        struct aws_string old = *str;
        aws_string_destroy(str);
        
        if (old.allocator == NULL) {
            assert(aws_string_is_valid(str));
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
        }
    } else {
        aws_string_destroy(str);
    }
}
