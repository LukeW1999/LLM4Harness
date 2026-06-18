#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    struct aws_array_list *list_ptr = &list;

    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);

    size_t total_size = item_count * item_size;

    uint8_t static_buf[1024];
    __CPROVER_assume(total_size <= sizeof(static_buf));

    void *raw_array = static_buf;

    int rc = aws_array_list_init_static(list_ptr, raw_array, item_count, item_size);
    __CPROVER_assert(rc == 0, "aws_array_list_init_static should succeed");

    __CPROVER_assert(list_ptr->alloc == NULL, "alloc must be NULL");
    __CPROVER_assert(list_ptr->data == raw_array, "data must point to the static buffer");
    __CPROVER_assert(list_ptr->item_size == item_size, "item_size must be preserved");
    __CPROVER_assert(list_ptr->capacity == item_count, "capacity must be set");
    __CPROVER_assert(list_ptr->length == 0, "length must be zero");
    __CPROVER_assert(list_ptr->current_size == 0, "current_size must be zero");
    __CPROVER_assert(list_ptr->destroy == NULL, "destroy must be NULL");
}
