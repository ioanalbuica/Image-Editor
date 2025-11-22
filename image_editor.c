#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct pixel {
	int r, g, b;
} pixel_t;

//aflam formatul unui fisier
int file_format(char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (!file) {
		return 0;
	}
	char c1, c2;
	fread(&c1, sizeof(char), 1, file);
	fread(&c2, sizeof(char), 1, file);
	fclose(file);
	if (c2 == '2')
		return 2;
	if (c2 == '3')
		return 3;
	if (c2 == '5')
		return 5;
	if (c2 == '6')
		return 6;
	return 0;
}

//deschidem acel fisier în functie de mod
FILE *open(char *filename)
{
	char mode[3];

	int type = file_format(filename);
	if (type == 0) {
		printf("Error opening file %s\n", filename);
		return NULL;
	}
	if (type == 5 || type == 6) {
		strcpy(mode, "rb");
	}
	if (type == 2 || type == 3) {
		strcpy(mode, "r");
	}

	FILE *file = fopen(filename, mode);
	if (!file) {
		printf("Error loading file %s\n", filename);
		return NULL;
	}
	printf("Loaded %s\n", filename);
	return file;
}

int **alloc_pgm(int width, int height)
{
	int **a;
	a = (int **)malloc(height * sizeof(int *));
	for (int i = 0; i < height; i++) {
		a[i] = (int *)malloc(width * sizeof(int));
	}
	return a;
}

pixel_t **alloc_ppm(int width, int height)
{
	pixel_t **a;
	a = (pixel_t **)malloc(height * sizeof(pixel_t *));
	for (int i = 0; i < height; i++) {
		a[i] = (pixel_t *)malloc(width * sizeof(pixel_t));
	}
	return a;
}

int **read_p2(FILE *file, char *filename, char *format, int *width,
			  int *height, int *intensity)
{
	int **a;

	file = open(filename);

	if (file) {
		fscanf(file, "%s", format);
		fscanf(file, "%d%d%d", width, height, intensity);
		a = alloc_pgm(*width, *height);
		for (int i = 0; i < *height; i++) {
			for (int j = 0; j < *width; j++) {
				fscanf(file, "%d", &a[i][j]);
			}
		}
	}
	return a;
}

pixel_t **read_p3(FILE *file, char *filename, char *format, int *width,
				  int *height, int *intensity)
{
	pixel_t **a;

	file = open(filename);

	if (file) {
		fscanf(file, "%s", format);
		fscanf(file, "%d%d%d", width, height, intensity);
		a = alloc_ppm(*width, *height);

		for (int i = 0; i < *height; i++) {
			for (int j = 0; j < *width; j++) {
				fscanf(file, "%d%d%d", &a[i][j].r, &a[i][j].g, &a[i][j].b);
			}
		}
	}
	return a;
}

int **read_p5(FILE *file, char *filename, char *format, int *width,
			  int *height, int *intensity)
{
	int **a;
	file = open(filename);

	if (file) {
		fscanf(file, "%s", format);
		fscanf(file, "%d%d%d", width, height, intensity);
		fgetc(file);
		a = alloc_pgm(*width, *height);
		for (int i = 0; i < *height; i++) {
			for (int j = 0; j < *width; j++) {
				unsigned char x;
				fread(&x, sizeof(unsigned char), 1, file);
				a[i][j] = (int)x;
			}
		}
	}
	return a;
}

pixel_t **read_p6(FILE *file, char *filename, char *format, int *width,
				  int *height, int *intensity)
{
	pixel_t **a;
	file = open(filename);

	if (file) {
		fscanf(file, "%s", format);
		fscanf(file, "%d%d%d", width, height, intensity);
		fgetc(file);
		a = alloc_ppm(*width, *height);
		for (int i = 0; i < *height; i++) {
			for (int j = 0; j < *width; j++) {
				unsigned char x, y, z;
				fread(&x, sizeof(unsigned char), 1, file);
				fread(&y, sizeof(unsigned char), 1, file);
				fread(&z, sizeof(unsigned char), 1, file);
				a[i][j].r = (int)x;
				a[i][j].g = (int)y;
				a[i][j].b = (int)z;
			}
		}
	}
	return a;
}

//se creează matricea selectata
int **select_pgm(int **matrix_pgm, int x1, int y1, int x2, int y2)
{
	int **selected_pgm = alloc_pgm(x2 - x1, y2 - y1);

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			selected_pgm[i - y1][j - x1] = matrix_pgm[i][j];
		}
	}
	return selected_pgm;
}

pixel_t **select_ppm(pixel_t **matrix_ppm, int x1, int y1, int x2, int y2)
{
	pixel_t **selected_ppm = alloc_ppm(x2 - x1, y2 - y1);

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			selected_ppm[i - y1][j - x1] = matrix_ppm[i][j];
		}
	}
	return selected_ppm;
}

// in functie de format si parametru se afiseaza matricea in fisierul dorit
void save_p2(char *filename, char *format, int width, int height,
			 int intensity, int **matrix_pgm)
{
	FILE *file = fopen(filename, "w");
	printf("Saved %s\n", filename);
	fprintf(file, "%s\n", format);
	fprintf(file, "%d %d\n%d\n", width, height, intensity);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fprintf(file, "%d ", matrix_pgm[i][j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

void save_p5(char *filename, char *format, int width, int height,
			 int intensity, int **matrix_pgm)
{
	FILE *file = fopen(filename, "wb");
	printf("Saved %s\n", filename);
	fprintf(file, "%s\n", format);
	fprintf(file, "%d %d\n%d\n", width, height, intensity);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			unsigned char x;
			x = (unsigned char)matrix_pgm[i][j];
			fwrite(&x, sizeof(unsigned char), 1, file);
		}
	}
	fclose(file);
}

void save_p3(char *filename, char *format, int width, int height,
			 int intensity, pixel_t **matrix_ppm)
{
	FILE *file = fopen(filename, "w");
	printf("Saved %s\n", filename);
	fprintf(file, "%s\n", format);
	fprintf(file, "%d %d\n%d\n", width, height, intensity);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fprintf(file, "%d %d %d ", matrix_ppm[i][j].r,
					matrix_ppm[i][j].g, matrix_ppm[i][j].b);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

void save_p6(char *filename, char *format, int width, int height,
			 int intensity, pixel_t **matrix_ppm)
{
	FILE *file = fopen(filename, "wb");
	printf("Saved %s\n", filename);
	fprintf(file, "%s\n", format);
	fprintf(file, "%d %d\n%d\n", width, height, intensity);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			unsigned char x, y, z;
			x = matrix_ppm[i][j].r;
			y = matrix_ppm[i][j].g;
			z = matrix_ppm[i][j].b;
			fwrite(&x, sizeof(unsigned char), 1, file);
			fwrite(&y, sizeof(unsigned char), 1, file);
			fwrite(&z, sizeof(unsigned char), 1, file);
		}
	}
	fclose(file);
}

int **free_pgm(int height, int **matrix_pgm)
{
	for (int i = 0; i < height; i++) {
		free(matrix_pgm[i]);
	}
	free(matrix_pgm);
	matrix_pgm = NULL;
	return matrix_pgm;
}

pixel_t **free_ppm(int height, pixel_t **matrix_ppm)
{
	for (int i = 0; i < height; i++) {
		free(matrix_ppm[i]);
	}
	free(matrix_ppm);
	matrix_ppm = NULL;
	return matrix_ppm;
}

// folosit pentru deep-copy al unor matrici
int **swap_p2(int width, int height, int **a, int **b)
{
	a = alloc_pgm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			a[i][j] = b[i][j];
		}
	}
	return a;
}

pixel_t **swap_p3(int width, int height, pixel_t **a, pixel_t **b)
{
	a = alloc_ppm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			a[i][j] = b[i][j];
		}
	}
	return a;
}

// converteste un sir de cifre in numar
int convert_string_to_int(char *s)
{
	int x = 0;
	for (int i = 0; i < (int)strlen(s); i++) {
		if (s[i] != '-' && (s[i] < '0' || s[i] > '9')) {
			return -1;
		}
		if (s[i] != '-')
			x = x * 10 + (s[i] - 48);
	}
	return x;
}

// se returneaza matricea rotita
int **rotate_pgm_90(int width, int height, int **matrix_pgm)
{
	int **rotated_pgm = alloc_pgm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_pgm[i][j] = matrix_pgm[width - j - 1][i];
		}
	}
	return rotated_pgm;
}

int **rotate_pgm_180(int width, int height, int **matrix_pgm)
{
	int **rotated_pgm = alloc_pgm(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_pgm[i][j] = matrix_pgm[height - i - 1][width - j - 1];
		}
	}
	return rotated_pgm;
}

int **rotate_pgm_270(int width, int height, int **matrix_pgm)
{
	int **rotated_pgm = alloc_pgm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_pgm[i][j] = matrix_pgm[j][height - i - 1];
		}
	}
	return rotated_pgm;
}

int **rotate_pgm_360(int width, int height, int **matrix_pgm)
{
	int **rotated_pgm = alloc_pgm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_pgm[i][j] = matrix_pgm[i][j];
		}
	}
	return rotated_pgm;
}

// in functie de unghi se calculeaza matricea rotita, iar inverse retine dacă
// unghiul a fost de 90 sau 270 de grade
int **rotate_pgm(int width, int height, int **matrix_pgm, int *inverse, int u)
{
	int **rotated_pgm;
	if (u < 0) {
		u = 360 + u;
	}
	if (u == 0) {
		u = 360;
	}
	if (u == 90) {
		rotated_pgm = rotate_pgm_90(height, width, matrix_pgm);
	}
	if (u == 180) {
		rotated_pgm = rotate_pgm_180(width, height, matrix_pgm);
	}
	if (u == 270) {
		rotated_pgm = rotate_pgm_270(height, width, matrix_pgm);
	}
	if (u == 360) {
		rotated_pgm = rotate_pgm_360(width, height, matrix_pgm);
	}
	if (u == 90 || u == 270) {
		*inverse = 1;
	} else {
		*inverse = 0;
	}
	return rotated_pgm;
}

pixel_t **rotate_ppm_90(int width, int height, pixel_t **matrix_ppm)
{
	pixel_t **rotated_ppm = alloc_ppm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_ppm[i][j] = matrix_ppm[width - j - 1][i];
		}
	}
	return rotated_ppm;
}

pixel_t **rotate_ppm_180(int width, int height, pixel_t **matrix_ppm)
{
	pixel_t **rotated_ppm = alloc_ppm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_ppm[i][j] = matrix_ppm[height - i - 1][width - j - 1];
		}
	}
	return rotated_ppm;
}

pixel_t **rotate_ppm_270(int width, int height, pixel_t **matrix_ppm)
{
	pixel_t **rotated_ppm = alloc_ppm(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_ppm[i][j] = matrix_ppm[j][height - i - 1];
		}
	}
	return rotated_ppm;
}

pixel_t **rotate_ppm_360(int width, int height, pixel_t **matrix_ppm)
{
	pixel_t **rotated_ppm = alloc_ppm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rotated_ppm[i][j] = matrix_ppm[i][j];
		}
	}
	return rotated_ppm;
}

pixel_t **rotate_ppm(int width, int height, pixel_t **matrix_ppm,
					 int *inverse, int u)
{
	pixel_t **rotated_ppm;
	if (u < 0) {
		u = 360 + u;
	}
	if (u == 0) {
		u = 360;
	}
	if (u == 90) {
		rotated_ppm = rotate_ppm_90(height, width, matrix_ppm);
	}
	if (u == 180) {
		rotated_ppm = rotate_ppm_180(width, height, matrix_ppm);
	}
	if (u == 270) {
		rotated_ppm = rotate_ppm_270(height, width, matrix_ppm);
	}
	if (u == 360) {
		rotated_ppm = rotate_ppm_360(width, height, matrix_ppm);
	}
	if (u == 90 || u == 270) {
		*inverse = 1;
	} else {
		*inverse = 0;
	}
	return rotated_ppm;
}

// se insereaza in matricea a elementele din matricea b pornind de la
// coordonatele x1, y1, x2, y2
int **insert_pgm(int width, int height, int x1, int y1, int x2, int y2,
				 int **a, int **b)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i >= y1 && j >= x1 && i < y2 && j < x2) {
				a[i][j] = b[i - y1][j - x1];
			}
		}
	}
	return a;
}

pixel_t **insert_ppm(int width, int height, int x1, int y1, int x2, int y2,
					 pixel_t **a, pixel_t **b)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i >= y1 && j >= x1 && i < y2 && j < x2) {
				a[i][j] = b[i - y1][j - x1];
			}
		}
	}
	return a;
}

// se verifica daca este putere al lui 2
int is_pow2(int y)
{
	int nr = 0;

	if (y == 0)
		return 0;
	while (y > 0) {
		int u = y & 1;
		if (u == 1) {
			nr++;
		}
		if (nr > 1) {
			return 0;
		}
		y = y >> 1;
	}
	return 1;
}

// calculam frecventa tuturor val dintr-o matrice
int *frec(int width, int height, int **matrix_pgm)
{
	int *v = (int *)calloc(256, sizeof(int));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			v[matrix_pgm[i][j]]++;
		}
	}
	return v;
}

// calculam noua valoare f  pentru fiecare 256 de valori posibile
// din matricea equalized
double *f_array(int width, int height, int **a)
{
	int sum_frec = 0;
	int *v = frec(width, height, a);
	double *f = (double *)calloc(256, sizeof(double));

	for (int i = 0; i < 256; i++) {
		sum_frec += v[i];
		f[i] = 255.0 * sum_frec / (width * height);
	}
	free(v);
	return f;
}

// se afiseaza histograma, folosindu-ne de suma frecventelor pe fiecare bin
void print_histogram(int y, int *sum)
{
	for (int i = 0; i < y; i++) {
		printf("%d\t|\t", sum[i]);
		for (int j = 0; j < sum[i]; j++) {
			printf("*");
		}
		printf("\n");
	}
}

// z reprezintă cate valori din 0-256 se află intr-un bin
void calculate_histogram(int width, int height, int **matrix_pgm, int x, int y)
{
	int maxx = 0, z = 256 / y, sum[256] = {0};

	int *v = frec(width, height, matrix_pgm);

	for (int i = 0; i < y; i++) {
		for (int j = i * z; j < (i + 1) * z; j++) {
			sum[i] += v[j];
			if (sum[i] > maxx) {
				maxx = sum[i];
			}
		}
	}
	for (int i = 0; i < y; i++) {
		sum[i] = (sum[i] * x) / maxx;
	}
	print_histogram(y, sum);
	free(v);
}

int clamp(int x)
{
	if (x < 0) {
		return 0;
	}
	if (x > 255) {
		return 255;
	}
	return x;
}

// pentru fiecare element din matrice se extrage f -ul valorii sale
int **equalized_matrix(int width, int height, int **a)
{
	double *f = f_array(width, height, a);

	int **b = alloc_pgm(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			b[i][j] = clamp(round(f[a[i][j]]));
		}
	}
	free(f);
	return b;
}

// se calculeaza noua matrice dupa fiecare efect
pixel_t **apply_sharpen(int width, int height, pixel_t **a)
{
	pixel_t **b = alloc_ppm(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if ((i != 0 && i != height - 1) && (j != 0 && j != width - 1)) {
				b[i][j].r = 5 * a[i][j].r - a[i - 1][j].r - a[i][j - 1].r
						   - a[i][j + 1].r - a[i + 1][j].r;
				b[i][j].g = 5 * a[i][j].g - a[i - 1][j].g - a[i][j - 1].g
						   - a[i][j + 1].g - a[i + 1][j].g;
				b[i][j].b = 5 * a[i][j].b - a[i - 1][j].b - a[i][j - 1].b
						   - a[i][j + 1].b - a[i + 1][j].b;
				b[i][j].r = clamp(b[i][j].r);
				b[i][j].g = clamp(b[i][j].g);
				b[i][j].b = clamp(b[i][j].b);
			} else {
				b[i][j] = a[i][j];
			}
		}
	}
	return b;
}

pixel_t **apply_edge(int width, int height, pixel_t **a)
{
	pixel_t **b = alloc_ppm(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i != 0 && i != height - 1 && j != 0 && j != width - 1) {
				b[i][j].r = 8 * a[i][j].r - a[i - 1][j].r - a[i][j - 1].r
						   - a[i][j + 1].r - a[i + 1][j].r - a[i - 1][j - 1].r
						   - a[i - 1][j + 1].r - a[i + 1][j - 1].r
						   - a[i + 1][j + 1].r;

				b[i][j].g = 8 * a[i][j].g - a[i - 1][j].g - a[i][j - 1].g
						   - a[i][j + 1].g - a[i + 1][j].g - a[i - 1][j - 1].g
						   - a[i - 1][j + 1].g - a[i + 1][j - 1].g
						   - a[i + 1][j + 1].g;

				b[i][j].b = 8 * a[i][j].b - a[i - 1][j].b - a[i][j - 1].b
						   - a[i][j + 1].b - a[i + 1][j].b - a[i - 1][j - 1].b
						   - a[i - 1][j + 1].b - a[i + 1][j - 1].b
						   - a[i + 1][j + 1].b;

				b[i][j].r = clamp(b[i][j].r);
				b[i][j].g = clamp(b[i][j].g);
				b[i][j].b = clamp(b[i][j].b);
			} else {
				b[i][j] = a[i][j];
			}
		}
	}
	return b;
}

pixel_t **apply_blur(int width, int height, pixel_t **a)
{
	pixel_t **b = alloc_ppm(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i != 0 && i != height - 1 && j != 0 && j != width - 1) {
				b[i][j].r = a[i][j].r + a[i - 1][j].r + a[i][j - 1].r
						   + a[i][j + 1].r + a[i + 1][j].r + a[i - 1][j - 1].r
						   + a[i - 1][j + 1].r + a[i + 1][j - 1].r
						   + a[i + 1][j + 1].r;

				b[i][j].g = a[i][j].g + a[i - 1][j].g + a[i][j - 1].g
						   + a[i][j + 1].g + a[i + 1][j].g + a[i - 1][j - 1].g
						   + a[i - 1][j + 1].g + a[i + 1][j - 1].g
						   + a[i + 1][j + 1].g;

				b[i][j].b = a[i][j].b + a[i - 1][j].b + a[i][j - 1].b
						   + a[i][j + 1].b + a[i + 1][j].b + a[i - 1][j - 1].b
						   + a[i - 1][j + 1].b + a[i + 1][j - 1].b
						   + a[i + 1][j + 1].b;

				b[i][j].r = clamp(b[i][j].r * 0.11111);
				b[i][j].g = clamp(b[i][j].g * 0.11111);
				b[i][j].b = clamp(b[i][j].b * 0.11111);
			} else {
				b[i][j] = a[i][j];
			}
		}
	}
	return b;
}

pixel_t **apply_gaussian(int width, int height, pixel_t **a)
{
	pixel_t **b = alloc_ppm(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i != 0 && i != height - 1 && j != 0 && j != width - 1) {
				b[i][j].r = 4 * a[i][j].r + 2 * a[i - 1][j].r
						   + 2 * a[i][j - 1].r + 2 * a[i][j + 1].r
						   + 2 * a[i + 1][j].r + a[i - 1][j - 1].r
						   + a[i - 1][j + 1].r + a[i + 1][j - 1].r
						   + a[i + 1][j + 1].r;

				b[i][j].g = 4 * a[i][j].g + 2 * a[i - 1][j].g
						   + 2 * a[i][j - 1].g + 2 * a[i][j + 1].g
						   + 2 * a[i + 1][j].g + a[i - 1][j - 1].g
						   + a[i - 1][j + 1].g + a[i + 1][j - 1].g
						   + a[i + 1][j + 1].g;

				b[i][j].b = 4 * a[i][j].b + 2 * a[i - 1][j].b
						   + 2 * a[i][j - 1].b + 2 * a[i][j + 1].b
						   + 2 * a[i + 1][j].b + a[i - 1][j - 1].b
						   + a[i - 1][j + 1].b + a[i + 1][j - 1].b
						   + a[i + 1][j + 1].b;

				b[i][j].r = clamp(b[i][j].r * 0.0625);
				b[i][j].g = clamp(b[i][j].g * 0.0625);
				b[i][j].b = clamp(b[i][j].b * 0.0625);
			} else {
				b[i][j] = a[i][j];
			}
		}
	}
	return b;
}

// se citeste parametrul si se aplica efectul corespunzator
pixel_t **decide_apply(char *parameter, int width, int height,
					   pixel_t **matrix_ppm)
{
	pixel_t **b = NULL;

	if (strcmp(parameter, "SHARPEN") == 0) {
		b = apply_sharpen(width, height, matrix_ppm);
		printf("APPLY SHARPEN done\n");
	} else if (strcmp(parameter, "EDGE") == 0) {
		b = apply_edge(width, height, matrix_ppm);
		printf("APPLY EDGE done\n");
	} else if (strcmp(parameter, "BLUR") == 0) {
		b = apply_blur(width, height, matrix_ppm);
		printf("APPLY BLUR done\n");
	} else if (strcmp(parameter, "GAUSSIAN_BLUR") == 0) {
		b = apply_gaussian(width, height, matrix_ppm);
		printf("APPLY GAUSSIAN_BLUR done\n");
	} else {
		printf("APPLY parameter invalid\n");
		return NULL;
	}
	return b;
}

// in functie de coordonatele x1, x2, y1, y2 si de parametru se aplică efectul
// corespunzator
pixel_t **f0(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 != 0 && x2 != width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1 - 1, x2 + 1, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 2, y2 - y1 + 2,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1 - 1, x2 + 1, y2 + 1,
								matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 2, applied);
		b = free_ppm(y2 - y1 + 2, b);
	}
	return matrix_ppm;
}

pixel_t **f1(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 != 0 && x2 != width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1 - 1, x2 + 1, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 2, y2 - y1 + 1,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1 - 1, x2 + 1, y2,
								matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f2(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 != 0 && x2 == width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1 - 1, x2, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1 + 2,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1 - 1, x2, y2 + 1,
								matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 2, applied);
		b = free_ppm(y2 - y1 + 2, b);
	}
	return matrix_ppm;
}

pixel_t **f3(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 != 0 && x2 == width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1 - 1, x2, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1 + 1,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1 - 1, x2, y2,
								matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f4(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 == 0 && x2 != width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1, x2 + 1, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 2, y2 - y1 + 1,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1, x2 + 1,
								y2 + 1, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f5(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 == 0 && x2 != width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1, x2 + 1, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 2, y2 - y1, applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1, x2 + 1,
								y2, matrix_ppm, b);
		applied = free_ppm(y2 - y1, applied);
		b = free_ppm(y2 - y1, b);
	}
	return matrix_ppm;
}

pixel_t **f6(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 == 0 && x2 == width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1, x2, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1 + 1,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1, x2,
								y2 + 1, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f7(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 != 0 && y1 == 0 && x2 == width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1 - 1, y1, x2, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1, applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1 - 1, y1, x2,
								y2, matrix_ppm, b);
		applied = free_ppm(y2 - y1, applied);
		b = free_ppm(y2 - y1, b);
	}
	return matrix_ppm;
}

pixel_t **f8(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 != 0 && x2 != width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1, y1 - 1, x2 + 1, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1 + 2,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1 + 1, x2 + 1,
								y2 + 1, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 2, applied);
		b = free_ppm(y2 - y1 + 2, b);
	}
	return matrix_ppm;
}

pixel_t **f9(char *parameter, int width, int height, int x1, int x2,
			 int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 != 0 && x2 != width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1, y1 - 1, x2 + 1, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1 + 1,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1 + 1, x2 + 1,
								y2, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f10(char *parameter, int width, int height, int x1, int x2,
			  int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 != 0 && x2 == width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1, y1 - 1, x2, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1, y2 - y1 + 2, applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1 + 1, x2,
								y2 + 1, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 2, applied);
		b = free_ppm(y2 - y1 + 2, b);
	}
	return matrix_ppm;
}

pixel_t **f11(char *parameter, int width, int height, int x1, int x2,
			  int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 != 0 && x2 == width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1, y1 - 1, x2, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1, y2 - y1 + 1, applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1 - 1, x2,
								y2, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f12(char *parameter, int width, int height, int x1, int x2,
			  int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 == 0 && x2 != width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1, y1, x2 + 1, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1 + 1,
								   applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1, x2 + 1,
								y2 + 1, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f13(char *parameter, int width, int height, int x1, int x2,
			  int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 == 0 && x2 != width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1, y1, x2 + 1, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1 + 1, y2 - y1, applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1, x2 + 1,
								y2, matrix_ppm, b);
		applied = free_ppm(y2 - y1, applied);
		b = free_ppm(y2 - y1, b);
	}
	return matrix_ppm;
}

pixel_t **f14(char *parameter, int width, int height, int x1, int x2,
			  int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 == 0 && x2 == width && y2 != height) {
		applied = select_ppm(matrix_ppm, x1, y1, x2, y2 + 1);
		pixel_t **b = decide_apply(parameter, x2 - x1, y2 - y1 + 1, applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1, x2,
								y2 + 1, matrix_ppm, b);
		applied = free_ppm(y2 - y1 + 1, applied);
		b = free_ppm(y2 - y1 + 1, b);
	}
	return matrix_ppm;
}

pixel_t **f15(char *parameter, int width, int height, int x1, int x2,
			  int y1, int y2, pixel_t **matrix_ppm)
{
	pixel_t **applied;
	if (x1 == 0 && y1 == 0 && x2 == width && y2 == height) {
		applied = select_ppm(matrix_ppm, x1, y1, x2, y2);
		pixel_t **b = decide_apply(parameter, x2 - x1, y2 - y1, applied);
		if (!b) {
			return matrix_ppm;
		}
		matrix_ppm = insert_ppm(width, height, x1, y1, x2,
								y2, matrix_ppm, b);
		applied = free_ppm(y2 - y1, applied);
		b = free_ppm(y2 - y1, b);
	}
	return matrix_ppm;
}

// se va selecta toata matricea
void select_all(char *format, int width, int height, int **matrix_pgm,
				pixel_t **matrix_ppm, int ***selected_pgm,
				pixel_t ***selected_ppm, int *x1, int *y1, int *x2, int *y2,
				int *ok, int *all)
{
	*ok = 1;
	*all = 1;
	if (strcmp(format, "P2") == 0 || strcmp(format, "P5") == 0) {
		*selected_pgm = select_pgm(matrix_pgm, 0, 0, width, height);
	}
	if (strcmp(format, "P3") == 0 || strcmp(format, "P6") == 0) {
		*selected_ppm = select_ppm(matrix_ppm, 0, 0, width, height);
	}
	*x1 = *y1 = 0;
	*x2 = width;
	*y2 = height;
	printf("Selected ALL\n");
}

// se vor citi coordonatele x1, y1, x2, y2 si va testa daca sunt valide
int ok_coordinates(char *arg, int width, int height,
				   int *x1, int *y1, int *x2, int *y2, int *ok, int *all)
{
	int a = convert_string_to_int(arg);
	if (a == -1) {
		printf("Invalid command\n");
		return 0;
	}

	char *arg2 = strtok(NULL, " ");
	if (!arg2) {
		printf("Invalid command\n");
		return 0;
	}
	int b = convert_string_to_int(arg2);
	if (b == -1) {
		printf("Invalid command\n");
		return 0;
	}

	char *arg3 = strtok(NULL, " ");
	if (!arg3) {
		printf("Invalid command\n");
		return 0;
	}
	int c = convert_string_to_int(arg3);
	if (c == -1) {
		printf("Invalid command\n");
		return 0;
	}

	char *arg4 = strtok(NULL, " ");
	if (!arg4) {
		printf("Invalid command\n");
		return 0;
	}
	int d = convert_string_to_int(arg4);
	if (d == -1) {
		printf("Invalid command\n");
		return 0;
	}

	if (arg[0] == '-' || arg2[0] == '-' || arg3[0] == '-' || arg4[0] == '-') {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	if (a > c) {
		int aux = c;
		c = a;
		a = aux;
	}
	if (b > d) {
		int aux = d;
		d = b;
		b = aux;
	}
	if (a < 0 || b < 0 || c > width || d > height) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	if (a == c || b == d) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	if (c < 0 || d < 0) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	*all = 0;
	*ok = 1;
	*x1 = a; *y1 = b; *x2 = c; *y2 = d;
	return 1;
}

// daca coordonatele sunt valide, se va selecta regiunea corespunzatoare
void select_region(char *format, int width, int height, int **matrix_pgm,
				   pixel_t **matrix_ppm, int ***selected_pgm,
				   pixel_t ***selected_ppm, int *x1, int *y1, int *x2, int *y2,
				   int *ok, int *all, char *arg)
{
	if (ok_coordinates(arg, width, height, x1, y1, x2, y2, ok, all)) {
		printf("Selected %d %d %d %d\n", *x1, *y1, *x2, *y2);
		if (strcmp(format, "P2") == 0 || strcmp(format, "P5") == 0) {
			*selected_pgm = select_pgm(matrix_pgm, *x1, *y1, *x2, *y2);
		}
		if (strcmp(format, "P3") == 0  || strcmp(format, "P6") == 0) {
			*selected_ppm = select_ppm(matrix_ppm, *x1, *y1, *x2, *y2);
		}
		if (*x1 == 0 && *y1 == 0 && *x2 == width && *y2 == height) {
			*all = 1;
		}
	} else {
		return;
	}
}

// se va inchide programul
void exit_command(int height, int ***matrix_pgm, pixel_t ***matrix_ppm,
				  int ok_load)
{
	if (!ok_load) {
		printf("No image loaded\n");
	}
	if (*matrix_pgm) {
		*matrix_pgm = free_pgm(height, *matrix_pgm);
	}
	if (*matrix_ppm) {
		*matrix_ppm = free_ppm(height, *matrix_ppm);
	}
}

// se va da load la fisier si vor citi imaginiile si datele lor
// se vor initializa x1, y1, x2 si y2 pentru a fi folositi fara select inainte
void load(FILE *file, char *filename, char *format, int *width, int *height,
		  int *intensity, int ***matrix_pgm, pixel_t ***matrix_ppm,
		  int *ok_load, int *x1, int *y1, int *x2, int *y2)
{
	char *s = strtok(NULL, " ");
	strcpy(filename, s);
	if (!filename) {
		printf("Invalid command\n");
		return;
	}
	if (ok_load && file) {
		fclose(file);
		file = NULL;
	}
	*ok_load = 1;
	if (file_format(filename) == 0) {
		printf("Failed to load %s\n", filename);
		*ok_load = 0;
		*matrix_pgm = NULL;
		*matrix_ppm = NULL;
	}
	if (*matrix_pgm) {
		*matrix_pgm = free_pgm(*height, *matrix_pgm);
	}
	if (*matrix_ppm) {
		*matrix_ppm = free_ppm(*height, *matrix_ppm);
	}
	if (file_format(filename) == 2) {
		*matrix_pgm = read_p2(file, filename, format, width, height, intensity);
	}
	if (file_format(filename) == 3) {
		*matrix_ppm = read_p3(file, filename, format, width, height, intensity);
	}
	if (file_format(filename) == 5) {
		*matrix_pgm = read_p5(file, filename, format, width, height, intensity);
	}
	if (file_format(filename) == 6) {
		*matrix_ppm = read_p6(file, filename, format, width, height, intensity);
	}
	*x1 = *y1 = 0;
	*x2 = *width;
	*y2 = *height;
}

// se va selecta in functie de parametrul citit
void select_command(char *format, int width, int height, int **matrix_pgm,
					pixel_t **matrix_ppm, int ***selected_pgm,
					pixel_t ***selected_ppm, int *x1, int *y1, int *x2,
					int *y2, int ok_load, int *ok, int *all)
{
	if (!ok_load) {
		printf("No image loaded\n");
		return;
	}
	char *arg = strtok(NULL, " ");
	if (!arg) {
		printf("Invalid command\n");
		return;
	}
	if (strcmp(arg, "ALL") == 0) {
		select_all(format, width, height, matrix_pgm, matrix_ppm,
				   selected_pgm, selected_ppm, x1, y1, x2, y2, ok, all);
	} else {
		select_region(format, width, height, matrix_pgm, matrix_ppm,
					  selected_pgm, selected_ppm, x1, y1, x2, y2, ok,
					  all, arg);
	}
}

// se salveaza imaginea in fisierul dat, in functie de argument
void save(char *filename, char *format, int width, int height,
		  int intensity, int **matrix_pgm, pixel_t **matrix_ppm, int ok_load)
{
	char *out = strtok(NULL, " ");
	char *arg = strtok(NULL, " ");
	if (!out) {
		printf("Invalid command\n");
		return;
	}
	if (!ok_load) {
		printf("No image loaded\n");
		return;
	}
	if (!arg) {
		if (file_format(filename) == 2 || file_format(filename) == 5) {
			strcpy(format, "P5");
			save_p5(out, format, width, height, intensity, matrix_pgm);
		}
		if (file_format(filename) == 3 || file_format(filename) == 6) {
			strcpy(format, "P6");
			save_p6(out, format, width, height, intensity, matrix_ppm);
		}
	} else if (strcmp(arg, "ascii") == 0) {
		if (file_format(filename) == 2 || file_format(filename) == 5) {
			strcpy(format, "P2");
			save_p2(out, format, width, height, intensity, matrix_pgm);
		}
		if (file_format(filename) == 3 || file_format(filename) == 6) {
			strcpy(format, "P3");
			save_p3(out, format, width, height, intensity, matrix_ppm);
		}
	}
}

// se elibereaza matricea apoi ea devine selectia facuta anterior
void crop(char *filename, int *width, int *height, int ***matrix_pgm,
		  pixel_t ***matrix_ppm, int x1, int y1, int x2, int y2,
		  int **selected_pgm, pixel_t **selected_ppm, int ok_load,
		  int *ok, int all)
{
	if (ok_load)
		printf("Image cropped\n");
	else {
		printf("No image loaded\n");
	}
	if (*ok == 1) {
		if (file_format(filename) == 2 || file_format(filename) == 5) {
			*matrix_pgm = free_pgm(*height, *matrix_pgm);
		}
		if (file_format(filename) == 3 || file_format(filename) == 6) {
			*matrix_ppm = free_ppm(*height, *matrix_ppm);
		}
		if (!all) {
			*width = x2 - x1;
			*height = y2 - y1;
		}
		*ok = 0;
		if (file_format(filename) == 2 || file_format(filename) == 5) {
			*matrix_pgm = selected_pgm;
		}
		if (file_format(filename) == 3 || file_format(filename) == 6) {
			*matrix_ppm = selected_ppm;
		}
	}
}

// se citeste unghiul, iar apoi daca nu a fost selectat nimic sau a fost
// totul selectat, se va tine cont de unghi pt a schimba dimesniunile
void rotate(char *filename, int *width, int *height,
			int ***matrix_pgm, pixel_t ***matrix_ppm, int x1, int y1, int x2,
			int y2, int ***selection_pgm, pixel_t ***selection_ppm,
			int ok_load, int ok, int all)
{
	if (!ok_load) {
		printf("No image loaded\n");
		return;
	}
	int ng = 0, inverse = 0;
	char *s = strtok(NULL, " ");
	if (s[0] == '-') {
		ng = 1;
	}
	int u = convert_string_to_int(s);
	if (u % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (ng) {
		printf("Rotated -%d\n", u);
		u = 360 - u;
	} else {
		printf("Rotated %d\n", u);
	}
	if (file_format(filename) == 2 || file_format(filename) == 5) {
		if (ok == 0 || all == 1) {
			int **rotated_p2 = rotate_pgm(*width, *height, *matrix_pgm,
										  &inverse, u);
			*matrix_pgm = free_pgm(*height, *matrix_pgm);
			*matrix_pgm = rotated_p2;
			if (inverse == 1) {
				int temp1 = *height;
				*height = *width;
				*width = temp1;
			}
		} else {
			int **rotated_p2 = rotate_pgm(x2 - x1, y2 - y1, *selection_pgm,
										  &inverse, u);
			*selection_pgm = free_pgm(y2 - y1, *selection_pgm);
			*selection_pgm = rotated_p2;
			*matrix_pgm = insert_pgm(*width, *height, x1, y1, x2, y2,
									 *matrix_pgm, rotated_p2);
		}
			}
	if (file_format(filename) == 3 || file_format(filename) == 6) {
		if (ok == 0 || all == 1) {
			pixel_t **rotated_p3 = rotate_ppm(*width, *height,
			*matrix_ppm, &inverse, u);
			*matrix_ppm = free_ppm(*height, *matrix_ppm);
			*matrix_ppm = rotated_p3;
				if (inverse == 1) {
					int temp2 = *height;
					*height = *width;
					*width = temp2;
				}
		} else {
			pixel_t **rotated_p3 = rotate_ppm(x2 - x1, y2 - y1, *selection_ppm,
											  &inverse, u);
			*selection_ppm = free_ppm(y2 - y1, *selection_ppm);
			*selection_ppm = rotated_p3;
			*matrix_ppm = insert_ppm(*width, *height, x1, y1, x2, y2,
									 *matrix_ppm, rotated_p3);
		}
	}
}

// se va aplica equalize matricei
void equalize(char *filename, int width, int height, int ***matrix_pgm,
			  int x1, int y1, int x2, int y2, int ***selection_pgm,
			  int ok_load, int ok)
{
	int **equalized_p2;
	if (!ok_load) {
		printf("No image loaded\n");
		return;
	}
	if (file_format(filename) == 3 || file_format(filename) == 6) {
		printf("Black and white image needed\n");
		return;
	}
	printf("Equalize done\n");
	equalized_p2 = equalized_matrix(width, height, *matrix_pgm);
	*matrix_pgm = equalized_p2;
	if (ok == 1) {
		*selection_pgm = free_pgm(y2 - y1, *selection_pgm);
		*selection_pgm = select_pgm(*matrix_pgm, x1, y1, x2, y2);
	}
}

// se va afisa histograma imaginii
void histogram(char *filename, int width, int height, int **matrix_pgm,
			   int ok_load)
{
	if (!ok_load) {
		printf("No image loaded\n");
		return;
	}
	char *parameter1 = strtok(NULL, " ");
	if (!parameter1) {
		printf("Invalid command\n");
		return;
	}
	int x = convert_string_to_int(parameter1);

	char *parameter2 = strtok(NULL, " ");
	if (!parameter2) {
		printf("Invalid command\n");
		return;
	}
	int y = convert_string_to_int(parameter2);

	char *parameter3 = strtok(NULL, " ");
	if (parameter3) {
		printf("Invalid command\n");
		return;
	}

	if (!parameter1 || !parameter2 || is_pow2(y) == 0) {
		printf("Invalid command\n");
	} else {
		if (file_format(filename) == 3 || file_format(filename) == 6) {
			printf("Black and white image needed\n");
		} else {
			calculate_histogram(width, height, matrix_pgm, x, y);
		}
	}
}

// se va aplica efectul dorit pe sectiunea selectata
void apply(char *filename, int width, int height, int x1,
		   int x2, int y1, int y2, pixel_t ***matrix_ppm, int ok_load)
{
	if (!ok_load) {
		printf("No image loaded\n");
		return;
	}
	char *parameter = strtok(NULL, " ");
	if (!parameter) {
		printf("Invalid command\n");
		return;
	}
	if (file_format(filename) == 2 || file_format(filename) == 5) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	*matrix_ppm = f0(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f1(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f2(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f3(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f4(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f5(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f6(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f7(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f8(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f9(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f10(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f11(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f12(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f13(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f14(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
	*matrix_ppm = f15(parameter, width, height, x1, x2, y1, y2, *matrix_ppm);
}

int main(void)
{
	int width, height, x1, x2, y1, y2, ok = 0, ok_load = 0;
	int intensity, all = 0;
	FILE *file = NULL;
	int **matrix_p2 = NULL, **selection_p2 = NULL;
	pixel_t  **matrix_p3 = NULL, **selection_p3 = NULL;

	char filename[1000], format[1000], input[1000];

	while (1) {
		fgets(input, sizeof(input), stdin);
		input[strcspn(input, "\n")] = '\0';
		char *command = strtok(input, " ");

		if (strcmp(command, "EXIT") == 0) {
			exit_command(height, &matrix_p2, &matrix_p3, ok_load);
			break;
		} else if (strcmp(command, "LOAD") == 0) {
			load(file, filename, format, &width, &height,
				 &intensity, &matrix_p2, &matrix_p3, &ok_load,
				 &x1, &y1, &x2, &y2);
		} else if (strcmp(command, "SELECT") == 0) {
			select_command(format, width, height, matrix_p2, matrix_p3,
						   &selection_p2, &selection_p3, &x1, &y1, &x2, &y2,
						   ok_load, &ok, &all);
		} else if (strcmp(command, "CROP") == 0) {
			crop(filename, &width, &height, &matrix_p2, &matrix_p3,
				 x1, y1, x2, y2, selection_p2, selection_p3, ok_load,
				 &ok, all);
		} else if (strcmp(command, "SAVE") == 0) {
			save(filename, format, width, height,
				 intensity, matrix_p2, matrix_p3, ok_load);
		} else if (strcmp(command, "ROTATE") == 0) {
			rotate(filename, &width, &height, &matrix_p2,
				   &matrix_p3, x1, y1, x2, y2, &selection_p2, &selection_p3,
				   ok_load, ok, all);
		} else if (strcmp(command, "HISTOGRAM") == 0) {
			histogram(filename, width, height, matrix_p2, ok_load);
		} else if (strcmp(command, "EQUALIZE") == 0) {
			equalize(filename, width, height, &matrix_p2, x1, y1, x2, y2,
					 &selection_p2, ok_load, ok);
		} else if (strcmp(command, "APPLY") == 0) {
			apply(filename, width, height, x1, x2, y1, y2,
				  &matrix_p3, ok_load);
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
