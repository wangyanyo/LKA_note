#include "streamer.h"
#include "config.h"
#include "status.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "tool.h"

struct disk_stream *diskstream_new(int disk_id)
{
        struct disk_stream *stream = NULL;
        struct disk *disk = NULL;

        disk = disk_get(disk_id);
        if (!disk)
                goto out;

        stream = kzalloc(sizeof(struct disk_stream));
        if (!stream)
                goto out;

        stream->disk = disk;
        stream->pos = 0;

out:
        return stream;
}

int diskstream_seek(struct disk_stream *stream, int pos)
{
        if (!stream)
                return -EINVAGS;

        stream->pos = pos;
        return 0;
}

int diskstream_read(struct disk_stream *stream, void *out, int total)
{
        char *buf = NULL;
        int ret = KERNEL_ALL_OK;
        int remain = total;
        char *out_buf = (char *)out;

        if (!stream || !out || total < 0) {
                ret = -EINVAGS;
                goto out;
        }

        buf = kmalloc(KERNEL_SECTOR_SIZE);
        if (!buf) {
                ret = -ENOMEM;
                goto out;
        }

        while (remain > 0) {
                int lba = stream->pos / KERNEL_SECTOR_SIZE;
                int offset = stream->pos % KERNEL_SECTOR_SIZE;
                int read_size = min_t(int, KERNEL_SECTOR_SIZE - offset, remain);

                ret = disk_read_block(stream->disk, lba, 1, buf);
                if (ret != KERNEL_ALL_OK) {
                        goto out;
                }

                memcpy(out_buf, buf + offset, read_size);

                out_buf += read_size;
                stream->pos += read_size;
                remain -= read_size;
        }

out:
        if (buf)
                kfree(buf);
        return ret;
}

void diskstream_close(struct disk_stream *stream)
{
        if (!stream)
                return;

        kfree(stream);
}