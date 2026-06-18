#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    void *raw_array;
    size_t item_count = (size_t)nondet_uint64_t();
    size_t item_size = (size_t)nondet_uint64_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t expected_current_size = 0;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &expected_current_size));

    /* Assume the static buffer is large enough */
    __CPROVER_assume(expected_current_size <= MAX_BUFFER_SIZE);
    raw_array = (void *)nondet_uintptr_t();
    __CPROVER_assume(raw_array != NULL);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    assert(list.alloc == NULL);
    assert(list.data == raw_array);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.current_size == expected_current_size);
    assert(aws_array_list_is_valid(&list));
}
