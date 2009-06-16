#include <iostream>
#include <cstdio>
#include "mandel.hpp"

void CowichanLinuxTuples::mandel(IntMatrix matrix) {
	LTMandel mandelApp;
	mandelApp.addOutput(0, matrix);
	mandelApp.start(SERVER, PORT, NUM_WORKERS);
}

void LTmandel::consumeInput() {

	// tuple template
	tuple *send = make_tuple("si", "mandel request");

	// send off a mandelbrot request for each grid row.
	for (size_t y = 0; y < NROWS; ++y) {
		send->elements[1].data.i = y;
		put_tuple(send, &ctx);
	}
	
	// destroy the template tuple
	destroy_tuple(send);

}

int LTmandel::mandelCalc(real x, real y) {

	real r = 0.0, i = 0.0;
	real rs = 0.0, is = 0.0;
	int numIterations = 0;		
	do {

		// calculate the complex value according to the mandelbrot set specs.
		i = (2.0 * r * i) + x;
		r = (rs - is) + y;
	
		// calculate squared complex value
		rs = r * r;
		is = i * i;			
	
		// "step" the simulation for this co-ordinate.
		++numIterations;			
	
	} while ((numIterations < MANDEL_MAX_ITER) && ((rs + is) < MANDEL_INFINITY));

	// we are interested if the series converges or diverges. Return the
	// number of iterations before such an event (divergence).
	return numIterations;

}

void LTmandel::work() {

	tuple *recv = make_tuple("s?", "mandel request");
	tuple *send = make_tuple("sis", "mandel done");
	
	// 2D delta between calculated points
	real dX = width / (NCOLS - 1);
	real dY = height / (NROWS - 1);

	// satisfy mandelbrot requests.
	while (1) {

		// block until we receieve a tuple.
		tuple* gotten = get_tuple(recv, &ctx);

		// copy over row co-ordinate of the computation; create
		// a buffer for the results of the computation.
		send->elements[1].data.i = gotten->elements[1].data.i;
		int* buffer = (int*) malloc(sizeof(int) * NCOLS);
		send->elements[2].data.s.len = sizeof(int) * NCOLS;
		send->elements[2].data.s.ptr = (char*) buffer;

		// perform the actual computation for this row.
		double rY = baseY + dY * send->elements[1].data.i;
		double rX = baseX;
		for (int x = 0; x < NCOLS; ++x, rX += dX) {
			buffer[x] = mandelCalc(rX, rY);
		}
	
		// send off the new tuple and purge local memory of the one we gotten
		put_tuple(send, &ctx);
		destroy_tuple(gotten);

	}

	// TODO destroy the template tuples; must send tuples for this
//	destroy_tuple(send);
//	destroy_tuple(recv);

}

void LTmandel::produceOutput() {

	// tuple template
	tuple *recv = make_tuple("s??", "mandel done");

	// grab output pointer locally.
	IntMatrix output = outputs[0];

	// grab all of the mandelbrot computations from the workers,
	// in an unspecified order.
	int computations = NROWS;
	while (computations > 0) {

		// get the tuple and copy it into the matrix.
		tuple* received = get_tuple(recv, &ctx);
		memcpy(
			&MATRIX_RECT_NC(output, received->elements[1].data.i, 0, NROWS),
			received->elements[2].data.s.ptr,
			received->elements[2].data.s.len);
		computations--;
		destroy_tuple(received);

	}

	// destroy the template tuple
	destroy_tuple(recv);

}
