#ifndef AWS_COMMON_PRIVATE_BYTE_BUF_H
#define AWS_COMMON_PRIVATE_BYTE_BUF_H

#include <aws/common/byte_buf.h>

AWS_COMMON_API size_t aws_nospec_mask(size_t index, size_t bound);

AWS_COMMON_API
int aws_byte_buf_reserve_smart(struct aws_byte_buf *buffer, size_t requested_capacity);

AWS_COMMON_API
int aws_byte_buf_reserve_smart_relative(struct aws_byte_buf *buffer, size_t additional_length);

#endif
