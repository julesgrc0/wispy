#include "chunk.h"

Chunk *generate_chunk(Image noise, unsigned int position)
{
  Chunk *chunk = malloc(sizeof(Chunk));
  if (!chunk)
    return NULL;

  chunk->position = position;

  unsigned int index = 0;
  for (unsigned int x = 0; x < CHUNK_WIDTH; x++)
  {
    unsigned int lineHeight =
        GetImageColor(noise, position * CHUNK_WIDTH + x, 0).r / 5; // 5
    unsigned int level = 0;

    for (unsigned int y = 0; y < CHUNK_HEIGHT; y++)
    {
      BlockTypes type = B_NONE;
      if (y >= lineHeight)
      {
        type = B_GRASS;
        if (level > 0)
        {
          unsigned int value =
              GetImageColor(noise, position * CHUNK_WIDTH + x, y).r;
          if (level < 10 || value <= 80)
          {
            type = B_DIRT;
          }
          else
          {
            type = B_STONE;
          }
        }

        level++;
      }

      chunk->blocks[index] = (Block){.x = x, .y = y, .type = type};
      index++;
    }
  }

  return chunk;
}

Chunk *load_chunk(char *map_name, unsigned int position)
{
  char path[MAX_PATH * 2];
  path[0] = 0;
  strcat(path, GetApplicationDirectory());
  strcat(path, "maps\\");
  strcat(path, map_name);
  strcat(path, "\\");

  char filename[MAX_PATH];
  filename[0] = 0;
  sprintf(filename, "%u.dat", position);
  strcat(path, filename);

  if (!FileExists(path))
    return NULL;

  size_t in_size = 0;
  char *in_data = LoadFileData(path, &in_size);
  if (!in_data)
    return NULL;

  size_t out_size = in_size * 2;
  char *out_data = malloc(out_size);
  if (!out_data)
  {
    sfree(in_data);
    return NULL;
  }

  while (uncompress(out_data, &out_size, in_data, (uLong)in_size) != Z_OK)
  {
    out_size += in_size;
    void *tmp = realloc(out_data, out_size);
    if (!tmp)
    {
      sfree(out_data);
      sfree(in_data);
    }
    out_data = tmp;
  }

  Chunk *chunk = malloc(sizeof(Chunk));

  size_t offset = 0;
  memcpy(&chunk->position, out_data + offset, sizeof(unsigned int));
  offset += sizeof(unsigned int);

  for (unsigned int i = 0; i < CHUNK_LEN; i++)
  {
    memcpy(&chunk->blocks[i], out_data + offset, sizeof(Block));
    offset += sizeof(Block);
  }

  sfree(in_data);
  sfree(out_data);

  return chunk;
}

void export_chunk(char *map_name, Chunk *chunk)
{
  size_t in_size = sizeof(unsigned int) + sizeof(Block) * CHUNK_LEN;
  char *in_data = (char *)malloc(in_size);
  if (!in_data)
    return;

  size_t offset = 0;
  memcpy(in_data + offset, &(chunk->position), sizeof(unsigned int));
  offset += sizeof(unsigned int);

  for (unsigned int i = 0; i < CHUNK_LEN; i++)
  {
    memcpy(in_data + offset, &(chunk->blocks[i]), sizeof(Block));
    offset += sizeof(Block);
  }

  size_t out_size = in_size;
  char *out_data = malloc(out_size);

  if (compress(out_data, &out_size, in_data, (uLong)in_size) != Z_OK)
  {
    sfree(in_data);
    sfree(out_data);
    return;
  }

  char path[MAX_PATH * 2];
  path[0] = 0;

#ifdef _WIN32

  strcat(path, GetApplicationDirectory());
  strcat(path, "maps\\");
  if (!DirectoryExists(path))
    CreateDirectoryA(path, NULL);

  strcat(path, map_name);
  strcat(path, "\\");
  if (!DirectoryExists(path))
    CreateDirectoryA(path, NULL);

#endif // _WIN32

  char filename[MAX_PATH];
  filename[0] = 0;
  sprintf(filename, "%u.dat", chunk->position);
  strcat(path, filename);

  SaveFileData(path, out_data, (unsigned int)out_size);

  sfree(in_data);
  sfree(out_data);
}
