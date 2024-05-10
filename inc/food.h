#ifndef FOOD_H
#define FOOD_H

#include "inc/ball.h"
#include <random>

namespace Ui {
class Food;
}

class Food : public Ball
{
    Q_OBJECT

public:
    explicit Food(QWidget *parent = 0);
    ~Food();
};

#endif // FOOD_H
