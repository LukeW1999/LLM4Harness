#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* Allocate two array list structures */
    struct aws_array_list *list_a = malloc(sizeof(struct aws_array_list));
    struct aws_array_list *list_b = malloc(sizeof(struct aws_array_list));

    /* Precondition: both pointers must be non-null */
    __CPROVER_assume(list_a != NULL);
    __CPROVER_assume(list_b != NULL);

    /* Precondition: lists must be distinct */
    __CPROVER_assume(list_a != list_b);

    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Set up a common item_size for both lists */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= SIZE_MAX);

    /* Set up list_a fields */
    list_a->alloc = alloc;
    list_a->item_size = item_size;

    size_t current_size_a;
    size_t length_a;
    /* current_size must be a multiple of item_size */
    __CPROVER_assume(current_size_a % item_size == 0);
    __CPROVER_assume(length_a <= current_size_a / item_size);

    list_a->current_size = current_size_a;
    list_a->length = length_a;

    /* data pointer: if current_size > 0, data must be non-null */
    if (current_size_a > 0) {
        void *data_a = malloc(current_size_a);
        __CPROVER_assume(data_a != NULL);
        list_a->data = data_a;
    } else {
        list_a->data = NULL;
    }

    /* Set up list_b fields */
    list_b->alloc = alloc;
    list_b->item_size = item_size;

    size_t current_size_b;
    size_t length_b;
    /* current_size must be a multiple of item_size */
    __CPROVER_assume(current_size_b % item_size == 0);
    __CPROVER_assume(length_b <= current_size_b / item_size);

    list_b->current_size = current_size_b;
    list_b->length = length_b;

    /* data pointer: if current_size > 0, data must be non-null */
    if (current_size_b > 0) {
        void *data_b = malloc(current_size_b);
        __CPROVER_assume(data_b != NULL);
        list_b->data = data_b;
    } else {
        list_b->data = NULL;
    }

    /* Verify preconditions hold */
    __CPROVER_assume(aws_array_list_is_valid(list_a));
    __CPROVER_assume(aws_array_list_is_valid(list_b));

    /* Save old values for postcondition checks */
    struct aws_array_list old_a = *list_a;
    struct aws_array_list old_b = *list_b;

    /* Call the function under verification */
    aws_array_list_swap_contents(list_a, list_b);

    /* Postconditions: validity */
    assert(aws_array_list_is_valid(list_a));
    assert(aws_array_list_is_valid(list_b));

    /* Postconditions: contents are swapped */
    assert(list_a->data == old_b.data);
    assert(list_b->data == old_a.data);

    assert(list_a->length == old_b.length);
    assert(list_b->length == old_a.length);

    assert(list_a->current_size == old_b.current_size);
    assert(list_b->current_size == old_a.current_size);

    assert(list_a->item_size == old_b.item_size);
    assert(list_b->item_size == old_a.item_size);

    assert(list_a->alloc == old_b.alloc);
    assert(list_b->alloc == old_a.alloc);

    /* Since both lists shared the same allocator, alloc is unchanged */
    assert(list_a->alloc == alloc);
    assert(list_b->alloc == alloc);

    /* item_size is the same for both, so it remains the same after swap */
    assert(list_a->item_size == item_size);
    assert(list_b->item_size == item_size);
}

void aws_array_list_swap_contents_harness(void) {
    aws_array_list_swap_contents_harness();
    return 0;
}
