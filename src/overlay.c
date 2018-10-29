#include "main.h"

TreeNode * overlay_tree(TreeNode *root, TreeNode *root1, TreeNode *root2, uint32_t size){
	static uint32_t nodes = -1;
	nodes++;
	
	color color_med;
	color_med.r = ((root1->red + root2->red) / 2);
	color_med.g = ((root1->green + root2->green) / 2);
	color_med.b = ((root1->blue + root2->blue) / 2);
	if((root1->top_left == NULL && root2->top_left == NULL)){
		TreeNode *leaf = createTreeNode(color_med, size, nodes);
		return leaf;
	}
	else{
		root = createTreeNode(color_med, size, nodes);	
		
		if(root1->top_left == NULL)
			root->top_left = overlay_tree(root, root1, root2->top_left, size / 4);
		else if(root2->top_left == NULL)
			root->top_left = overlay_tree(root, root1->top_left, root2, size / 4);
		else 
			root->top_left = overlay_tree(root, root1->top_left, root2->top_left, size / 4);
		
		if(root1->top_right == NULL)
			root->top_right = overlay_tree(root, root1, root2->top_right, size / 4);
		else if(root2->top_right == NULL)
			root->top_right = overlay_tree(root, root1->top_right, root2, size / 4);
		else
			root->top_right = overlay_tree(root, root1->top_right, root2->top_right, size / 4);
		
		if(root1->bottom_right == NULL)
			root->bottom_right = overlay_tree(root, root1, root2->bottom_right, size / 4);
		else if(root2->bottom_right == NULL)
			root->bottom_right = overlay_tree(root, root1->bottom_right, root2, size / 4);
		else
			root->bottom_right = overlay_tree(root, root1->bottom_right, root2->bottom_right, size / 4);
		
		if(root1->bottom_left == NULL)
			root->bottom_left = overlay_tree(root, root1, root2->bottom_left, size / 4);
		else if(root2->bottom_left == NULL)
			root->bottom_left = overlay_tree(root, root1->bottom_left, root2, size / 4);
		else
			root->bottom_left = overlay_tree(root, root1->bottom_left, root2->bottom_left, size / 4);
	}
	return root;
}


void free_mat(int width, color **mat){
	int i;
	for(i = 0; i < width; i++)
		free(mat[i]);
	free(mat);
}

void free_Tree(TreeNode *root){

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


void overlay(int n, char *options[]){
	char file1_in[50] = "", file2_in[50] = "", file_out[50] = "";
	strcpy(file1_in, options[n - 3]);
	strcpy(file2_in, options[n - 2]);
	strcpy(file_out, options[n - 1]);
	int factor = get_factor(options, 2);
	unsigned char x[3] = "";
	int width = 0, heigth = 0, maxColor = 0, i;
	color **mat1 = NULL, **mat2 = NULL;
	
	FILE *fin = fopen(file1_in, "rb");
	fscanf(fin, "%s", x);
	fscanf(fin, "%d%d", &width, &heigth);
	fscanf(fin, "%d", &maxColor);
	fgetc(fin);
	mat1 = (color **)malloc(heigth * sizeof(color*));
	for(i = 0; i < heigth; i++)
		mat1[i] = (color *)malloc(width * sizeof(color));
	readPixelMatrix(fin, width, heigth, mat1);
	fclose(fin);

	fin = fopen(file2_in, "rb");
	fscanf(fin, "%s", x);
	fscanf(fin, "%d%d", &width, &heigth);
	fscanf(fin, "%d", &maxColor);
	fgetc(fin);
	mat2 = (color **)malloc(heigth * sizeof(color*));
	for(i = 0; i < heigth; i++)
		mat2[i] = (color *)malloc(width * sizeof(color));
	readPixelMatrix(fin, width, heigth, mat2);
	fclose(fin);

	TreeNode *root1 = NULL, *root2 = NULL;
	uint32_t n_node1 = 0, n_node2 = 0;
	root1 = createQuadTree_compression(root1, mat1, width*heigth, factor, &n_node1, 1, width, 1, heigth);
	root2 = createQuadTree_compression(root2, mat2, width*heigth, factor, &n_node2, 1, width, 1, heigth);
	n_node2 = n_node2 - n_node1;
	free_mat(width, mat1);
	free_mat(width, mat2);
	color **mat = NULL;
	mat = (color **)malloc(heigth * sizeof(color*));
	for(i = 0; i < heigth; i++)
		mat[i] = (color *)malloc(width * sizeof(color));
	TreeNode *root = NULL;
	root = overlay_tree(root, root1, root2, width*heigth);
	free_Tree(root2);
	free_Tree(root1);
	create_MatrixRGB(root, mat, 1, width, 1, width);
	free_Tree(root);
	print_decompressedImage(width, heigth, mat, file_out);
	free_mat(width, mat);
}