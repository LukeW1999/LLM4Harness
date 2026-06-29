#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 4
#define MAX_ITEM_SIZE 8

void aws_array_list_swap_contents_harness(void) {
    struct aws_array_list from;
    struct aws_array_list to;

    /* Setup from list */
    __CPROVER_assume(aws_array_list_is_bounded(&from, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&from));
    __CPROVER_assume(aws_array_list_is_bounded(&to, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&to));
    __CPROVER_assume(from.alloc != NULL);
    __CPROVER_assume(to.alloc != NULL);
    __CPROVER_assume(from.item_size > 0);
    __CPROVER_assume(to.item_size > 0);
    __CPROVER_assume(from.item_size == to.item_size);

    /* Also require same allocator (precondition of the function) */
    __CPROVER_assume(from.alloc == to.alloc);

    /* Save original values for postcondition checks */
    struct aws_array_list old_from = from;
    struct aws_array_list old_to = to;

    /* Call the function under test */
    aws_array_list_swap_contents(&from, &to);

    /* Postcondition 1: Both lists are still valid after swap */
    assert(aws_array_list_is_valid(&from));
    assert(aws_array_list_is_valid(&to));

    /* Postcondition 2: Contents are swapped - from now has to's old contents */
    assert(from.data == old_to.data);
    assert(from.current_size == old_to.current_size);
    assert(from.length == old_to.length);
    assert(from.item_size == old_to.item_size);
    assert(from.alloc == old_to.alloc);

    /* Postcondition 3: Contents are swapped - to now has from's old contents */
    assert(to.data == old_from.data);
    assert(to.current_size == old_from.current_size);
    assert(to.length == old_from.length);
    assert(to.item_size == old_from.item_size);
    assert(to.alloc == old_from.alloc);

    /* Postcondition 4: item_size is preserved (same in both since precondition requires equality) */
    assert(from.item_size == to.item_size);

    /* Postcondition 5: Length invariants - lengths are valid with respect to capacity */
    assert(from.length * from.item_size <= from.current_size);
    assert(to.length * to.item_size <= to.current_size);

    /* Postcondition 6: Allocators are still non-null and equal */
    assert(from.alloc != NULL);
    assert(to.alloc != NULL);
    assert(from.alloc == to.alloc);
}
