/*
 * Copyright © 2018 Mingliang Zeng <zengmingliang1998@gmail.com>
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */
#define _OSH_FS_VERSION 2147483647
#define FUSE_USE_VERSION 26
#define HACK
#ifdef HACK
#define OFFSET_QAQ 100
#define malloc mymalloc
#define realloc myrealloc
#define free myfree
#endif
#define DEBUG
#undef DEBUG
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fuse.h>
#include <sys/mman.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

struct filenode
{
	char *filename;
	void *content;
	struct stat *st;
	struct filenode *next;
};

static const size_t size = 4 * 1024 * 1024 * (size_t)1024;
static void *mem[64 * 1024]; // 所有东西都存在这里面 QAQ

#ifdef HACK
void *mymalloc(size_t sz)
{
#ifdef DEBUG
	printf("malloc: %lu\n", sz);
#endif
	size_t blocknr = sizeof(mem) / sizeof(mem[0]);
	size_t blocksize = size / blocknr;
	size_t p = 0, b;
	while ((b = *((size_t *)((char *)mem[p] + OFFSET_QAQ))) != 0 && (*((size_t *)((char *)mem[p] + OFFSET_QAQ * 2)) == 0 || b / blocksize < sz / blocksize))
		p += b / blocksize + 2;
	if (p + sz / blocksize + 2 >= blocknr)
	{
#ifdef DEBUG
		printf("failed: QAQ\n");
#endif
		return NULL; // no enough space now
	}
	if (b)
	{
		*((size_t *)((char *)mem[p] + OFFSET_QAQ * 2)) = 0; // overwrite deleted file
		/*
		if(b/blocksize > sz/blocksize)
		{
			*((size_t *)((char *)mem[p + sz / blocksize + 2] + OFFSET_QAQ)) = (b/blocksize - sz/blocksize - 2)*blocksize;
			*((size_t *)((char *)mem[p + sz / blocksize + 2] + OFFSET_QAQ * 2)) = 1;
		}
		*/
	}
	else
	{
		for(int i = 1; i <= sz / blocksize + 2; i++)
			mem[p + i] = mmap(mem[p + i], blocksize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); // allocate memory
		*((size_t *)((char *)mem[p] + OFFSET_QAQ)) = sz;
	}
#ifdef DEBUG
	printf("success: %p\n", mem[p + 1]);
#endif
	return mem[p + 1];
}

void myfree(void *ptr)
{
#ifdef DEBUG
	printf("free: %p\n", ptr);
#endif
	size_t blocknr = sizeof(mem) / sizeof(mem[0]);
	size_t blocksize = size / blocknr;
	if (ptr != NULL)
	{
		*((size_t *)((char *)ptr - blocksize + OFFSET_QAQ * 2)) = 1; // deleted mark
		size_t sz = *(size_t *)((char *)ptr - blocksize + OFFSET_QAQ);
	}
	return;
}

void *myrealloc(void *ptr, size_t sz)
{
#ifdef DEBUG
	printf("realloc: %p %lu\n", ptr, sz);
#endif
	size_t blocknr = sizeof(mem) / sizeof(mem[0]);
	size_t blocksize = size / blocknr;
	size_t *psz = (size_t *)((char *)ptr - blocksize + OFFSET_QAQ);
	if (ptr == NULL || sz / blocksize > (*psz) / blocksize)
	{
		if (ptr == NULL || (*((size_t *)(ptr + blocksize * ((*psz) / blocksize + 1) + OFFSET_QAQ))) != 0)
		{
			void *qwq = mymalloc(sz);
			memcpy(qwq, ptr, ptr == NULL ? 0 : *psz); // move memory
			myfree(ptr);
			return qwq;
		}
		else
		{
			int p = (ptr - mem[0]) / blocksize;
			for (int i = *psz / blocksize + 2; i <= sz / blocksize + 2; i++)
				mem[p + i] = mmap(mem[p + i], blocksize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); // reallocate memory
			*psz = sz;
			return ptr;
		}
	}
	else
	{
		return ptr;
	}
}
#endif


static struct filenode *get_filenode(const char *name)
{
	struct filenode **root = (struct filenode **)((char *)mem[0] + OFFSET_QAQ * 3);
	struct filenode *node = *root;
	while (node)
	{
		if (strcmp(node->filename, name + 1) != 0)
			node = node->next;
		else
			return node;
	}
	return NULL;
}

static void create_filenode(const char *filename, const struct stat *st)
{
	struct filenode **root = (struct filenode **)((char *)mem[0] + OFFSET_QAQ * 3);
	struct filenode *new = (struct filenode *)malloc(sizeof(struct filenode));
	new->filename = (char *)malloc(strlen(filename) + 1);
	memcpy(new->filename, filename, strlen(filename) + 1);
	new->st = (struct stat *)malloc(sizeof(struct stat));
	memcpy(new->st, st, sizeof(struct stat));
	new->next = *root;
	new->content = NULL;
	*root = new;
}

static void delete_filenode(struct filenode *fn)
{
	myfree(fn->filename);
	myfree(fn->content);
	myfree(fn->st);
	myfree(fn);
}

static void *oshfs_init(struct fuse_conn_info *conn)
{
	size_t blocknr = sizeof(mem) / sizeof(mem[0]);
	size_t blocksize = size / blocknr;
	mem[0] = mmap(NULL, blocksize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	for (int i = 0; i < blocknr; i++)
	{
		mem[i] = (char *)mem[0] + blocksize * i;
	}
	mem[1] = mmap(mem[1], blocksize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	mem[2] = mmap(mem[2], blocksize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	struct filenode **root = (struct filenode **)((char *)mem[0] + OFFSET_QAQ * 3);
	*root = NULL;
	return NULL;
}

static int oshfs_getattr(const char *path, struct stat *stbuf)
{
	int ret = 0;
	struct filenode *node = get_filenode(path);
	if (strcmp(path, "/") == 0)
	{
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = S_IFDIR | 0755;
	}
	else if (node)
	{
		memcpy(stbuf, node->st, sizeof(struct stat));
	}
	else
	{
		ret = -ENOENT;
	}
	return ret;
}

static int oshfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	struct filenode **root = (struct filenode **)((char *)mem[0] + OFFSET_QAQ * 3);
	struct filenode *node = *root;
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	while (node)
	{
		filler(buf, node->filename, node->st, 0);
		node = node->next;
	}
	return 0;
}

static int oshfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	struct stat st;
	st.st_mode = S_IFREG | 0644;
	st.st_uid = fuse_get_context()->uid;
	st.st_gid = fuse_get_context()->gid;
	st.st_nlink = 1;
	st.st_size = 0;
	create_filenode(path + 1, &st);
	return 0;
}

static int oshfs_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

static int oshfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	struct filenode *node = get_filenode(path);
	if (offset + size > node->st->st_size)
		node->st->st_size = offset + size;
	node->content = realloc(node->content, node->st->st_size);
	memcpy(node->content + offset, buf, size);
	return size;
}

static int oshfs_truncate(const char *path, off_t size)
{
	struct filenode *node = get_filenode(path);
	node->st->st_size = size;
	node->content = realloc(node->content, size);
	return 0;
}

static int oshfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	struct filenode *node = get_filenode(path);
	int ret = size;
	if (offset + size > node->st->st_size)
		ret = node->st->st_size - offset;
	memcpy(buf, node->content + offset, ret);
	return ret;
}

static int oshfs_unlink(const char *path)
{
	struct filenode **root = (struct filenode **)((char *)mem[0] + OFFSET_QAQ * 3);
	struct filenode *node = *root;
	if (node == NULL)
		return 0;
	if (strcmp(node->filename, path + 1) == 0)
	{
		*root = node->next;
		delete_filenode(node);
		return 0;
	}
	while (node->next)
	{
		if (strcmp(node->next->filename, path + 1) != 0)
			node = node->next;
		else
		{
			struct filenode *ptr = node->next;
			node->next = node->next->next;
			delete_filenode(ptr);
			return 0;
		}
	}
	return 0;
}

static const struct fuse_operations op = {
	.init = oshfs_init,
	.getattr = oshfs_getattr,
	.readdir = oshfs_readdir,
	.mknod = oshfs_mknod,
	.open = oshfs_open,
	.write = oshfs_write,
	.truncate = oshfs_truncate,
	.read = oshfs_read,
	.unlink = oshfs_unlink,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &op, NULL);
}
