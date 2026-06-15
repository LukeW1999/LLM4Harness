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
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array array;
    aws_array_init(&array, allocator, sizeof(char), 10);
    char c_str[MAX_BUFFER_SIZE];
    __CPROVER_nondet_string(c_str);

    for (size_t i = 0; i < strlen(c_str); i++) {
        aws_array_push_back(&array, &c_str[i]);
    }

    bool result = aws_array_eq_c_str(&array, c_str);
    assert(result == (array.length == strlen(c_str) && memcmp(array.data, c_str, array.length) == 0));
    assert(aws_array_is_valid(&array));

    aws_array_clean_up(&array);

    // Test with empty array
    aws_array_init(&array, allocator, sizeof(char), 10);
    result = aws_array_eq_c_str(&array, c_str);
    assert(result == (array.length == 0 && strlen(c_str) == 0));
    assert(aws_array_is_valid(&array));
    aws_array_clean_up(&array);

    // Test with empty c_str
    aws_array_init(&array, allocator, sizeof(char), 10);
    for (size_t i = 0; i < strlen(c_str); i++) {
        aws_array_push_back(&array, &c_str[i]);
    }
    char empty_c_str[] = "";
    result = aws_array_eq_c_str(&array, empty_c_str);
    assert(result == (array.length == 0 && strlen(empty_c_str) == 0));
    assert(aws_array_is_valid(&array));
    aws_array_clean_up(&array);
}

int main() {
    aws_array_eq_c_str_harness();
    return 0;
}
