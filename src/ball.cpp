#include "inc/ball.h"

Ball::Ball(QWidget *parent, const int& size, const double& speed, const QString& color, const bool& _isMe)
    : QLabel(parent)
{
    this->__size = size;
    this->__speed = speed;
    this->__color = color;
    this->_isMe = _isMe;
    this->__is_exist = true;
    updateQuality();
}

double Ball::doubleAbs(double num)
{
    return num < 0 ? -num : num;
}

bool Ball::isExist() const
{
    return this->__is_exist;
}

void Ball::setExist(bool status)
{
    this->__is_exist = status;
}


double Ball::getSize() const
{
    return this->__size;
}

bool Ball::reSize(const double& size)
{
    this->__size = size;
    return updateQuality();
}

double Ball::getSpeed() const
{
    return this->__speed;
}

void Ball::setSpeed(const double& speed)
{
    this->__speed = speed;
}

void Ball::setColor(const QString& color)
{
    this->__color = color;
    this->updateQuality();
}

void Ball::growUp(const double& size)
{
    this->reSize(this->__size + (size * ((1 / (this->__size + 6)) + 0.13)));
}

bool Ball::updateQuality()
{
    this->resize(this->__size, this->__size);
    this->setStyleSheet(QString("QLabel {"
                        "background-color: %1;"
                        "border-radius: %2px;"
                        "}")
                        .arg(this->__color)
                        .arg(static_cast<int>(this->__size / 2)));
    if (this->size().width() == this->__size)
    {
        return true;
    }
    return false;
}

void Ball::updatePos(const int& x, const int& y)
{
    this->move(x - static_cast<int>(this->__size / 2), y - static_cast<int>(this->__size / 2));
}

int Ball::getCircleX() const
{
    return this->x() + (this->__size / 2);
}

int Ball::getCircleY() const
{
    return this->y() + (this->__size / 2);
}

bool Ball::isMe() const
{
    return this->_isMe;
}

int Ball::getRank() const
{
    return this->__rank;
}

void Ball::setRank(const int& rank)
{
    this->__rank = rank;
}

Ball::~Ball()
{

}
