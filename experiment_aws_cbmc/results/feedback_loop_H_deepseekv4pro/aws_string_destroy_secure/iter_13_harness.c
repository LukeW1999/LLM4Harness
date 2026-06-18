#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = aws_string_new_from_c_str(allocator, "test");
    assert(aws_string_is_valid(str));
    aws_string_destroy_secure(str);
}
