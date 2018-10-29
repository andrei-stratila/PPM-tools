#include "mirror.h"
#include "main.h"
#include "overlay.h"


void free_QuadTree(TreeNode *root){

	if(root == NULL){
		free(root);
		return;
	}
	free_QuadTree(root->top_left);
	free_QuadTree(root->top_right);
	free_QuadTree(root->bottom_right);
	free_QuadTree(root->bottom_left);	
	free(root);
}

void free_memory(color **mat, int width, TreeNode *root, QuadtreeNode *compressed_image){
	int i;
	for(i = 0; i < width; i++)
		free(mat[i]);
	free(mat);
	if(compressed_image != NULL)
		free(compressed_image);
	free_QuadTree(root);
}

void init_filename(char *file_in, char *file_out, int n, char *options[]){
	strcpy(file_in, options[n - 2]);
	strcpy(file_out, options[n - 1]);
}


int get_factor(char *options[], int index){
	int nr = 0, i;
	for(i = 0; i < strlen(options[index]); i++){
		nr = nr * 10 + (int)(options[index][i] - '0');
	}
	return nr;	
}


void readPixelMatrix(FILE *fin, int width, int heigth, color **mat){
	unsigned char r, g, b;
	int i, j;
	for(i = 0; i < width; i++){
		for(j = 0; j < heigth; j++){
			fread(&r, sizeof(unsigned char), 1, fin);
			fread(&g, sizeof(unsigned char), 1, fin);
			fread(&b, sizeof(unsigned char), 1, fin);
			mat[i][j].r = r;
			mat[i][j].g = g;
			mat[i][j].b = b;
		}
	}
}


color mediumColor(color **mat, uint32_t size, 
				  int line_i, int line_f, int col_i, int col_f){
	
	int i, j;
	long long int r_med = 0, g_med = 0, b_med = 0;
	color color_med;
	for(i = line_i; i < line_f; i++){
		for(j = col_i; j < col_f; j++){
			r_med = r_med + mat[i][j].r;
			g_med = g_med + mat[i][j].g;
			b_med = b_med + mat[i][j].b;
		}	
	}
	color_med.r = (unsigned char)(r_med / size);
	color_med.g = (unsigned char)(g_med / size);
	color_med.b = (unsigned char)(b_med / size);
	return color_med;
}


int divideImage(int factor, color **mat, color color_med, uint32_t size,
				int line_i, int line_f, int col_i, int col_f){
	long long int score = 0, r_aux = 0, g_aux = 0, b_aux = 0;
	int i, j;
	for(i = line_i; i < line_f; i++){
		for(j = col_i; j < col_f; j++){
			r_aux = (color_med.r - mat[i][j].r) * (color_med.r - mat[i][j].r);
			g_aux = (color_med.g - mat[i][j].g) * (color_med.g - mat[i][j].g);
			b_aux = (color_med.b - mat[i][j].b) * (color_med.b - mat[i][j].b);
			score = score + r_aux + g_aux + b_aux;
		}	
	}
	long long int s = size;
	s = s * 3;
	int result = score / s;
	if(result > factor)
		return -1;
	else
		return 1;
}


TreeNode * createTreeNode(color color_med, uint32_t size, int nodes){
	TreeNode *newNode = (TreeNode *)malloc(sizeof(struct TreeNode));
	newNode->blue = color_med.b;
	newNode->green = color_med.g;
	newNode->red = color_med.r;
	newNode->area = size;
	newNode->index = nodes;
	newNode->top_right = NULL;
	newNode->top_left = NULL;
	newNode->bottom_right = NULL;
	newNode->bottom_left = NULL;
	return newNode;
}


TreeNode * createQuadTree_compression(TreeNode *root, color **mat, uint32_t size, int factor,
					uint32_t *n_node, int line_i, int line_f, int col_i, int col_f){
	static uint32_t nodes = -1;
	nodes++;
	color color_med = mediumColor(mat, size, line_i - 1, line_f, col_i - 1, col_f);
	int divide = divideImage(factor, mat, color_med, size, 
							line_i - 1, line_f, col_i - 1, col_f);
	if(divide == 1 || size == 1){
		TreeNode *leaf = createTreeNode(color_med, size, nodes);
		return leaf;
	}
	else{
		root = createTreeNode(color_med, size, nodes);
		
		int coefL = (line_f-line_i)/ 2;
		int coefC = (col_f-col_i)/ 2;
		root->top_left = createQuadTree_compression(root, mat, size / 4, factor, n_node,
			line_i, line_i + coefL, col_i, col_i + coefC);
		root->top_right = createQuadTree_compression(root, mat, size / 4, factor, n_node,
			line_i, line_i + coefL, col_i + coefC + 1, col_f);
		root->bottom_right = createQuadTree_compression(root, mat, size / 4, factor, n_node,
			line_i + coefL + 1, line_f, col_i + coefC + 1, col_f);
		root->bottom_left = createQuadTree_compression(root, mat, size / 4, factor, n_node,
			line_i + coefL + 1, line_f, col_i, col_i + coefC);
	}
	*n_node = nodes + 1;
	return root;
}


QuadtreeNode TreeNode_info(TreeNode *node){
	QuadtreeNode info;
	info.area = node->area;
	info.red = node->red;
	info.green = node->green;
	info.blue = node->blue;
	if(node->top_left == NULL || node->top_right == NULL 
			|| node->bottom_right == NULL || node->bottom_left == NULL){
		info.top_left = -1;
		info.top_right = -1;
		info.bottom_right = -1;
		info.bottom_left = -1;
	}
	else{
		info.top_left = node->top_left->index;
		info.top_right = node->top_right->index;
		info.bottom_right = node->bottom_right->index;
		info.bottom_left = node->bottom_left->index;
	}
	return info;
}


void create_compressedImage(QuadtreeNode *compressed_image, TreeNode *root, 
							uint32_t *n_leaf){
	static uint32_t stat_n_node = 0, stat_n_leaf = 0;
	if(root == NULL)
		return;
	compressed_image[stat_n_node++] = TreeNode_info(root);
	if(root->top_left == NULL){
		stat_n_leaf++;
		*n_leaf = stat_n_leaf;
		return;
	}
	create_compressedImage(compressed_image, root->top_left, n_leaf);
	create_compressedImage(compressed_image, root->top_right, n_leaf);
	create_compressedImage(compressed_image, root->bottom_right, n_leaf);
	create_compressedImage(compressed_image, root->bottom_left, n_leaf);	
}


void print_compressedImage(uint32_t n_node, uint32_t n_leaf, 
							QuadtreeNode *compressed_image, char *file_out){
	FILE *fout = fopen(file_out, "wb");
	fwrite(&n_leaf, sizeof(uint32_t), 1, fout);
	fwrite(&n_node, sizeof(uint32_t), 1, fout);
	fwrite(compressed_image, sizeof(QuadtreeNode), n_node, fout);
	fclose(fout);
}


void compression(int n, char *options[]){
	char file_in[50] = "", file_out[50] = "";
	init_filename(file_in, file_out, n, options);
	int factor = get_factor(options, 2);
	unsigned char x[3] = "";
	int width = 0, heigth = 0, maxColor = 0, i;

	FILE *fin = fopen(file_in, "rb");
	fscanf(fin, "%s", x);
	fscanf(fin, "%d%d", &width, &heigth);
	fscanf(fin, "%d", &maxColor);
	fgetc(fin);
	color **mat;
	mat = (color **)malloc(heigth * sizeof(color*));
	for(i = 0; i < heigth; i++)
		mat[i] = (color *)malloc(width * sizeof(color));
	readPixelMatrix(fin, width, heigth, mat);
	fclose(fin);
	static uint32_t n_leaf = 0, n_node = 0;
	TreeNode *root = NULL;
	root = createQuadTree_compression(root, mat, width*heigth, factor, &n_node, 1, width, 1, heigth);	
	if(n_node == 0)
		n_node++;
	QuadtreeNode *compressed_image = NULL;
	compressed_image = (QuadtreeNode *)malloc((n_node + 1) * sizeof(struct QuadtreeNode));
	create_compressedImage(compressed_image, root, &n_leaf);
	print_compressedImage(n_node, n_leaf, compressed_image, file_out);
	free_memory(mat, width, root, compressed_image);
}


void print_decompressedImage(int width, int heigth, color **mat, char *file_out){
	FILE *fout = fopen(file_out, "wb");
	int i;
	fprintf(fout, "P6\n");
	fprintf(fout, "%d %d\n", width, heigth);
	fprintf(fout, "255\n");
	for(i = 0; i < width; i++){
		fwrite(mat[i], sizeof(color), width, fout);
	}
	fclose(fout);
}


void fill_MatrixRGB(color **mat, color med_color,
					int line_i, int line_f, int col_i, int col_f){
	int i, j;
	for(i = line_i; i < line_f; i++){
		for(j = col_i; j < col_f; j++){
			mat[i][j].r = med_color.r;
			mat[i][j].g = med_color.g;
			mat[i][j].b = med_color.b;
		}
	}
}


void create_MatrixRGB(TreeNode *root, color **mat, 
					  int line_i, int line_f, int col_i, int col_f){
	if(root->top_left == NULL){
		color med_color;
		med_color.r = root->red;
		med_color.g = root->green;
		med_color.b = root->blue;
		fill_MatrixRGB(mat, med_color, line_i - 1, line_f, col_i - 1, col_f);
	}
	else{		
		int coefL = (line_f-line_i)/ 2;
		int coefC = (col_f-col_i)/ 2;
		create_MatrixRGB(root->top_left, mat,
			line_i, line_i + coefL, col_i, col_i + coefC);
		create_MatrixRGB(root->top_right, mat,
			line_i, line_i + coefL, col_i + coefC + 1, col_f);
		create_MatrixRGB(root->bottom_right, mat,
			line_i + coefL + 1, line_f, col_i + coefC + 1, col_f);
		create_MatrixRGB(root->bottom_left, mat,
			line_i + coefL + 1, line_f, col_i, col_i + coefC);
	}
}


TreeNode * createQuadTree_decompression(TreeNode *root, QuadtreeNode *compressed_image,
	int index){
	color color_med;
	color_med.r = compressed_image[index].red;
	color_med.g = compressed_image[index].green;
	color_med.b = compressed_image[index].blue;
	uint32_t size = compressed_image[index].area;
	if(compressed_image[index].top_left == -1){
		TreeNode *leaf = createTreeNode(color_med, size, index);
		return leaf;
	}
	else{
		root = createTreeNode(color_med, size, index);
		
		root->top_left = createQuadTree_decompression(root, compressed_image,
			compressed_image[index].top_left);
		root->top_right = createQuadTree_decompression(root, compressed_image,
			compressed_image[index].top_right);
		root->bottom_right = createQuadTree_decompression(root, compressed_image,
			compressed_image[index].bottom_right);
		root->bottom_left = createQuadTree_decompression(root, compressed_image,
			compressed_image[index].bottom_left);
	}
	return root;
}

void decompression(int n, char *options[]){
	char file_in[50] = "", file_out[50] = "";
	init_filename(file_in, file_out, n, options);
	QuadtreeNode *compressed_image = NULL;
	uint32_t n_node = 0, n_leaf = 0;
	int width = 0, heigth = 0, i;
	FILE *fin = fopen(file_in, "rb");
	fread(&n_leaf, sizeof(uint32_t), 1, fin);
	fread(&n_node, sizeof(uint32_t), 1, fin);
	compressed_image = (QuadtreeNode *)malloc((n_node + 1) * sizeof(struct QuadtreeNode));
	fread(compressed_image, sizeof(QuadtreeNode), n_node, fin);
	fclose(fin);
	
	width = (int)sqrt(compressed_image[0].area);
	heigth = width;
	TreeNode *root = NULL;
	root = createQuadTree_decompression(root, compressed_image, 0);

	color **mat = NULL;
	mat = (color **)malloc(heigth * sizeof(color*));
	for(i = 0; i < heigth; i++)
		mat[i] = (color *)malloc(width * sizeof(color));
	create_MatrixRGB(root, mat, 1, width, 1, width);
	print_decompressedImage(width, heigth, mat, file_out);
	free_memory(mat, width, root, compressed_image);
}


int selected_option(char *options[]){ 
	if(!strcmp(options[1], "-c"))
		return 1;
	else if(!strcmp(options[1], "-d"))
		return 2;
	else if(!strcmp(options[1], "-m"))
		return 3;
	else if(!strcmp(options[1], "-o"))
		return 4;
	else
		return -1;
}

void image_command(int n, char *options[]){
	int command = selected_option(options);
	if(command == 1)
		compression(n, options);
	else if(command == 2)
		decompression(n, options);
	else if(command == 3)
		mirror(n, options);
	else if(command == 4)
		overlay(n, options);

}

int main(int argc, char *argv[]){
	image_command(argc, argv);
	return 0;	
}