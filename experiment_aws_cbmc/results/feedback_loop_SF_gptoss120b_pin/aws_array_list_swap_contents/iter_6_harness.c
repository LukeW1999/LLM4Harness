#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_contents_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* nondet inputs */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t capacity_a = nondet_size_t();
    size_t capacity_b = nondet_size_t();
    __CPROVER_assume(capacity_a <= 10);
    __CPROVER_assume(capacity_b <= 10);
    __CPROVER_assume(capacity_a == 0 || capacity_a <= SIZE_MAX / item_size);
    __CPROVER_assume(capacity_b == 0 || capacity_b <= SIZE_MAX / item_size);

    size_t length_a = nondet_size_t();
    size_t length_b = nondet_size_t();
    __CPROVER_assume(length_a <= capacity_a);
    __CPROVER_assume(length_b <= capacity_b);

    /* initialize array lists */
    struct aws_array_list list_a;
    int rc_a = aws_array_list_init(&list_a, alloc, capacity_a, item_size);
    __CPROVER_assume(rc_a == AWS_OP_SUCCESS);
    list_a.length = length_a;

    struct aws_array_list list_b;
    int rc_b = aws_array_list_init(&list_b, alloc, capacity_b, item_size);
    __CPROVER_assume(rc_b == AWS_OP_SUCCESS);
    list_b.length = length_b;

    /* additional assumptions to satisfy is_valid */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_b.alloc != NULL);
    __CPROVER_assume(length_a == 0 || list_a.data != NULL);
    __CPROVER_assume(length_b == 0 || list_b.data != NULL);

    /* preconditions */
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* snapshot pre‑call state */
    struct aws_allocator *alloc_a_snapshot = list_a.alloc;
    struct aws_allocator *alloc_b_snapshot = list_b.alloc;
    size_t item_size_snapshot = list_a.item_size;
    size_t length_a_snapshot = list_a.length;
    size_t length_b_snapshot = list_b.length;
    size_t current_size_a_snapshot = list_a.current_size;
    size_t current_size_b_snapshot = list_b.current_size;
    void *data_a_snapshot = list_a.data;
    void *data_b_snapshot = list_b.data;

    /* call under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* post‑conditions */
    __CPROVER_assert(aws_array_list_is_valid(&list_a), "list_a remains valid");
    __CPROVER_assert(aws_array_list_is_valid(&list_b), "list_b remains valid");

    __CPROVER_assert(list_a.alloc == alloc_a_snapshot, "alloc unchanged for list_a");
    __CPROVER_assert(list_b.alloc == alloc_b_snapshot, "alloc unchanged for list_b");

    __CPROVER_assert(list_a.item_size == item_size_snapshot, "item_size unchanged for list_a");
    __CPROVER_assert(list_b.item_size == item_size_snapshot, "item_size unchanged for list_b");

    __CPROVER_assert(list_a.length == length_b_snapshot, "length swapped to list_a");
    __CPROVER_assert(list_b.length == length_a_snapshot, "length swapped to list_b");

    __CPROVER_assert(list_a.current_size == current_size_b_snapshot, "current_size swapped to list_a");
    __CPROVER_assert(list_b.current_size == current_size_a_snapshot, "current_size swapped to list_b");

    __CPROVER_assert(list_a.data == data_b_snapshot, "data pointer swapped to list_a");
    __CPROVER_assert(list_b.data == data_a_snapshot, "data pointer swapped to list_b");
}
