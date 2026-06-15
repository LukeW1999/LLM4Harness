#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness() {
    struct aws_array *array = ensure_array_is_bounded(nondet_ptr(), MAX_BUFFER_SIZE);
    const char *c_str = nondet_c_string(MAX_BUFFER_SIZE);
    struct aws_allocator *allocator = aws_default_allocator();

    bool result = aws_array_eq_c_str(array, c_str);

    assert(result == (array->length == strlen(c_str)));
    if (result) {
        for (size_t i = 0; i < array->length; i++) {
            assert(((char *)array->data)[i] == c_str[i]);
        }
    }

    assert(aws_array_is_valid(array));
}
