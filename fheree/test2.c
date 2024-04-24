#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
	FILE *csvFile;
	srand(time(NULL));

	const char *width [] = {"1920", "1280", "640", "320", "NA"};
	const char *height [] = {"1080", "720", "480", "240", "NA"};
	const char *format [] = {"NV12","RGB","NA"};
	const char *framerate [] = {"24", "25", "30", "48", "50", "60","NA"};

	csvFile = fopen("testcases.csv", "w");

	// Write the header row

	fprintf(csvFile, "format,width,height,framerate,shape-size,draw-coordinates,shape,fill-mode,shape-color,copy- coordinate,alpha,strict-mode\n");

	for(int a =0; a < 20; a++){ 

		int num = rand() % 5;
		int index_fr = rand() % 7;
		int fmt = rand() % 3;
		
		int w, h;
		if (!strcmp ("NA", width[num])) {
			w = 320;
			h = 240;
		}
		else {
			w = atoi (width [num]);
			h = atoi (height [num]);
		}
		int size = rand() % 500;
		int x_corr = rand() % w;
		int y_corr = rand() % h;
		int shape = rand() % 4;
		int fill_mode = rand() % 3;
		int color = rand() % 3;
		int x_copy_corr = rand() % w;
		int y_copy_corr = rand() % h; 
		int alpha = rand() % 101;
		int strict_mode = rand() % 2;
		
		fprintf(csvFile, "%s,%s,%s,%s,%d,\"%d,%d\",%d,%d,%d,\"%d,%d\",%d,%d\n", format[fmt], width[num], height[num], framerate [index_fr],
				size, x_corr, y_corr, shape, fill_mode, color,x_copy_corr, y_copy_corr, alpha, strict_mode);
	}
	
	fclose(csvFile);

	return 0;
}

