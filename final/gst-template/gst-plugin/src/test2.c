#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
	FILE *csvFile;
	srand(time(NULL));

	const char *width [] = {"1920", "1280", "640", "320"};
	const char *height [] = {"1080", "720", "480", "240"};
	//const char *format [] = {"NV12","RGB","NA"};
	const char *framerate [] = {"24", "25", "30", "50", "60"};
	const char *shapee [] = {"square","rectangle","circle","triangle"};
	const char *colour [] = {"red","green","blue"};
	const char *fill_mode [] = {"colour","no-fill","copy"};
	
	csvFile = fopen("testcases.csv", "w");

	// Write the header row

	fprintf(csvFile, "width,height,format,framerate,shape-size, x-coordinate, y-coordinate,shape,shape-color,fill-mode,x-copy_ coordinate, y-copy_coordinate,alpha,strict-mode\n");

	for(int a =0; a < 10; a++){ 

		int num = rand() % 4;
		int index_fr = rand() % 5;
		int fmt = rand() % 3;
		
		int w, h;
		if (!strcmp ("NA", width[num])) {
			w = 1920;
			h = 1080;
		}
		else {
			w = atoi (width [num]);
			h = atoi (height [num]);
		}
		
		int size = rand() % 500;
		int x_corr = rand() % w;
		int y_corr = rand() % h;
		//int shape = atoi (shapee [num]);//rand() % 4;
//		int fill_mode = rand() % 3;
//		int color = rand() % 3;
		int x_copy_corr = rand() % w;
		int y_copy_corr = rand() % h; 
		int alpha = rand() % 101;
		int strict_mode = rand() % 2;
		
		fprintf(csvFile, "%s,%s,%s,%s,%d,%d,%d,%s,%s,%s,%d,%d,%d,%d\n", width[num], height[num], "NV12", framerate [index_fr],size, x_corr, y_corr, shapee[num], colour[fmt], fill_mode[fmt], x_copy_corr, y_copy_corr, alpha, strict_mode);
	}
	
	fclose(csvFile);

	return 0;
}

