n = 20

make: fileTest dirTest
	gcc -g thang_le_assignment1.c -o thang_le_assignment1

fileTest:
	mkdir fileTest
	cd fileTest; \
	for i in {0..$n} ; do \
		echo "" > "file$${i}.txt" ; \
	done

dirTest:
	mkdir dirTest
	cd dirTest; \
	for i in {0..$n} ; do \
		mkdir "dir$${i}" ; \
	done

clean:
	rm -rf thang_le_assignment1 dirTest fileTest
