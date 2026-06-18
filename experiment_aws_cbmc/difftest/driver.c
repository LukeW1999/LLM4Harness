#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <stdio.h>
#include <string.h>

static unsigned long long st = 88172645463325252ULL;
static unsigned rnd(unsigned m){ st ^= st<<13; st ^= st>>7; st ^= st<<17; return (unsigned)(st % m); }

int main(void){
    struct aws_allocator *alloc = aws_default_allocator();
    for (int t = 0; t < 60000; t++){
        unsigned len1 = rnd(25);
        uint8_t a[40];
        /* full byte range, including interior NUL (legal in aws_string/cursor/buf) */
        for (unsigned i=0;i<len1;i++) a[i] = (uint8_t)rnd(256);
        unsigned mode = rnd(6);
        unsigned len2 = len1;
        uint8_t b[42]; memcpy(b, a, len1);
        if (mode == 1 && len1 > 0) b[rnd(len1)] ^= (uint8_t)(1 + rnd(255));
        else if (mode == 2) { len2 = rnd(25); for (unsigned i=0;i<len2;i++) b[i] = (uint8_t)rnd(256); }
        else if (mode == 3 && len1 > 0) { len2 = len1 - 1; }            /* prefix */
        else if (mode == 4) { len2 = len1 + 1; b[len1] = (uint8_t)rnd(256); } /* extension */
        else if (mode == 5 && len1 > 0) { b[len1-1] = (uint8_t)(b[len1-1] + 1); } /* last-byte bump */
        struct aws_string *s = aws_string_new_from_array(alloc, a, len1);
        int r = -1;
        unsigned nullmode = rnd(10);   /* 0-6: normal; 7: str=NULL; 8: other=NULL; 9: both NULL */
#if FUNC_VARIANT == 1
        /* c_str cannot contain interior NUL: rewrite zeros, keep aws_string side fully general */
        for (unsigned i=0;i<len2;i++) if (b[i] == 0) b[i] = (uint8_t)(1 + rnd(255));
        b[len2] = 0;
        r = (int)aws_string_eq_c_str(nullmode==7||nullmode==9 ? NULL : s,
                                     nullmode==8||nullmode==9 ? NULL : (const char *)b);
#elif FUNC_VARIANT == 2
        struct aws_byte_cursor cur = aws_byte_cursor_from_array(b, len2);
        r = (int)aws_string_eq_byte_cursor(nullmode==7||nullmode==9 ? NULL : s,
                                           nullmode==8||nullmode==9 ? NULL : &cur);
#else
        struct aws_byte_buf buf = aws_byte_buf_from_array(b, len2);
        r = (int)aws_string_eq_byte_buf(nullmode==7||nullmode==9 ? NULL : s,
                                        nullmode==8||nullmode==9 ? NULL : &buf);
#endif
        printf("%d %d\n", t, r);
        aws_string_destroy(s);
    }
    return 0;
}
