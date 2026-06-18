#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_init_static_harness(void) {
    /* Allocate the list structure on the stack */
    struct aws_array_list list;
    struct aws_array_list *list_ptr = &list;

    /* Nondeterministic inputs */
    size_t item_count = (size_t)nondet_uint64_t();
    size_t item_size  = (size_t)nondet_uint64_t();

    /* Preconditions */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= SIZE_MAX / item_size); /* no overflow */

    size_t total_size = item_count * item_size;

    /* Allocate a static buffer */
    void *raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* Snapshot of inputs */
    void *raw_array_pre = raw_array;
    size_t item_count_pre = item_count;
    size_t item_size_pre = item_size;

    /* Call the function under verification */
    aws_array_list_init_static(list_ptr, raw_array, item_count, item_size);

    /* Post‑condition assertions */
    __CPROVER_assert(list_ptr->alloc == NULL, "alloc must be NULL");
    __CPROVER_assert(list_ptr->data == raw_array_pre, "data must point to the static buffer");
    __CPROVER_assert(list_ptr->item_size == item_size_pre, "item_size must be preserved");
    __CPROVER_assert(list_ptr->capacity == item_count_pre, "capacity must be set");
    __CPROVER_assert(list_ptr->length == 0, "length must be zero");
    __CPROVER_assert(list_ptr->current_size == 0, "current_size must be zero");
    __CPROVER_assert(list_ptr->destroy == NULL, "destroy must be NULL");

    /* Frame condition: the static buffer must remain unchanged */
    uint8_t *raw_pre  = (uint8_t *)raw_array_pre;
    uint8_t *raw_post = (uint8_t *)list_ptr->data;
    for (size_t i = 0; i < total_size; ++i) {
        __CPROVER_assert(raw_pre[i] == raw_post[i], "static buffer unchanged");
    }
}
