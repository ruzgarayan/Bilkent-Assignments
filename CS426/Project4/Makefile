all:
	nvcc -std=c++14 -I . -o kmer-parallel kmer-parallel.cu util.cu
	nvcc -std=c++14 -I . -o kmer-serial kmer-serial.cu util.cu
