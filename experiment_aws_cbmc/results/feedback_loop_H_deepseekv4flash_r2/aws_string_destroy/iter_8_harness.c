#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string) + 1);
    if (str) {
        str->allocator = aws_default_allocator();
        str->len = 0;
        str->bytes[0] = '\0';
        __CPROVER_assume(aws_string_is_valid(str));
    }
    aws_string_destroy(str);
}
