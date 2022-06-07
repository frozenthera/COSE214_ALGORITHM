#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "heap.h"

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree( tNode *root, char *code, int depth, char *codes[]);

// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode *newNode(unsigned char data, int freq);

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드를 화면에 출력
void print_huffman_code( char *codes[])
{
	int i;
	
	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%s\n", i, codes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 트리로부터 허프만 코드를 생성
// traverse_tree 함수 호출
void make_huffman_code( tNode *root, char *codes[])
{
	char code[128];
	
	traverse_tree( root, code, 0, codes);
}

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree( tNode *root, char *code, int depth, char *codes[]){
	if(root->left == NULL && root->right == NULL) codes[root->data] = strdup( code);
	if(root->left != NULL){
		code[depth] = '0';
		code[depth+1] = '\0';
		traverse_tree(root->left, code, depth+1, codes);
	}
	if(root->right != NULL){
		code[depth] = '1';
		code[depth+1] = '\0';
		traverse_tree(root->right, code, depth+1, codes);
	}
}

// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode *newNode(unsigned char data, int freq){
	tNode *ptr = (tNode *)malloc(sizeof(tNode));
	ptr->data = data;
	ptr->freq = freq;
	ptr->left = NULL;
	ptr->right = NULL;
	return ptr;
}

// 파일에 속한 각 문자(바이트)의 빈도 저장
// return value : 파일에서 읽은 바이트 수
int read_chars( FILE *fp, int ch_freq[]){
	int num = 0;
	
	fseek(fp, 0, SEEK_END);
	int byteSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	for(int i=0; i<byteSize; i++){
		ch_freq[getc(fp)]++;
	}
	
	return byteSize;
}

// 파일로부터 문자별 빈도(256개)를 읽어서 ch_freq에 저장
void get_char_freq( FILE *fp, int ch_freq[]){
	fread(ch_freq, sizeof(int), 256, fp);
}

// 허프만 트리를 생성
// 1. capacity 256 짜리 빈(empty) 힙 생성 // HEAP *heap = heapCreate( 256);
// 2. 개별 알파벳에 대한 노드 생성
// 3. 힙에 삽입 (minheap 구성) // heapInsert( heap, node);
// 4. 2개의 최소값을 갖는 트리 추출 // tNode *left = heapDelete( heap); tNode *right = heapDelete( heap);
// 5. 두 트리를 결합 후 새 노드에 추가
// 6. 새 트리를 힙에 삽입
// 7. 힙에 한개의 노드가 남을 때까지 반복
// return value: 트리의 root 노드의 포인터
tNode *make_huffman_tree( int ch_freq[]){
	HEAP *heap = heapCreate(256);
	for(int i=0; i<256; i++){
		heapInsert( heap, newNode( i, ch_freq[i]));
	}
	while(heap->last != 0){
		tNode *left = heapDelete( heap);
		tNode *right = heapDelete( heap);
		
		tNode *temp = newNode(0, 0);
		temp->left = left;	
		temp->right = right;
		temp->freq = left->freq + right->freq;
	
		heapInsert( heap, temp);
	}
	tNode* ret = heapDelete( heap);
	heapDestroy(heap);
	return ret;
}

// 허프만 코드에 대한 메모리 해제
void free_huffman_code( char *codes[]){
	for(int i=0; i< 256; i++){
		free( codes[i]);
	}
}

// 허프만 트리 메모리 해제
void destroyTree( tNode *root){
	if(root->left != NULL) destroyTree(root->left);
	if(root->right != NULL) destroyTree(root->right);
	root->data = 0;
	root->freq = 0;
	free(root);
}

// 입력 텍스트 파일(infp)을 허프만 코드를 이용하여 출력 파일(outfp)로 인코딩
// return value : 인코딩된 텍스트의 바이트 수 (파일 크기와는 다름)
int encoding( char *codes[], int ch_freq[], FILE *infp, FILE *outfp){
	//outfp에 빈도수 저장
	fwrite(ch_freq, sizeof(int), 256, outfp);
	int size = 0, checker = 0;
	unsigned char input = 0;
	
	fseek(infp, 0, SEEK_END);
	int byteSize = ftell(infp);
	fseek(infp, 0, SEEK_SET);
	
	for(int i=0; i < byteSize; i++){
		unsigned char cur = getc(infp);
		char *temp = codes[cur];
		int len = 0;
		while(temp[len] != '\0'){
			
			input = input << 1;
			input |= temp[len] == '0' ? 0 : 1; 
			len++;
			checker++;
			
			if(checker == 8){
				fwrite(&input, sizeof(unsigned char), 1, outfp);
				checker = 0;
				input = 0;
				size++;
			}
		}
	}
	unsigned int ret = size * 8 + checker;
	if(checker != 0){
		input = input << 8-checker;
		fwrite(&input, sizeof(unsigned char), 1, outfp);
		size++;
	}
	fwrite(&ret, sizeof(unsigned int), 1, outfp);
	return size;
}

// 입력 파일(infp)을 허프만 트리를 이용하여 텍스트 파일(outfp)로 디코딩
void decoding( tNode *root, FILE *infp, FILE *outfp){
	
	int bitSize = 0;
	fseek(infp, -sizeof(int), SEEK_END);
	fread(&bitSize, sizeof(int), 1, infp);
	fseek(infp, sizeof(int) * 256, SEEK_SET);
	tNode *curNode = root;
	
	while(bitSize > 0){
		unsigned char cur;
		fread(&cur, sizeof(unsigned char), 1, infp);
		for(int i=0; i<8; i++){
			int curNum = (cur & (unsigned char)128) >> 7;
			if(curNum == 0) curNode = curNode->left;
			else curNode = curNode->right;
			if(curNode->left == NULL && curNode->right == NULL){
				fwrite(&curNode->data, sizeof(char), 1, outfp);
				curNode = root;
			}
			cur = cur << 1;
			bitSize--;
			if(bitSize < 1) return;
		}
	}
}