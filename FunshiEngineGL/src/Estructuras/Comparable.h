#ifndef COMPARABLE_H
#define COMPARABLE_H

using namespace std;

template<typename E>
class Comparable {
	public:
		virtual int compareTo(E* p1) = 0;
};
#endif