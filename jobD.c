#include <stdio.h> 
#include <unistd.h>

int main(int argc, char *argv[] ) {
	int i = 0;
	printf("process A is currently running \n");
	for (int z = 0; z < 15; z++){
		for (int i = 0; i < 21; i++){
			for (int x = 0; x < 74; x++){
				for (int y = 0; y < 13; y++){
					// do nothing, just wast time
				}
			}
		}
	}
	sleep(17);
	
	printf("process A is is finished \n");
	return 0;
}
