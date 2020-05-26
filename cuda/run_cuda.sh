nvcc -std=c++11 -arch=compute_52 -code=sm_52 main.cu cuda_smith_waterman_skeleton.cu -o cuda_smith_waterman
# num_block=8
# num_thread=512

num_block=16
num_thread=64

input=../datasets/20k3.in
for filename in ../datasets/*
do
  ./cuda_smith_waterman $filename $num_block $num_thread
done;


