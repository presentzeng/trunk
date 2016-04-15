/*******************************************************************************
 *                              5116
 *                            Module: musicdec
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       jpliao     2010-09-11 15:00     1.0             build this file
 *******************************************************************************/
#include "app_mengine.h"
#include "mengine_rdata.h"

/*!
 *  \brief
 *      DSP协议类型
 */
int32 mmm_mp_read(void *buf, int32 size, int32 count, storage_io_t *io)
{
    int32 ret = 0;
    int32 memcpy_cnt;
    if ((size != 1) /*|| (count != 512)*/|| (buf == NULL))
    {
        return -1;
    }
    // music_print("music read",0,0);
    if ((g_prefetch_data->prefetch_flag == TRUE) && (g_prefetch_data->prefetch_buf_remain > 0))
    {
        memcpy_cnt = g_prefetch_data->prefetch_buf_remain;
        if (g_prefetch_data->prefetch_buf_remain >= count)
        {
            memcpy_cnt = count;
        }
        libc_memcpy(buf, g_prefetch_data->prefetch_buf_ptr + (g_prefetch_data->prefetch_buf_size
                - g_prefetch_data->prefetch_buf_remain), memcpy_cnt);
        g_prefetch_data->prefetch_buf_remain -= memcpy_cnt;
        ret = memcpy_cnt;
        if (memcpy_cnt < 512)
        {
            g_prefetch_data->prefetch_flag = FALSE;
        }
    }
    else
    {
#ifdef WIN32
        ret = fread(buf, 1, count, io->handel);
#else
        ret = (int32) vfs_file_read(file_sys_id, buf, count, io->handel);
#endif
    }

    return ret;
}

int32 mmm_mp_seek(storage_io_t *io, int32 offset, int32 whence)
{
    int32 ret = 0;
#ifdef WIN32
    ret = fseek(io->handel, offset, whence);
#else
    /* 0从文件首往文件尾定位,1  从当前位置往文件尾定位,2从当前位置往文件首定位,3从文件尾往文件首定位 */
    ret = vfs_file_seek(file_sys_id, offset, whence, io->handel);
#endif

    if (g_prefetch_data->prefetch_flag == TRUE)
    {
        g_prefetch_data->prefetch_buf_remain = 0;
    }

    return ret;
}

int32 mmm_mp_tell(storage_io_t *io)
{
    int32 ret = 0;
    int32 offset = 0;
#ifdef WIN32
    offset = ftell(io->handel);
#else
    ret = vfs_file_tell(file_sys_id, &offset, io->handel);
#endif
    return offset;
}

int32 mmm_mp_read_sd(void *buf, int32 size, int32 count, storage_io_t *io)
{
    int32 ret = 0;
    //    if ((size != 1) || (count != 512) || (buf == NULL))
    //    {
    //        return -1;
    //    }

#ifdef WIN32
    ret = fread(buf, 1, count, io->handel);
#else
    ret = (int32) sys_sd_fread(io->handel, buf, count);
#endif

    return ret;
}

int32 mmm_mp_seek_sd(storage_io_t *io, int32 offset, int32 whence)
{
    int32 ret = 0;
#ifdef WIN32
    ret = fseek(io->handel, offset, whence);
#else
    /* 0从文件首往文件尾定位,1  从当前位置往文件尾定位,2从当前位置往文件首定位,3从文件尾往文件首定位 */
    ret = sys_sd_fseek(io->handel, whence, offset);
#endif
    return ret;
}

int32 mmm_mp_tell_sd(storage_io_t *io)
{
    int32 ret = 0;
#ifdef WIN32
    ret = ftell(io->handel);
#else
    ret = (int32) sys_sd_ftell(io->handel);
#endif
    return ret;
}
