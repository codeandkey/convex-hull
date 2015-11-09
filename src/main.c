#include "data_points.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

struct ch_chain { /* This will be used in a special type of linked list: it will grow in two directions. (in multithreaded optimizations) */
	struct ch_chain* next, *prev;
	int point;
};

static int line_partition(struct ch_point* first, struct ch_point* second, struct ch_point* test);
static int line_test(int i, int j);

int main(int argc, char** argv) {
	struct ch_chain* chain_front = NULL, *chain_back = NULL;

	int* in_chain = malloc(sizeof(int) * ch_point_list_size); /* This list is used for fast testing if a point is in the chain. */
	memset(in_chain, 0, sizeof(int) * ch_point_list_size);

	/* Fast implementation of a solution to the convex-hull problem.
	 * We look for lines which can become a part of the convex hull boundary.
	 * We also use several optimization passes and make use of dynamic programming to speed up the algorithm. */

	// First, we must find any line which is part of the boundary. This is a random search. Things will get faster later.

	for (int i = 0; i < ch_point_list_size - 1; ++i) {
		for (int j = i + 1; j < ch_point_list_size; ++j) {
			int lp = line_test(i, j);

			if (lp == -1) {
				printf("There must be at least 3 points!\n");
				exit(-1);
			}

			if (lp == -2) {
				continue; /* Find next pair, this is not part of the hull */
			}

			/* At this point, the pair is on the hull. We start the chain and break the loop. */
			chain_front = malloc(sizeof(struct ch_chain));
			chain_back = malloc(sizeof(struct ch_chain));

			chain_front->prev = chain_back;
			chain_back->next = chain_front;
			chain_front->next = chain_back->prev = NULL;

			chain_front->point = i;
			chain_back->point = j;

			in_chain[i] = in_chain[j] = 1;

			i = ch_point_list_size; /* Setting this is an odd but efficient way to kill the loop. */
			j = ch_point_list_size;
		}
	}

	/* From here, we know that all ends of the chain will soon become points to new lines. */
	/* We can use this information and narrow down the possible line combinations. */
	/* This can be multithreaded but we will instead just augment the chain one end at at time. */

	/* Since this is singlethreaded, we will only approach one end of the chain.
	 * We know that eventually, each end will connect. (forming the loop of the hull)
	 * !! A multithreaded application would easily take advantage of this and attack both ends of the chain concurrently. !! */

	struct ch_chain* cur = chain_front;

	int chain_flag = 1;
	while (chain_flag) {
		/* We want to extend the front of the chain. We search for points NOT in the current chain. in_chain will help with performance. */

		int first_point = cur->point;
		int found_line = 0;

		for (int i = 0; i < ch_point_list_size; ++i) {
			if (in_chain[i]) {
				continue; // This saves us a LOT of time.
			}

			int lp = line_test(first_point, i);

			if (lp >= 0) {
				if (i == chain_back->point) {
					chain_flag = 0;
					break;
				}

				struct ch_chain* new = malloc(sizeof(struct ch_chain));
				new->prev = cur;
				cur->next = new;
				chain_front = new;
				cur = new;
				new->point = i;
				in_chain[i] = 1;
				found_line = 1;
				break;
			}
		}

		if (!found_line) {
			/* There are no more lines to make. The chain is complete. */
			break;
		}
	}

	/* At this point, the chain has gone full circle. We print the list. */
	cur = chain_back;

	while (cur) {
		printf("%d ", cur->point);

		cur = cur->next;
	}

	printf("\n");
	return 0;
}

int line_partition(struct ch_point* first, struct ch_point* second, struct ch_point* test) {
	/* To determine whether a point is on a certain side of a line, we use a linear algebraic trick.
	 * The determined value is the Z value of the cross-product between the line and the point.
	 * This will actually change positivity when the point is on the other side of the line. */

	return ((second->x - first->x) * (test->y - first->y) - (second->y - first->y) * (test->x - first->x)) > 0;
}

int line_test(int i, int j) {
	int lp = -1; /* Current line partition: all lines should have the same partition value, it is stored here for comparing */
	// lp is also used to report failure: see codes below

	for (int k = 0; k < ch_point_list_size; ++k) {
		if (k == i || k == j) continue;
		int part = line_partition(ch_point_list + i, ch_point_list + j, ch_point_list + k);

		if (lp == -1) {
			lp = part; /* Initial partition, we test to see if the other points have this value as well. */
		} else {
			if (lp != part) {
				lp = -2; /* Partition mismatch. Set code and stop. */
				break;
			}
		}
	}

	return lp;
}
