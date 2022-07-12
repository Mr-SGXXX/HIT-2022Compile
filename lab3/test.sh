rm -f log.txt
for i in $(find test/ -name "*.c")
do
echo "${i%.*}:" >> log.txt
./scanner i >> log.txt
done