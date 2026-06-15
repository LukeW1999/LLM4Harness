#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness() {
    struct aws_array *array = ensure_array_is_bounded_and_allocated(MAX_BUFFER_SIZE, aws_default_allocator());
    const char *c_str = __CPROVER_nondet_string();
    bool result = aws_array_eq_c_str(array, c_str);

    assert(result == (array->length == strlen(c_str) && memcmp(array->data, c_str, array->length) == 0));
    assert(aws_array_is_valid(array));
}

int main() {
    aws_array_eq_c_str_harness();
    return 0;
}
