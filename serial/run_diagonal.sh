g++ -std=c++11 main.cpp serial_smith_waterman_diagonal.cpp -o serial_smith_waterman
for filename in ../datasets/*
do
    ./serial_smith_waterman $filename
done