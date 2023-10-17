#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define smart_ptr_make_unique(...) sp__smart_ptr_make_unique(__VA_ARGS__, __FILE__, __func__, __LINE__)
#define smart_ptr_set(...) sp__smart_ptr_set(__VA_ARGS__, __FILE__, __func__, __LINE__)
#define smart_ptr_get(...) sp__smart_ptr_get(__VA_ARGS__, __FILE__, __func__, __LINE__)
#define smart_ptr_resize(...) sp__smart_ptr_resize(__VA_ARGS__, __FILE__, __func__, __LINE__)
#define smart_ptr_free(...) sp__smart_ptr_free(__VA_ARGS__, __FILE__, __func__, __LINE__)
#define smart_ptr_throw(...) sp__smart_ptr_throw(__VA_ARGS__, __FILE__, __func__, __LINE__)
#define smart_ptr_catch(...) sp__smart_ptr_catch(__VA_ARGS__, __FILE__, __func__, __LINE__)

typedef struct sp__smart_ptr *smart_ptr;

struct sp__smart_ptr {
	struct sp__smart_ptr **owner;
	void *data;
	size_t size;
};

/* Return an empty pointer, should only be used with smart_ptr_make_unique() */
inline static struct sp__smart_ptr *smart_ptr_init()
{
	struct sp__smart_ptr *ptr = (struct sp__smart_ptr *)malloc(sizeof(struct sp__smart_ptr));
	memset(ptr, 0, sizeof(struct sp__smart_ptr));
	return ptr;
}

/* Bind the pointer and allocate size */
inline static void sp__smart_ptr_make_unique(struct sp__smart_ptr **owner, size_t size, const char *file, const char *func, int line)
{
	struct sp__smart_ptr *ptr = *owner;
	if (ptr == NULL) {
		fprintf(stderr, "%s:%s():%d: uninitialize pointer, cannot make unique\n", file, func, line);
		abort();
	}
	static struct sp__smart_ptr zero;
	if (memcmp(ptr, &zero, sizeof(struct sp__smart_ptr)) != 0) {
		fprintf(stderr, "%s:%s():%d: pointer '%p' is not empty, cannot make unique\n", file, func, line, ptr);
		abort();
	}
	*ptr = (struct sp__smart_ptr){
		.owner = owner,
		.data = malloc(size),
		.size = size
	};
}

/* Copy .size bytes of data into the allocated memory */
inline static void sp__smart_ptr_set(struct sp__smart_ptr **owner, void *data, const char *file, const char *func, int line)
{
	struct sp__smart_ptr *ptr = *owner;
	if (ptr == NULL) {
		fprintf(stderr, "%s:%s():%d: uninitialize pointer, cannot set value\n", file, func, line);
		abort();
	}
	if (ptr->owner != owner) {
		fprintf(stderr, "%s:%s():%d: invalid ownership of '%p', cannot set value\n", file, func, line, ptr);
		abort();
	}
	memcpy(ptr->data, data, ptr->size);
}

/* Copy .size bytes of the allocated memory into data */
inline static void sp__smart_ptr_get(struct sp__smart_ptr **owner, void *data, const char *file, const char *func, int line)
{
	struct sp__smart_ptr *ptr = *owner;
	if (ptr == NULL) {
		fprintf(stderr, "%s:%s():%d: uninitialize pointer, cannot retrieve value\n", file, func, line);
		abort();
	}
	if (ptr->owner != owner) {
		fprintf(stderr, "%s:%s():%d: invalid ownership of '%p', cannot retrieve value\n", file, func, line, ptr);
		abort();
	}
	memcpy(data, ptr->data, ptr->size);
}

/* Change the size of the memory block pointed to by ptr to size bytes
 * Behave similarly to realloc */
inline static void sp__smart_ptr_resize(struct sp__smart_ptr **owner, size_t size, const char *file, const char *func, int line)
{
	struct sp__smart_ptr *ptr = *owner;
	if (ptr == NULL) {
		fprintf(stderr, "%s:%s():%d: uninitialize pointer, cannot retrieve value\n", file, func, line);
		abort();
	}
	if (ptr->owner != owner) {
		fprintf(stderr, "%s:%s():%d: invalid ownership of '%p', cannot retrieve value\n", file, func, line, ptr);
		abort();
	}
	ptr->size = size;
	ptr->data = realloc(ptr->data, ptr->size);
}

/* Unbind the pointer and free its data
 * Should be thrown away afterwards */
inline static void sp__smart_ptr_free(struct sp__smart_ptr **owner, const char *file, const char *func, int line)
{
	struct sp__smart_ptr *ptr = *owner;
	if (ptr == NULL) {
		fprintf(stderr, "%s:%s():%d: pointer is already freed, cannot free\n", file, func, line);
		abort();
	}
	if (ptr->owner != owner) {
		fprintf(stderr, "%s:%s():%d: invalid ownership of '%p', cannot free\n", file, func, line, ptr);
		abort();
	}
	free(ptr->data);
	free(ptr);
	*owner = NULL;
}

/* Use when passing to functions
 * Thrown smart pointers are not settable, gettable, or freeable */
inline static struct sp__smart_ptr *sp__smart_ptr_throw(struct sp__smart_ptr **owner, const char *file, const char *func, int line)
{
	struct sp__smart_ptr *ptr = *owner;
	if (ptr == NULL) {
		fprintf(stderr, "%s:%s():%d: uninitialize pointer, cannot throw\n", file, func, line);
		abort();
	}
	if (ptr->owner != owner) {
		fprintf(stderr, "%s:%s():%d: invalid ownership of '%p', cannot throw\n", file, func, line, ptr);
		abort();
	}
	ptr->owner = NULL;
	return *owner;
}

/* Catch thrown smart pointers and bind them to a new unique pointer
 * Generally used at the beginning of functions to catch thrown pointers */
inline static void sp__smart_ptr_catch(struct sp__smart_ptr **owner, const char *file, const char *func, int line)
{
	struct sp__smart_ptr *ptr = *owner;
	if (ptr == NULL) {
		fprintf(stderr, "%s:%s():%d: uninitialize pointer, cannot catch\n", file, func, line);
		abort();
	}
	if (ptr->owner != NULL) {
		fprintf(stderr, "%s:%s():%d: invalid ownership of '%p', cannot catch\n", file, func, line, ptr);
		abort();
	}
	ptr->owner = owner;
}
