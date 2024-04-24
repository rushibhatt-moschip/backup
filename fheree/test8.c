#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

/*void run_pipeline(int w, int h, char *ff, int a, int x, int y, char *ss, char *sc, char *rot, char *logo, int st) {
	char pipeline[MAX_LINE_LENGTH];

	sprintf(pipeline, "gst-launch-1.0 videotestsrc ! video/x-raw,width=%d,height=%d,format=%s ! insertlogo alpha=%d coordinate='<%d,%d>' speed=%s scrolling=%s rotation=%s logo-file=%s strict-mode=%d ! autovideosink", w, h, ff, a, x, y, ss, sc, rot, logo, st);

	printf("Running pipeline: %s\n", pipeline);

	 system(pipeline); // Commented for testing purposes
//	printf("Pipeline execution skipped for testing.\n");
}
*/

int main(int argc, char *argv[]) {
	FILE *file = fopen("TestCase.csv", "r");

	if (file == NULL) {
		perror("Error opening file");
		return 1;
	}

	int row;
	printf("Enter the test case: ");
	scanf("%d", &row);

	char line[MAX_LINE_LENGTH];
	char *token;
	
	//manmipulate
	char format[MAX_LINE_LENGTH], shape[MAX_LINE_LENGTH], color[MAX_LINE_LENGTH], fill[MAX_LINE_LENGTH];
	int width, height, size , x, y, alpha, strict, xx, yy;
	//done
	
	int current_row = 0;

	// Read and discard lines until reaching the desired row
	while (current_row < row && fgets(line, sizeof(line), file) != NULL) {
		current_row++;
	}

	if (current_row != row) {
		printf("Row %d not found in the file.\n", row);
		fclose(file);
		return 1;
	}

	// Tokenize line
	token = strtok(line, ",");
	if (token != NULL) {
		width = atoi(token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		height = atoi(token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		strcpy(format, token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		size = atoi(token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		x = atoi(token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		y = atoi(token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		strcpy(shape, token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		strcpy(color, token);
	}

	token = strtok(NULL, ",");
	if (token != NULL) {
		strcpy(fill, token);
	}


	token = strtok(NULL, ",");
	if (token != NULL) {
		alpha = atoi(token);
	}
	
	token = strtok(NULL, ",");
	if (token != NULL) {
		strict = atoi(token);
	}

	printf("%d %d %d %d\n", x, y, width, height);
//	run_pipeline(wid, hig, fr, a, x, y, speed, scroll, rotate, logo, strict);

	//char pipeline[MAX_LINE_LENGTH];
char pipeline[2000];

	sprintf(pipeline, "gst-launch-1.0 videotestsrc ! video/x-raw,width=%d,height=%d,format=%s ! drawshapes shape-size=%d draw-coordinate='<%d,%d>' shape=%s shape-color=%s fill-mode=%s alpha=%d strict-mode=%d ! autovideosink", width, height, format, size, x, y, shape, color, fill, alpha, strict);

	printf("Running pipeline: %s\n", pipeline);

	system(pipeline); // Commented for testing purposes

	fclose(file);

	return 0;
}

