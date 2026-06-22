/* Stub for non-x86 (ARM) builds -- SSE routines not available */
typedef float FLOAT;
typedef struct gr_info_s gr_info;

void init_xrpow_core_sse(gr_info * const cod_info, FLOAT xrpow[576], int upper, FLOAT * sum)
{
    (void)cod_info; (void)xrpow; (void)upper; (void)sum;
}
