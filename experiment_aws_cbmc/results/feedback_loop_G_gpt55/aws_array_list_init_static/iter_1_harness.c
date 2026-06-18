#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;

    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t current_size = 0;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(no_overflow);
    __CPROVER_assume(current_size > 0);

    uint8_t *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(raw_array, current_size));

    __CPROVER_havoc_object(raw_array);

    struct store_byte_from_buffer old_byte;
    save_byte_from_array(raw_array, current_size, &old_byte);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.data == raw_array);

    assert(aws_array_list_capacity(&list) == item_count);
    assert(aws_array_list_length(&list) == 0);

    assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    assert_byte_from_buffer_matches(raw_array, &old_byte);

    assert(aws_array_list_is_valid(&list));
}
