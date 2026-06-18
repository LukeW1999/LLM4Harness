#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness(void) {
    /* Allocate the list structure on the stack */
    struct aws_array_list list;
    struct aws_array_list *list_ptr = &list;

    /* Nondeterministic inputs */
    size_t item_count = nondet_uint64_t();
    size_t item_size  = nondet_uint64_t();

    /* Preconditions */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= SIZE_MAX / item_size); /* no overflow */

    size_t total_size = item_count * item_size;
    void *raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* Snapshot of inputs */
    void *raw_array_pre = raw_array;
    size_t item_count_pre = item_count;
    size_t item_size_pre = item_size;

    /* Call the function under verification */
    aws_array_list_init_static(list_ptr, raw_array, item_count, item_size);

    /* Post‑condition assertions */
    assert(list_ptr->alloc == NULL);
    assert(list_ptr->data == raw_array_pre);
    assert(list_ptr->item_size == item_size_pre);
    assert(list_ptr->capacity == item_count_pre);
    assert(list_ptr->length == 0);
    assert(list_ptr->current_size == 0);
    assert(list_ptr->destroy == NULL);

    /* Frame condition: the static buffer must remain unchanged */
    uint8_t *raw_pre  = (uint8_t *)raw_array_pre;
    uint8_t *raw_post = (uint8_t *)list_ptr->data;
    for (size_t i = 0; i < total_size; ++i) {
        assert(raw_pre[i] == raw_post[i]);
    }
}
