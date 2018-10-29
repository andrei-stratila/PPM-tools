#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef struct color{
	unsigned char r, g, b;
} color;

typedef struct QuadtreeNode {
	unsigned char blue , green , red ;
	uint32_t area;
	int32_t top_left, top_right;
	int32_t bottom_left, bottom_right;
} __attribute__ ((packed)) QuadtreeNode ;

typedef struct TreeNode {
	int index;
	unsigned char blue , green , red ;
	uint32_t area;
	struct TreeNode *top_left, *top_right, *bottom_left, *bottom_right;
} __attribute__ ((packed)) TreeNode ;

void init_filename(char *file_in, char *file_out, int n, char *options[]);

int get_factor(char *options[], int index);

void readPixelMatrix(FILE *fin, int width, int heigth, color **mat);


TreeNode * createQuadTree_compression(TreeNode *root, color **mat, uint32_t size, int factor,
					uint32_t *n_node, int line_i, int line_f, int col_i, int col_f);

void print_Tree(TreeNode *root);


void free_memory(color **mat, int width, TreeNode *root, QuadtreeNode *compressed_image);

void print_decompressedImage(int width, int heigth, color **mat, char *file_out);


void create_MatrixRGB(TreeNode *root, color **mat, 
					  int line_i, int line_f, int col_i, int col_f);

TreeNode * createTreeNode(color color_med, uint32_t size, int nodes);

color mediumColor(color **mat, uint32_t size, 
				  int line_i, int line_f, int col_i, int col_f);

void free_QuadTree(TreeNode *root);

int divideImage(int factor, color **mat, color color_med, uint32_t size,
				int line_i, int line_f, int col_i, int col_f);