#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Preconditions */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= 10);
    __CPROVER_assume(item_size <= 10);

    size_t current_size = 0;
    int overflow = aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(!overflow);
    __CPROVER_assume(current_size > 0);
    __CPROVER_assume(current_size <= 1024);

    uint8_t raw_buffer[1024];
    void *raw_array = raw_buffer;
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postconditions */
    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);
    assert(list.current_size == current_size);
    assert(aws_array_list_is_valid(&list));
}
