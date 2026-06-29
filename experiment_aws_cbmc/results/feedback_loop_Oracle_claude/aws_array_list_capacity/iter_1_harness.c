#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;

    /* Nondeterministically choose between static and dynamic initialization */
    bool is_dynamic;
    __CPROVER_assume(is_dynamic == true || is_dynamic == false);

    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 128);

    size_t initial_item_count;
    __CPROVER_assume(initial_item_count > 0);
    __CPROVER_assume(initial_item_count <= 16);

    if (is_dynamic) {
        /* Dynamic initialization */
        int rc = aws_array_list_init_dynamic(&list, aws_default_allocator(), initial_item_count, item_size);
        __CPROVER_assume(rc == AWS_OP_SUCCESS);
    } else {
        /* Static initialization */
        size_t buf_size;
        __CPROVER_assume(!aws_mul_size_checked(initial_item_count, item_size, &buf_size));
        __CPROVER_assume(buf_size > 0);
        void *raw_array = malloc(buf_size);
        __CPROVER_assume(raw_array != NULL);
        aws_array_list_init_static(&list, raw_array, initial_item_count, item_size);
    }

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save state before the call for frame condition checks */
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    size_t old_length = list.length;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* Postcondition 1: Return value correctness
     * capacity = current_size / item_size */
    assert(capacity == old_current_size / old_item_size);

    /* Postcondition 2: Capacity invariants
     * capacity must be >= length (since current_size >= length * item_size) */
    assert(capacity >= list.length);

    /* Postcondition 3: The list remains valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 4: Frame conditions - nothing should be modified */
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);
    assert(list.data == old_data);
    assert(list.alloc == old_alloc);

    /* Postcondition 5: Capacity * item_size <= current_size */
    size_t capacity_bytes;
    bool no_overflow = !aws_mul_size_checked(capacity, list.item_size, &capacity_bytes);
    assert(no_overflow);
    assert(capacity_bytes <= list.current_size);

    /* Postcondition 6: If current_size is 0, capacity is 0 */
    if (list.current_size == 0) {
        assert(capacity == 0);
    }

    /* Postcondition 7: If capacity > 0, then current_size > 0 */
    if (capacity > 0) {
        assert(list.current_size > 0);
    }
}
