# Doubly Linked List in Fortran
A doubly linked list implementation in fortran. Supported types are:
1. Complex
2. Integer
3. Real
## TODO
1. Make `remove` return iterator to next node.
2. Adding a special end node which `list%end()` will return. This is akin to end() in C++.
3. Check the minimum version of fortran this code works in.
4. Remove duplication (especially in remove and pop functions.). Maybe add `destroy_node()` and `unlink_node()` subroutine.
5. Add functionality for allowing users to provide a custom equality function (Callbacks will probably be needed here).
6. Regarding source code documentation, add links to other parts of the documentation using directives.
## Requirements
1. [gfortran](https://gcc.gnu.org/fortran/) (compiler)
2. [make](https://www.gnu.org/software/make/) (build system)
3. [fypp](https://github.com/aradi/fypp) (for preprocessing)
4. [ford](https://github.com/Fortran-FOSS-Programmers/ford) (for testing)
## Running tests
Tests can be run using the following command.
```bash
make run
```
## Generating docs
You can generate docs by running the following command
```
make docs
```
You can then open `doc/index.html` in your browser
## Usage
### Pushing elements
```f90
use class_IntDoublyLinkedList

type(IntDoublyIterator)   :: iter
type(IntDoublyLinkedList) :: list

call list%push_front(1) ! pushes element to the front. O(1)
call list%push_back(3)  ! pushes element to the end. O(1)
call list%push_back(4)

iter = list%begin() ! begin() returns iterator pointing to list head
call iter%next() ! makes iterator point to next node
call list%insert(iter, 2) ! inserts a new node before iter. O(1)

! final list: {1, 2, 3, 4}
```
### Removing elements
```f90
use class_IntDoublyLinkedList

type(IntDoublyIterator)   :: iter
type(IntDoublyLinkedList) :: list
integer :: returned

do i=1,5
	call list%push_back(i)
end do
! list = {1, 2, 3, 4, 5}

call list%pop_front(returned) ! pops element from front (head) and puts it in given variable. O(1)
print *, returned ! output: 1 
! list = {2, 3, 4, 5}

call list%pop_back(returned) ! pops element from end (tail) and returns it. O(1)
print *, returned ! output: 5
! list = {2, 3, 4}

iter = list%begin() ! points to 2
call iter%next() ! points to 3
call list%remove(iter, returned) ! removes node pointed to by iter. O(1)
print *, returned ! output: 3
! final list: {2, 4}
```
### Iterating through the list
```f90
use class_IntDoublyLinkedList

type(IntDoublyIterator)   :: iter
type(IntDoublyLinkedList) :: list

do i=1,5
	call list%push_back(i)
end do
! list = {1, 2, 3, 4, 5}

! prints list in reverse
iter = list%end() ! points to tail node
do while(iter%has_next()) ! has_next() is true when iter is associated to a memory location
	print *, iter%get_val()
	call iter%prev()
end do
```
## Some other things...
1. It seems we can use XOR to make nodes store information about both `next` and `prev` nodes at the same time.  
This will reduce storage required but increase runtime processing cost since every insert or remove will require  
decoding.
2. There is something called [Unrolled Linked List](https://en.wikipedia.org/wiki/Unrolled_linked_list) which can improve cache locality.
