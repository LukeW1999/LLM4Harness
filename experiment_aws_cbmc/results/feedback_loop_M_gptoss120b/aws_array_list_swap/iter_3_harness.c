#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Allocate and bound the array list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Initialize the list */
    int init_result = aws_array_list_init(&list, alloc, capacity, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Set a nondeterministic length within capacity */
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Choose nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save the elements at the two indices */
    uint8_t *old_elem_a = (uint8_t *)malloc(item_size);
    uint8_t *old_elem_b = (uint8_t *)malloc(item_size);
    __CPROVER_assume(old_elem_a != NULL);
    __CPROVER_assume(old_elem_b != NULL);

    aws_array_list_get_at(&list, old_elem_a, a);
    aws_array_list_get_at(&list, old_elem_b, b);

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Verify that the two elements have been swapped */
    uint8_t *new_elem_a = (uint8_t *)malloc(item_size);
    uint8_t *new_elem_b = (uint8_t *)malloc(item_size);
    __CPROVER_assume(new_elem_a != NULL);
    __CPROVER_assume(new_elem_b != NULL);

    aws_array_list_get_at(&list, new_elem_a, a);
    aws_array_list_get_at(&list, new_elem_b, b);

    for (size_t i = 0; i < item_size; ++i) {
        assert(new_elem_a[i] == old_elem_b[i]);
        assert(new_elem_b[i] == old_elem_a[i]);
    }

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
