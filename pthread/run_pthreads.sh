g++ -std=c++11 -lpthread main.cpp pthreads_smith_waterman_skeleton.cpp -o pthreads_smith_waterman
num=8
for filename in ../datasets/*
do
    ./pthreads_smith_waterman $filename $num
done;

