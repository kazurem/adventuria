!! @todo Make tests for iterator class 
!! @todo Make makefile that builds docs and test
!! @todo Complete README
!! @todo Refine source code documentation


!! WARNING: TESTS AREN"T COMPLETE YET
program tester
	use stdlib_error, only: check
	use class_IntDoublyLinkedList
	implicit none

	type(IntDoublyLinkedList) :: list
	type(IntDoublyIterator) :: iter
	integer :: returned
	integer :: i
	integer :: val
	logical :: valid
	integer :: nremoved

	integer, parameter :: no_of_tests = 10
	logical :: tests(no_of_tests) = [ &
		.true., & ! clear
		.true., & ! insert
		.true., & ! push_front
		.true., & ! push_back
		.true., & ! remove
		.true., & ! pop_front
		.true., & ! pop_back
		.true., & ! replace
		.true., & ! erase
		.true.  ] ! list iteration

	! 2. clear
	if (tests(1) .eqv. .true.) then
		do i=1,10
			call list%insert(list%begin(), i)
		end do
		call list%clear()
		call check(list%size() == 0)
		call check(validate(list))
		print "(a)", "clear: tests passed!"
	end if

	! 1. insert
	if (tests(2) .eqv. .true.) then
		call list%insert(list%begin(),2) ! when list is empty and iter is null, insert
		call check(list%size() == 1)
		iter = list%begin()
		call check(iter%get_val() == 2)
		call iter%next()
		call list%insert(iter, 4) ! when list is not empty but iter is null 
		call check(list%size() == 2)
		iter = list%begin()
		call iter%next()
		call check(iter%get_val() == 4)
		call iter%prev() ! at head
		call list%insert(iter, 1) ! when list is not empty and iter is at head
		call check(list%size() == 3)
		call iter%prev() ! at head
		call check(iter%get_val() == 1)
		call iter%next()
		call iter%next() ! at tail
		call list%insert(iter, 3) ! insertion in middle (before tail in this case)
		call check(list%size() == 4)
		call iter%prev() ! at before tail
		call check(iter%get_val() == 3)

		call check(validate(list))
		call list%clear()
		print "(a)", "Insert: tests passed!"
	end if

	! 4. push_front
	if (tests(3) .eqv. .true.) then
		call list%push_front(1)
		call check(list%size() == 1)
		iter = list%begin()
		call check(iter%get_val() == 1)
		iter = list%end()
		call check(iter%get_val() == 1)

		call list%push_front(2)
		call check(list%size() == 2)
		iter = list%begin()
		call check(iter%get_val() == 2)
		iter = list%end()
		call check(iter%get_val() == 1)

		call list%push_front(3)
		call check(list%size() == 3)
		iter = list%begin()
		call check(iter%get_val() == 3)
		iter = list%begin()
		call iter%next()
		call check(iter%get_val() == 2)
		iter = list%end()
		call check(iter%get_val() == 1)

		call check(validate(list))
		call list%clear()
		print "(a)", "push_front: tests passed!"
	end if

	! 6. push_back
	if (tests(4) .eqv. .true.) then
		call list%push_back(1)
		call check(list%size() == 1)
		iter = list%begin()
		call check(iter%get_val() == 1)
		iter = list%end()
		call check(iter%get_val() == 1)
		
		call list%push_back(2)
		call check(list%size() == 2)
		iter = list%begin()
		call check(iter%get_val() == 1)
		iter = list%end()
		call check(iter%get_val() == 2)

		call list%push_back(3)
		call check(list%size() == 3)
		iter = list%begin()
		call check(iter%get_val() == 1)
		iter = list%begin()
		call iter%next()
		call check(iter%get_val() == 2)
		iter = list%end()
		call check(iter%get_val() == 3)

		call check(validate(list))
		call list%clear()
		print "(a)", "push_back: tests passed!"
	end if

	! 3. remove
	if (tests(5) .eqv. .true.) then
		call list%push_back(1)
		call list%push_back(2)
		call list%push_back(3)
		call list%push_back(4)
		iter = list%begin()
		call iter%next()
		call list%remove(iter, returned)
		call check(list%size() == 3)
		call check(returned == 2)
		iter = list%begin()
		call list%remove(iter, returned)
		call check(list%size() == 2)
		call check(returned == 1)
		iter = list%end()
		call list%remove(iter, returned)
		call check(list%size() == 1)
		call check(returned == 4)
		iter = list%begin()
		call list%remove(iter, returned)
		call check(list%size() == 0)
		call check(returned == 3)

		call check(validate(list))
		call list%clear()
		print "(a)", "remove: tests passed!"
	end if

	! 5. pop_front
	if (tests(6) .eqv. .true.) then
		call list%push_front(1)
		call list%push_front(2)
		call list%push_front(3)
		call list%pop_front(returned)
		call check(list%size() == 2)
		call check(returned == 3)
		iter = list%begin()
		call check(iter%get_val() == 2)
		iter = list%end()
		call check(iter%get_val() == 1)

		call list%pop_front(returned)
		call check(list%size() == 1)
		call check(returned == 2)
		iter = list%begin()
		call check(iter%get_val() == 1)
		iter = list%end()
		call check(iter%get_val() == 1)

		call list%pop_front(returned)
		call check(list%size() == 0)
		call check(returned == 1)

		call check(validate(list))
		call list%clear()
		print "(a)", "pop_front: tests passed!"
	end if

	! 7. pop_back
	if (tests(7) .eqv. .true.) then
		call list%push_back(1)
		call list%push_back(2)
		call list%push_back(3)
		call list%pop_back(returned)
		call check(list%size() == 2)
		call check(returned == 3)
		iter = list%begin()
		call check(iter%get_val() == 1)
		iter = list%end()
		call check(iter%get_val() == 2)

		call list%pop_back(returned)
		call check(list%size() == 1)
		call check(returned == 2)
		iter = list%begin()
		call check(iter%get_val() == 1)
		iter = list%end()
		call check(iter%get_val() == 1)

		call list%pop_back(returned)
		call check(list%size() == 0)
		call check(returned == 1)

		call check(validate(list))
		call list%clear()
		print "(a)", "pop_back: tests passed!"
	end if

	! 8. replace
	if (tests(8) .eqv. .true.) then
		call list%push_back(2)
		call list%push_back(3)
		call list%push_back(3)
		call list%replace(3, 1)
		iter = list%end()
		call check(iter%get_val() == 1)
		call iter%prev()
		call check(iter%get_val() == 1)

		call check(validate(list))
		call list%clear()
		print "(a)", "replace: tests passed!"
	end if

	! erase
	if (tests(9) .eqv. .true.) then
		call list%push_back(2)
		call list%push_back(1)
		call list%push_back(1)
		call list%erase(1, nremoved)
		call check(list%size() == 1)
		call check(nremoved == 2)
		iter = list%begin()
		call check(iter%get_val() == 2)

		call check(validate(list))
		call list%clear()
		print "(a)", "erase: tests passed!"
	end if

	! iterating through list
	if (tests(10) .eqv. .true.) then
		do i=1,10
			call list%push_back(i)
		end do

		iter = list%begin()
		val = 1
		valid = .true.
		do while (iter%has_next())
			if (val /= iter%get_val()) then
				valid = .false.
			end if
			val = val + 1
			call iter%next()
		end do

		call check(valid)
		print "(a)", "list iteration: tests passed!"
	end if

	print "(a)", "ALL ALLOWED TESTS PASSED!"

	contains
		!> @brief Validate the list by checking whether a
		!! forward pass and backward pass traverse the same
		!! amount of nodes and if that count is equal to the
		!! `no_of_elems`. It also check if the pointers are
		!! pointing to the correct places.
		function validate (list) result(valid)
			type(IntDoublyLinkedList), intent(in) :: list
		    logical :: valid

		    type(IntDoublyNode), pointer :: iter
		    integer :: forward_count
		    integer :: backward_count

		    forward_count = 0
		    backward_count = 0
		    valid = .false.

		    iter => list%head
		    do while (associated(iter))
		        if (associated(iter%next)) then
		            if (.not. associated(iter%next%prev, iter)) return
		        end if
		        forward_count = forward_count + 1
		        iter => iter%next
		    end do

		    iter => list%tail
		    do while (associated(iter))
		        backward_count = backward_count + 1
		        iter => iter%prev
		    end do

		    if ((forward_count /= backward_count) .or. (forward_count /= list%no_of_elems)) return
		    valid = .true.
		end function validate
end program tester