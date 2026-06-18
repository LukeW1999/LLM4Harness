#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 1024

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    uint8_t buffer[sizeof(struct aws_string) + MAX_LEN];

    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);

        str = (struct aws_string *)buffer;

        if (__CPROVER_nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;
    }

    struct aws_string old_snapshot;
    if (str != NULL) {
        old_snapshot.allocator = str->allocator;
        old_snapshot.len = str->len;
    }

    aws_string_destroy(str);

    if (str != NULL) {
        if (old_snapshot.allocator == NULL) {
            assert(str->allocator == old_snapshot.allocator);
            assert(str->len == old_snapshot.len);
            assert(aws_string_is_valid(str));
        }
    }
}
