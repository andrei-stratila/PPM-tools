#include "mirror.h"
#include "main.h"


void swap_TreeNodeInfo(TreeNode **node1, TreeNode **node2){
	TreeNode *p = *node1, *q = *node2;
	uint32_t aux_area;
	unsigned char aux_color;
	TreeNode *aux = NULL;
	aux_area = p->area;
	p->area = q->area;
	q->area = aux_area;
	
	aux_color = p->red;
	p->red = q->red;
	q->red = aux_color;

	aux_color = p->green;
	p->green = q->green;
	q->green = aux_color;
	
	aux_color = p->blue;
	p->blue = q->blue;
	q->blue = aux_color;

	aux = p->top_left;
	p->top_left = q->top_left;
	q->top_left = aux;

	aux = p->top_right;
	p->top_right = q->top_right;
	q->top_right = aux;

	aux = p->bottom_left;
	p->bottom_left = q->bottom_left;
	q->bottom_left = aux;

	aux = p->bottom_right;
	p->bottom_right = q->bottom_right;
	q->bottom_right = aux;
	
	*node1 = p;
	*node2 = q;
}


TreeNode * mirror_tree(TreeNode *root, char type){
	if(root->top_left == NULL)
		return root;
	else{
		if(type == 'h'){
			swap_TreeNodeInfo(&root->top_left, &root->top_right);
			swap_TreeNodeInfo(&root->bottom_left, &root->bottom_right);
		}
		else if(type == 'v'){
			swap_TreeNodeInfo(&root->top_left, &root->bottom_left);
			swap_TreeNodeInfo(&root->top_right, &root->bottom_right);
		}
		root->top_left = mirror_tree(root->top_left, type);
		root->top_right = mirror_tree(root->top_right, type);
		root->bottom_right = mirror_tree(root->bottom_right, type);
		root->bottom_left = mirror_tree(root->bottom_left, type);
	}
	return root;
}


void mirror(int n, char *options[]){
	char file_in[50] = "", file_out[50] = "";
	init_filename(file_in, file_out, n, options);
	int factor = get_factor(options, 3);
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
	uint32_t n_node = 0;
	TreeNode *root = NULL;
	root = createQuadTree_compression(root, mat, width*heigth, factor, &n_node, 1, width, 1, heigth);	
	if(n_node == 0)
		n_node++;
	char type = options[2][0];
	root = mirror_tree(root, type);
	create_MatrixRGB(root, mat, 1, width, 1, width);
	print_decompressedImage(width, heigth, mat, file_out);
	QuadtreeNode *compressed_image = NULL;
	free_memory(mat, width, root, compressed_image);
}
