#include "unpack.h"

int decompress(const char* in_buffer, size_t in_size, char** out_buffer, size_t* out_size) {
    z_stream strm;
    int ret;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = in_size;
    strm.next_in = (Bytef*)in_buffer;

    if (inflateInit(&strm) != Z_OK) return Z_ERRNO;

    *out_buffer = malloc(sizeof(0));
    while (1) 
    {
        char buffer[BUFSIZ];
        strm.avail_out = BUFSIZ;
        strm.next_out = (Bytef*)buffer;

        ret = inflate(&strm, Z_NO_FLUSH);

        if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) 
        {
            inflateEnd(&strm);
            return ret;
        }

        size_t have = BUFSIZ - strm.avail_out;

        *out_buffer = (char*)realloc(*out_buffer, *out_size + have);
        memcpy(*out_buffer + *out_size, buffer, have);
        *out_size += have;

        if (ret == Z_STREAM_END) break;
    }

    inflateEnd(&strm);
    return Z_OK;
}


char* load_resource(HINSTANCE hInstance, size_t* size)
{
#ifdef _WIN32

    HRSRC hResInfo = FindResourceA(hInstance, MAKEINTRESOURCE(IDR_ASSETS_PACK1), "ASSETS_PACK");
    if (!hResInfo)
    {
        return NULL;
    }

    HGLOBAL hResData = LoadResource(hInstance, hResInfo);
    if (!hResData)
    {
        return NULL;
    }

    *size = SizeofResource(hInstance, hResInfo);
    const char* data = (const char*)LockResource(hResData);
    
    FreeResource(hResData);

    return data;
#else
    return NULL; // TODO
#endif
}

AssetItem* unpack_assets(HINSTANCE hInstance, size_t* size)
{
    size_t in_size = 0;
    char* in_buffer = load_resource(hInstance, &in_size);

    if (in_buffer == NULL) return NULL;

    size_t out_size = 0;
    char* out_buffer = NULL;
    
    if (decompress(in_buffer, in_size, &out_buffer, &out_size) != Z_OK)
    {
        sfree(in_buffer);
        sfree(out_buffer);

        return NULL;
    }
    sfree(in_buffer);


    AssetItem* items = malloc(sizeof(0));
    if (items == NULL)
    {
        sfree(in_buffer);
        sfree(out_buffer);

        return NULL;
    }

    size_t index = 0;
    while (index < size)
    {
        AssetItem item = { 0 };

        size_t current_size = 0;
        item.name = malloc(sizeof(current_size));
       
        while (index < size)
        {
            item.name = realloc(item.name, current_size + 1);
            item.name[current_size] = out_buffer[index];
              
            current_size++;
            index++;

            if (out_buffer[index] == 0)
            {
                current_size = 0;
                break;
            }
        }

        char* file_size = NULL;
        while (index < size)
        {
            if (out_buffer[index] == 0)
            {
                current_size = 0;
                index++;
                break;
            }

            file_size = realloc(file_size, current_size + 1);
            file_size[current_size] = out_buffer[index];
            current_size++;
            index++;
        }

        item.size = (int)memcpy(item.size, file_size, sizeof(unsigned int));
        
        item.buffer = malloc(sizeof(item.size));
        item.buffer = memcpy(item.buffer, out_buffer + index, item.size);
        index += item.size;

        items = realloc(items, ((*size) + 1) * sizeof(AssetItem));
        memcpy(items + (*size), &item, sizeof(AssetItem));
        (*size)++;
    }

	return items;
}
