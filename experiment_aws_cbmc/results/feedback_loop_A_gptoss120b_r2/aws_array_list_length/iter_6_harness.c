#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.allocator = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    size_t index = nondet_size_t();

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);

        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert_array_list_equivalence(&list, &old, NULL);
    }

    assert(aws_array_list_is_valid(&list));
}
