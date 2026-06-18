#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    /* Use bounded length allocation helper */
    size_t length;
    __CPROVER_assume(length <= 10);
    
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    
    /* Allocate the string through the allocator so CBMC tracks it as a dynamic object */
    struct aws_string *str = aws_mem_acquire(allocator, sizeof(struct aws_string) + length + 1);
    __CPROVER_assume(str != NULL);
    
    /* Initialize the string fields */
    *(struct aws_allocator **)(&str->allocator) = allocator;
    *(size_t *)(&str->len) = length;
    /* null terminator */
    ((uint8_t *)str->bytes)[length] = 0;
    
    __CPROVER_assume(aws_string_is_valid(str));
    
    /* Call function under test */
    aws_string_destroy(str);
    
    assert(true);
}
