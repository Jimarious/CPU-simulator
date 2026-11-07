test_file_name="a2_dependency_test.bin.gz"

./simulator "$test_file_name"
./simulator "$test_file_name" -enableexefwd
./simulator "$test_file_name" -enablememfwd
./simulator "$test_file_name" -enableexefwd -enablememfwd