#include <proof_helpers/make_common_data_structures.h>

static const uint8_t data_buf[256] = {0};

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint();
        __CPROVER_assume(len < 256);
        str = aws_string_new_from_array(aws_default_allocator(), data_buf, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_allocator *old_allocator = NULL;
    if (str != NULL) {
        old_allocator = str->allocator;
    }

    aws_string_destroy(str);

    (void)old_allocator;
}
