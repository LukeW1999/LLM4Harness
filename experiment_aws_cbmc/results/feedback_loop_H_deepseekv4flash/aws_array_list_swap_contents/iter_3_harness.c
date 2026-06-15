#include <aws/common/array_list.h>
#include <stdlib.h>

#define MAX_ITEM_SIZE 10
#define MAX_INITIAL_ALLOCATION 100

void aws_array_list_swap_contents_harness() {
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    // Nondeterministic sizes
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    size_t initial_allocation_a;
    __CPROVER_assume(initial_allocation_a % item_size == 0);
    __CPROVER_assume(initial_allocation_a <= MAX_INITIAL_ALLOCATION);
    size_t initial_allocation_b;
    __CPROVER_assume(initial_allocation_b % item_size == 0);
    __CPROVER_assume(initial_allocation_b <= MAX_INITIAL_ALLOCATION);
    size_t length_a;
    __CPROVER_assume(length_a <= initial_allocation_a / item_size);
    size_t length_b;
    __CPROVER_assume(length_b <= initial_allocation_b / item_size);

    // Allocate data arrays
    void *data_a = malloc(initial_allocation_a);
    void *data_b = malloc(initial_allocation_b);
    __CPROVER_assume(initial_allocation_a == 0 || data_a != NULL);
    __CPROVER_assume(initial_allocation_b == 0 || data_b != NULL);

    // Allocator (same for both lists, non-NULL)
    static struct aws_allocator allocator_instance;
    list_a.alloc = &allocator_instance;
    list_b.alloc = &allocator_instance;

    // Assign fields
    list_a.item_size = item_size;
    list_b.item_size = item_size;
    list_a.current_size = initial_allocation_a;
    list_b.current_size = initial_allocation_b;
    list_a.length = length_a;
    list_b.length = length_b;
    list_a.data = data_a;
    list_b.data = data_b;

    // Ensure validity
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    // Ensure distinct objects
    __CPROVER_assume(&list_a != &list_b);

    // Save old states
    struct aws_array_list old_list_a = list_a;
    struct aws_array_list old_list_b = list_b;

    // Call function under test
    aws_array_list_swap_contents(&list_a, &list_b);

    // Postcondition: contents are swapped
    assert(list_a.alloc == old_list_b.alloc);
    assert(list_a.current_size == old_list_b.current_size);
    assert(list_a.length == old_list_b.length);
    assert(list_a.item_size == old_list_b.item_size);
    assert(list_a.data == old_list_b.data);

    assert(list_b.alloc == old_list_a.alloc);
    assert(list_b.current_size == old_list_a.current_size);
    assert(list_b.length == old_list_a.length);
    assert(list_b.item_size == old_list_a.item_size);
    assert(list_b.data == old_list_a.data);

    // Validity invariants are preserved
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
