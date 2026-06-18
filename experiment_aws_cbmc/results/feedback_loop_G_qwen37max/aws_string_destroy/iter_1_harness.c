#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        str = malloc(sizeof(struct aws_string) + len);
        if (str) {
            __CPROVER_assume(aws_string_is_valid(str));
        }
    }

    bool was_freed = str && str->allocator != NULL;

    aws_string_destroy(str);

    if (!was_freed && str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
