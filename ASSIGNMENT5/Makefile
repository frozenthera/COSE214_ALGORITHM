CC = gcc

.c.o: 
	$(CC) -c $<

all: huffman_encoder huffman_decoder

huffman_encoder: huffman_encoder.o huffman.o heap.o
	$(CC) -o $@ huffman_encoder.o huffman.o heap.o

huffman_decoder: huffman_decoder.o huffman.o heap.o
	$(CC) -o $@ huffman_decoder.o huffman.o heap.o

clean:
	rm -f *.o
	rm -f huffman_encoder
	rm -f huffman_decoder
