#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* nondeterministic input string */
    const char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= max_len);

        char *tmp = malloc(str_len + 1);
        __CPROVER_assume(tmp != NULL);
        /* fill with nondet bytes */
        for (size_t i = 0; i < str_len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[str_len] = '\0';
        c_str = tmp;
    }

    /* call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* postconditions */
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);
        assert(buf.buffer == (uint8_t *)c_str);
    }
    assert(buf.allocator == NULL);

    /* validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
