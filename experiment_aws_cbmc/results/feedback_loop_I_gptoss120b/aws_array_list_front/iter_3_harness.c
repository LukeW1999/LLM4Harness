#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    /* Bound the allocation parameters */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate the internal buffer */
    ensure_array_list_has_allocated_data_member(&list);

    /* Basic validity of the list */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Item size constraints */
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.item_size > 0);

    /* Choose a nondeterministic length that respects the invariants */
    list.length = nondet_uint();
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    /* Preserve the old state for frame condition checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_bytes);
    }

    /* Call the function under test */
    void *front = aws_array_list_front(&list);

    /* Post‑conditions */
    if (list.length > 0) {
        assert(front != NULL);
        assert(front == list.data);
        assert_bytes_match((uint8_t *)front,
                           (uint8_t *)list.data,
                           list.item_size);
    } else {
        assert(front == NULL);
    }

    /* Frame condition: the list structure and its buffer contents are unchanged */
    assert_array_list_equivalence(&list, &old, &old_bytes);
    assert(aws_array_list_is_valid(&list));
}
