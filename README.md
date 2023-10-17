# Smart Pointer (smart_ptr)
The Smart Pointer is a C library that provides a mechanism for managing memory and resources in a safe and efficient way. It allows you to allocate and deallocate memory dynamically while ensuring that the memory is properly managed.

## Overview
This is a header-only library designed to improve the safety of memory management in C by providing a set of functions for creating, setting, getting, and freeing dynamically allocated memory. It ensures that memory is allocated, freed, and accessed in a safe and controlled manner, similar to `std::unique_ptr<>` in C++.

Additionally, this Smart Pointer library produces useful error messages, such as:
```
main.c:print_value():6: invalid ownership of '0x7ffcac4997e0', cannot catch
```
or
```
main.c:main():36: pointer is already freed, cannot free
```

## Usage
To use the Smart Pointer library in your C program, follow these steps:

1. Include the `smart_ptr.h header` file in your source code.
```c
#include "smart_ptr.h"
```
2. Use the provided functions to create, set, get, and free smart pointers as needed in your program.

3. Make sure to properly initialize and manage smart pointers using the provided functions.

## Functions
The Smart Pointer library offers the following functions:

`smart_ptr smart_ptr_init()`: Return an empty pointer. This function should only be used with smart_ptr_make_unique().

`void smart_ptr_make_unique(smart_ptr *owner, size_t size)`: Bind the pointer and allocate memory.

`void smart_ptr_set(smart_ptr *ptr, void *data)`: Copy data into the allocated memory.

`void smart_ptr_get(smart_ptr *ptr, void *data)`: Copy the allocated memory into data.

`void smart_ptr_free(smart_ptr *ptr)`: Unbind the pointer and free its data. The pointer should be thrown away afterward.

`smart_ptr smart_ptr_throw(smart_ptr *ptr)`: Use when passing smart pointers to functions. Thrown smart pointers are not settable, gettable, or freeable.

`void smart_ptr_catch(smart_ptr *ptr)`: Catch thrown smart pointers and bind them to a new unique pointer. Generally used at the beginning and outside of functions to catch thrown pointers.

## Example
```c
#include "smart_ptr.h"

void print_value(smart_ptr p)
{
	/* Bind the pointer's ownership to `p` */
	smart_ptr_catch(&p);

	int i;
	if (p->size < sizeof(int)) {
		return;
	}

	/* Write the data held by `p` to `i` */
	smart_ptr_get(&p, &i);
	smart_ptr_throw(&p);
	printf("%d\n", i);
}

int main()
{
	/* Fill `p` with an empty pointer */
	smart_ptr p = smart_ptr_init();
	
	/* Allocate and bind the memory to `p` */
	smart_ptr_make_unique(&p, sizeof(int));
	
	/* Write to the memory of `p` */
	smart_ptr_set(&p, (int[]){1});

	/* Pass the pointer to `print_value()` and lease its ownership */
	print_value(smart_ptr_throw(&p));

	/* Regain the lost ownership */
	smart_ptr_catch(&p);

	/* Don't forget to free the memory */
	smart_ptr_free(&p);
	
	return 0;
}
```

## License
This Smart Pointer library is released under the BSD Zero Clause License. You are free to use, modify, and distribute it as needed.
