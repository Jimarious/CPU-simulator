test_file_name="a2_dependency_test.bin.gz"

for pipewidth in 1 2 3 4 5 8
do
./simulator "$test_file_name" -pipewidth "$pipewidth"
./simulator "$test_file_name" -enableexefwd -pipewidth "$pipewidth"
./simulator "$test_file_name" -enablememfwd -pipewidth "$pipewidth"
./simulator "$test_file_name" -enableexefwd -enablememfwd -pipewidth "$pipewidth"
done