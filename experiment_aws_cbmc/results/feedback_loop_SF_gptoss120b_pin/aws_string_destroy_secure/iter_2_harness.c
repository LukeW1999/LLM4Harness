#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *s = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 1024);
        uint8_t *buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        s = aws_string_new_from_array(allocator, buf, len);
        free(buf);
    }

    aws_string_destroy_secure(s);
}
