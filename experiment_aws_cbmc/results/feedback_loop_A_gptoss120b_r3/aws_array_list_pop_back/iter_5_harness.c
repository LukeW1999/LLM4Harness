#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet item size and capacity within bounds */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* initialize the list */
    aws_array_list_init(&list, allocator, capacity, item_size);

    /* nondet length respecting capacity */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= capacity);

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Prepare output buffer */
    uint8_t out[MAX_ITEM_SIZE];

    /* Call function under test */
    int result = aws_array_list_pop_back(&list, out);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        assert(list.length == old.length - 1);
    } else {
        assert(list.length == old.length);
    }

    /* Fields that must not change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
