#include "inc/food.h"

Food::Food(QWidget *parent)
    : Ball(parent)
{
    // The size will be [6, 16)
    this->__size = rand() % 10 + 6;
    this->resize(this->__size, this->__size);
    this->setStyleSheet(QString("QLabel {"
                        "background-color: %1;"
                        "border-radius: %2px;"
                        "}")
                        .arg(QString("rgb(%1, %2, %3)")
                             .arg(rand() % 255)
                             .arg(rand() % 255)
                             .arg(rand() % 255))
                        .arg(static_cast<int>((this->__size / 2) - (rand() % 3 + 1))));
    this->__is_exist = true;
}

Food::~Food()
{

}
