#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t c_str_len;
    char *c_str = NULL;

    c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);

    c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    // Fill with non-deterministic bytes, but no null bytes before the end
    for (size_t i = 0; i < c_str_len; i++) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != 0);
    }
    c_str[c_str_len] = '\0';

    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        assert(allocator == result->allocator);
        assert(c_str_len == result->len);
        assert(aws_string_is_valid(result));
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_str_len);
        assert(result->bytes[result->len] == 0);
    } else {
        // Allocation failure, nothing to assert
    }

    free(c_str);

    // Optional cleanup of result is omitted for the harness
}
