#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    struct aws_array_list *list_ptr = &list;

    bool has_alloc = __CPROVER_nondet_bool();
    if (has_alloc) {
        list_ptr->alloc = aws_default_allocator();
    } else {
        list_ptr->alloc = NULL;
    }

    bool has_data = __CPROVER_nondet_bool();
    if (has_data) {
        size_t data_size = __CPROVER_nondet_uint();
        __CPROVER_assume(data_size > 0);
        if (list_ptr->alloc) {
            list_ptr->data = aws_mem_acquire(list_ptr->alloc, data_size);
        } else {
            list_ptr->data = malloc(data_size);
        }
        __CPROVER_assume(list_ptr->data != NULL);
    } else {
        list_ptr->data = NULL;
    }

    list_ptr->current_size = __CPROVER_nondet_uint();
    list_ptr->length = __CPROVER_nondet_uint();
    list_ptr->item_size = __CPROVER_nondet_uint();

    unsigned char *frame_buffer = malloc(16);
    __CPROVER_assume(frame_buffer != NULL);
    for (size_t i = 0; i < 16; ++i) {
        frame_buffer[i] = __CPROVER_nondet_uchar();
    }

    unsigned char frame_buffer_copy[16];
    for (size_t i = 0; i < 16; ++i) {
        frame_buffer_copy[i] = frame_buffer[i];
    }

    aws_array_list_clean_up(list_ptr);

    assert(list_ptr->alloc == NULL);
    assert(list_ptr->data == NULL);
    assert(list_ptr->current_size == 0);
    assert(list_ptr->length == 0);
    assert(list_ptr->item_size == 0);

    for (size_t i = 0; i < 16; ++i) {
        assert(frame_buffer[i] == frame_buffer_copy[i]);
    }

    free(frame_buffer);
}
