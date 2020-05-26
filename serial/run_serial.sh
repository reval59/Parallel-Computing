g++ -std=c++11 main.cpp serial_smith_waterman.cpp -o serial_smith_waterman
for filename in ../datasets/*
do
    ./serial_smith_waterman $filename
done;
