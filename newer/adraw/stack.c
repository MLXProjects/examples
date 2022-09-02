#include "common.h"

byte adraw_stack_free(ADRAW_STACKP stack){
	if (!stack) return 0;
	int i;
	for (i=stack->max; i>0; i--){
		free(stack->items[i-1]);
	}
	free(stack->items);
	free(stack);
}

ADRAW_STACKP adraw_stack(int max_size){
	ADRAW_STACKP stack = calloc(sizeof(ADRAW_STACK), 1);
	if (!stack){
		loge("stack failed to allocate main storage");
		return NULL;
	}
	ADRAW_POINTP *items = malloc(sizeof(ADRAW_POINTP)*max_size);
	if (!items){
		loge("stack failed to allocate items pointer array");
		free(stack);
		return NULL;
	}
	stack->items=items;
	int i;
	for (i=0; i<max_size; i++){
		ADRAW_POINTP item = malloc(sizeof(ADRAW_POINT));
		if (!item) {
			loge("failed to alloc stack item #%d", i);
			stack->max=i;
			//free items allocated before the fail
			adraw_stack_free(stack);
			return NULL;
		}
		items[i]=item;
	}
	stack->max=max_size;
	return stack;
}

byte adraw_stack_push(ADRAW_STACKP stack, int x, int y){
	if (!stack) return 0;
	if (stack->count==stack->max){
		loge("stack full, push failed");
		return 0;
	}
	ADRAW_POINTP top = stack->items[stack->count];
	if (!top) return 0;
	top->x=x;
	top->y=y;
	stack->count+=1;
	return 1;
}

byte adraw_stack_pop(ADRAW_STACKP stack, int *x, int *y){
	if (!stack) return 0;
	if (stack->count<1){
		return 0;
	}
	stack->count-=1;
	ADRAW_POINTP point = stack->items[stack->count];
	if (x!=NULL) *x=point->x;
	if (y!=NULL) *y=point->y;
	return 1;
}
