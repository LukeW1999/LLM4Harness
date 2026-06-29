#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;

    /* Use a bounded item_size and current_size to avoid state space explosion */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= 4);

    size_t num_elements;
    __CPROVER_assume(num_elements <= 4);

    size_t current_size = item_size * num_elements;

    list.item_size = item_size;
    list.current_size = current_size;

    size_t length;
    __CPROVER_assume(length <= num_elements);
    list.length = length;

    if (current_size == 0) {
        list.data = NULL;
    } else {
        void *data = malloc(current_size);
        __CPROVER_assume(data != NULL);
        list.data = data;
    }

    list.alloc = NULL;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state for frame condition checks */
    void   *old_data         = list.data;
    size_t  old_current_size = list.current_size;
    size_t  old_item_size    = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under verification */
    aws_array_list_clear(&list);

    /* Postcondition: validity invariant preserved */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition (length): length is now 0 */
    assert(list.length == 0);

    /* Postcondition (frame): data pointer unchanged */
    assert(list.data == old_data);

    /* Postcondition (frame): current_size unchanged */
    assert(list.current_size == old_current_size);

    /* Postcondition (frame): item_size unchanged */
    assert(list.item_size == old_item_size);

    /* Postcondition (frame): alloc unchanged */
    assert(list.alloc == old_alloc);
}
