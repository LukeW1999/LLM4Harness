/* CBMC harness for aws_array_list_capacity */

#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

/* nondet function prototype */
uint64_t nondet_uint64_t(void);

static size_t checksum_bytes(const uint8_t *buf, size_t len) {
    size_t sum = 0;
    for (size_t i = 0; i < len; ++i) {
        sum += buf[i];
    }
    return sum;
}

void harness(void) {
    struct aws_array_list list;
    struct aws_array_list old_list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic item size (must be > 0) */
    size_t item_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_size > 0);

    /* nondeterministic initial allocation count */
    size_t init_count = (size_t)nondet_uint64_t();

    /* initialize the list dynamically */
    int init_result = aws_array_list_init_dynamic(&list, allocator, init_count, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* compute a checksum of the data buffer (if any) */
    size_t pre_checksum = 0;
    if (list.current_size > 0) {
        pre_checksum = checksum_bytes((const uint8_t *)list.data, list.current_size);
    }

    /* set length to a nondeterministic value within capacity */
    size_t capacity = list.current_size / list.item_size;
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= capacity);
    list.length = len;

    /* snapshot the whole list structure before the call */
    old_list = list;

    /* call the function under test */
    size_t cap = aws_array_list_capacity(&list);

    /* postconditions */

    /* return value correctness */
    __CPROVER_assert(cap == list.current_size / list.item_size, "capacity matches calculation");
    __CPROVER_assert(cap >= list.length, "capacity not less than length");

    /* frame condition: list structure unchanged */
    __CPROVER_assert(memcmp(&list, &old_list, sizeof(struct aws_array_list)) == 0, "list struct unchanged");

    /* data buffer unchanged */
    if (list.current_size > 0) {
        size_t post_checksum = checksum_bytes((const uint8_t *)list.data, list.current_size);
        __CPROVER_assert(post_checksum == pre_checksum, "data buffer unchanged");
    }

    /* list remains valid */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
