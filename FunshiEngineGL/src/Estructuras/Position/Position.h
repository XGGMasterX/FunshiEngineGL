#ifndef POSITION_H
#define POSITION_H

using namespace std;
template <typename E>
class Position {
public:
    virtual E getElement() = 0;
};
#endif

