#include <aws/common/array_list.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = __CPROVER_nondet_uint8_t();
        }
    }

    size_t index = __CPROVER_nondet_size_t();

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(
            __CPROVER_memcmp(((uint8_t *)list.data) + index * list.item_size,
                            val,
                            list.item_size) == 0,
            "bytes match after set_at");

        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        __CPROVER_assert(list.length == expected_len, "length updated correctly");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
        __CPROVER_assert(list.alloc == old.alloc, "allocator unchanged");
    } else {
        assert_array_list_equivalence(&list, &old, NULL);
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
