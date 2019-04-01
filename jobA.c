#include <stdio.h> 
#include <unistd.h>

int main(int argc, char *argv[] ) {
	int i = 0;
	printf("process A is currently running \n");
	for (int z = 0; z < 155; z++){
		for (int i = 0; i < 115; i++){
			for (int x = 0; x < 75; x++){
				for (int y = 0; y < 15; y++){
					// do nothing, just wast time
				}
			}
		}
	}
	sleep(3);
	
	printf("process A is is finished \n");
	return 0;
}
