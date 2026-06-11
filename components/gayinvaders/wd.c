#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "wd.h"
#include "gayinvaders.h"

#define TABLE_START 4

static size_t _wd_file_size;
static size_t _data_start;
static asset_info_t _asset_table[ASSET_TYPE_COUNT];

static const char *_asset_names[ASSET_TYPE_COUNT] = {
	"INTRO",
	"LEFTSIDE",
	"RIGHTSIDE",
	"PLAYER",
	"BULLET"
};

static const char *_wd_filepath;


static asset_type_t _name_to_asset_type(char *name)
{
	int i;

	for (i = 0; i < ASSET_TYPE_COUNT; ++i)
		if (strcmp(name, _asset_names[i]) == 0)
			return i;
	
	return ASSET_TYPE_NONE;
}

int wd_init(const char *wd_fpath)
{
	uint32_t num_of_assets;
	uint32_t mark;
	FILE *f;
	int i;

	_wd_filepath = wd_fpath;

	f = fopen(wd_fpath, "r");
	if (!f) {
		printf("No WD file!\n");
		return -EIO;
	}

	fread(&mark, 1, 4, f);

	if (mark != 0xDEADBEEF) {
		printf("No WD file mark in start of file!\n");
		return -EINVAL;
	}

	fread(&num_of_assets, 1, 4, f);

	if (num_of_assets != ASSET_TYPE_COUNT) {
		printf("Number of assets is not correct: %d != %d\n", num_of_assets, ASSET_TYPE_COUNT);
		return -EINVAL;
	}

	_data_start = 8;

	printf("Assets:\n");
	for (i = 0; i < ASSET_TYPE_COUNT; ++i) {
		asset_type_t ass_type;
		asset_info_t *ass;
		uint8_t name_sz;
		char name[64];

		fread(&name_sz, 1, 1, f);
		fread(&name, 1, name_sz, f);
		name[name_sz] = 0;

		ass_type = _name_to_asset_type(name);

		if (ass_type == ASSET_TYPE_NONE) {
			printf("Unknown asset type \'%s'\n", name);
			return -EINVAL;
		}

		ass = &_asset_table[ass_type];

		/* Fill asset */
		memcpy(ass->name, name, name_sz+1);
		ass->name_size = name_sz;
		fread(&ass->offset, 1, 4, f);
		fread(&ass->size, 1, 4, f);
		fread(&ass->w, 1, 4, f);
		fread(&ass->h, 1, 4, f);

		_data_start += ass->name_size + 17;

		printf("%s: %d %d\n", ass->name, ass->w, ass->h);
	}

	fclose(f);
	
	return 0;
}

const asset_info_t *wd_get_asset_info(asset_type_t atype)
{
	return &_asset_table[atype];
}

int wd_read_asset(asset_type_t atype, uint16_t *buffer, int offset, int sz)
{
	asset_info_t *ass = &_asset_table[atype];
	uint8_t *buff;
	int x, y;
	FILE *f;

	f = fopen(_wd_filepath, "r");
	if (!f) {
		printf("No WD file!\n");
		return -EIO;
	}

	fseek(f, _data_start + ass->offset + offset, SEEK_SET);

	buff = gayinvaders_malloc(sz);

	fread(buff, 1, sz, f);

	for (x = 0; x < ass->w; ++x) {
		for (y = 0; y < ass->h; ++y) {
			int index = (y*ass->w+x)*3;
			uint8_t r = buff[index+0] / 8;
			uint8_t g = buff[index+1] / 8;
			uint8_t b = buff[index+2] / 8;

			uint16_t pix = (r & 0x1f) | ((g & 0x3f)<<5) | ((b & 0x1f)<<11);

			buffer[y*ass->w+x] = ((pix&0xFF)<< 8) | (pix >> 8);
		}
	}

	fclose(f);

	gayinvaders_free(buff);

	return 0;
}
