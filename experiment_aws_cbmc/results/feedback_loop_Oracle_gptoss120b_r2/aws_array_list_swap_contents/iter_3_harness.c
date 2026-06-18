#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

/* Bounds for the nondeterministic allocation sizes used in the harness */
#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

void aws_array_list_swap_contents_harness(void) {
    struct aws_array_list list_a;
    struct aws_array_list list_b;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize allocator fields */
    list_a.alloc = allocator;
    list_b.alloc = allocator;

    /* Nondeterministically initialize the structures */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    __CPROVER_assume(list_a.item_size > 0);
    __CPROVER_assume(list_b.item_size > 0);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);

    /* Ensure underlying buffers, if both present, are distinct */
    __CPROVER_assume(!(list_a.data != NULL && list_b.data != NULL && list_a.data == list_b.data));

    /* Save old copies of the structures */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* Call the function under verification */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks */
    /* -------------------------------------------------------------------- */

    /* 1. Allocator pointers are swapped and remain equal */
    assert(list_a.alloc == old_b.alloc);
    assert(list_b.alloc == old_a.alloc);
    assert(list_a.alloc == list_b.alloc);

    /* 2. Item size is unchanged and equal for both lists */
    assert(list_a.item_size == old_b.item_size);
    assert(list_b.item_size == old_a.item_size);
    assert(list_a.item_size == list_b.item_size);

    /* 3. Length and current_size are swapped */
    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);
    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);

    /* 4. Data pointers are swapped */
    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* 5. Validity predicates still hold after the swap */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* 6. Boundedness predicates still hold after the swap */
    assert(aws_array_list_is_bounded(&list_a,
                                     MAX_INITIAL_ITEM_ALLOCATION,
                                     MAX_ITEM_SIZE));
    assert(aws_array_list_is_bounded(&list_b,
                                     MAX_INITIAL_ITEM_ALLOCATION,
                                     MAX_ITEM_SIZE));
}
