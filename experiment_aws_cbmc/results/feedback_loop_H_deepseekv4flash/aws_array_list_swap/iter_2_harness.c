#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <string.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic indices */
    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save old state for comparison */
    struct aws_array_list old = list;

    /* Allocate buffers to store old element values at indices a and b */
    uint8_t *old_a = malloc(list.item_size);
    uint8_t *old_b = malloc(list.item_size);
    __CPROVER_assume(old_a != NULL);
    __CPROVER_assume(old_b != NULL);

    /* Save old values - preconditions guarantee success */
    int r1 = aws_array_list_get_at(&list, old_a, a);
    assert(r1 == AWS_OP_SUCCESS);
    int r2 = aws_array_list_get_at(&list, old_b, b);
    assert(r2 == AWS_OP_SUCCESS);

    /* 4. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Assert list fields unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert data content swapped */
    uint8_t *new_a = malloc(list.item_size);
    uint8_t *new_b = malloc(list.item_size);
    __CPROVER_assume(new_a != NULL);
    __CPROVER_assume(new_b != NULL);

    int r3 = aws_array_list_get_at(&list, new_a, a);
    assert(r3 == AWS_OP_SUCCESS);
    int r4 = aws_array_list_get_at(&list, new_b, b);
    assert(r4 == AWS_OP_SUCCESS);

    if (a == b) {
        assert_bytes_match(old_a, new_a, list.item_size);
    } else {
        assert_bytes_match(old_a, new_b, list.item_size);
        assert_bytes_match(old_b, new_a, list.item_size);
    }

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Cleanup */
    free(old_a);
    free(old_b);
    free(new_a);
    free(new_b);
}
