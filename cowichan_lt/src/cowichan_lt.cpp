#include "cowichan_lt.hpp"

int main(int argc, char* argv[]) {
	Cowichan* linuxTuples = new CowichanLinuxTuples();
	linuxTuples->main(argc, argv, false, true);
	return 0;
}

const char* CowichanLinuxTuples::SERVER = "localhost";

void CowichanLinuxTuples::winnow(IntMatrix /* matrix */, BoolMatrix /* mask */, PointVector /* points */) {}
void CowichanLinuxTuples::hull(PointVector /* pointsIn */, PointVector /* pointsOut */) {}
void CowichanLinuxTuples::outer(PointVector /* points */, Matrix /* matrix */, Vector /* vector */) {}
void CowichanLinuxTuples::sor(Matrix /* matrix */, Vector /* target */, Vector /* solution */) {}

void CowichanLinuxTuples::chain(bool /* use_randmat */, bool /* use_thresh */) {}
