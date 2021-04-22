#include <aroma.h>
void draw_gradient();
void test_values();

int main(){
	libaroma_start();
	pid_t pp = getppid();
	kill(pp, 19); 
	draw_gradient();
	libaroma_end();
	kill(pp, 18); 
}

void draw_gradient(){
	byte startR=0x00;
	byte startG=0x00;
	byte startB=0x00;
	byte endR=0x00;
	byte endG=0x00;
	byte endB=0x00;
	int i=0;
	int target=1;
	byte debug=0;
	while (i<=255*9){		
	//printf("Started gradient draw\n");
		switch (target){
			case 1: //draw to red
				startR+=5;
				endG+=5;
				break;
			case 2: //draw to yellow
				startG+=5;
				endB+=5;
				break;
			case 3: //draw to green
				startR-=5;
				endG-=5;
				break;
			case 4: //draw to cyan
				startB+=5;
				endR+=5;
				break;
			case 5: //draw to blue
				startG-=5;
				endB-=5;
				break;
			case 6: //draw to white
				startG+=5;
				endB+=5;
				startR+=5;
				endG+=5;
				break;
			case 7:
				startR=endR=0xFF;
				startG=endG=0x00;
				startB=endB=0x00;
				break;
			case 8:
				startR=endR=0x00;
				startG=endG=0xFF;
				startB=endB=0x00;
				break;
			case 9:
				startR=endR=0x00;
				startG=endG=0x00;
				startB=endB=0xFF;
				break;
		}
		if (i==255*1) target=2;
		else if (i==255*2) target=3;
		else if (i==255*3) target=4;
		else if (i==255*4) target=5;
		else if (i==255*5) target=6;
		else if (i==255*6) target=7;
		else if (i==255*7) target=8;
		else if (i==255*8) target=9;
		libaroma_gradient_ex1(libaroma_fb()->canvas, 0, 0, libaroma_fb()->w, libaroma_fb()->h, 
								libaroma_rgb(startR, startG, startB), libaroma_rgb(endR, endG, endB), 0, 0, 0xFF, 0xFF, 0);
		libaroma_fb_sync();
		if (debug)
			printf("Target is %d\n"
					"startR: %d, endR: %d\n"
					"startG: %d, endG: %d\n"
					"startB: %d, endB: %d\n",
					target, startR, endR, startG, endG, startB, endB);
					/*
		if (startR==0xFF && startG==0x00 && startB==0x00) target=2; //if red, go yellow
		else if (startR==0xFF && startG==0xFF && startB==0x00) target=3; //if yellow, go green
		else if (startR==0x00 && startG==0xFF && startB==0x00) target=4; //if green, go cyan
		else if (startR==0x00 && startG==0xFF && startB==0xFF) target=5; //if cyan, go blue
		else if (startR==0x00 && startG==0x00 && startB==0xFF) target=6; //if blue, go white
		else if (startR==0xFF && startG==0xFF && startB==0xFF) break; //if white, exit
		*/
		if (i>=255*6) i+=3;
		else i+=5;
	}
	libaroma_canvas_blank(libaroma_fb()->canvas);
	libaroma_fb_sync();
}

		//long start=libaroma_tick();
		//printf("Finished drawing in %ld ms, I is %d and target is %d\n", libaroma_tick()-start, i, target);
void test_values(){
	word i;
	
	for (i=0; i<0xFF; i++){
		printf("%d\n", i);
	}
}