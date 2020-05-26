mpic++ -std=c++11 main.cpp mpi_smith_waterman_skeleton.cpp -o mpi_smith_waterman
num=4
for filename in ../datasets/*
do
    mpiexec -n $num --hostfile ~/hostfile ./mpi_smith_waterman $filename
done;
