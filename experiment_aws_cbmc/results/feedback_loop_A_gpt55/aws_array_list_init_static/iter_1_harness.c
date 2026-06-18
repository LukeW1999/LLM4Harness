#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;

    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();
    size_t current_size = 0;

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_mul_size_checked(item_count, item_size, &current_size) == AWS_OP_SUCCESS);
    __CPROVER_assume(current_size > 0);
    __CPROVER_assume(current_size <= MAX_BUFFER_SIZE);

    uint8_t *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(raw_array, current_size));

    for (size_t i = 0; i < current_size; ++i) {
        raw_array[i] = nondet_uint8_t();
    }

    struct store_byte_from_buffer old_raw_array_byte;
    save_byte_from_array(raw_array, current_size, &old_raw_array_byte);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.data == raw_array);

    assert(aws_array_list_length(&list) == 0);
    assert(aws_array_list_capacity(&list) == item_count);
    assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));

    assert_byte_from_buffer_matches(raw_array, &old_raw_array_byte);

    assert(aws_array_list_is_valid(&list));
}
