#ifndef BALL_H
#define BALL_H

#include <QLabel>

class Ball : public QLabel
{
    Q_OBJECT

public:
    explicit Ball(QWidget *parent = 0, const int& size = 0, const double& speed = 0, const QString& color = Q_NULLPTR, const bool& _isMe = false);
    ~Ball();

public slots:
    static double doubleAbs(double num);
    bool isExist() const;
    void setExist(bool status);
    double getSize() const;
    bool reSize(const double& size);
    double getSpeed() const;
    void setSpeed(const double& speed);
    void setColor(const QString& color);
    void growUp(const double& size);
    bool updateQuality();
    void updatePos(const int& x, const int& y);
    int getCircleX() const;
    int getCircleY() const;
    bool isMe() const;
    int getRank() const;
    void setRank(const int& rank);

protected:
    double __size;
    bool __is_exist;

private:
    bool _isMe;
    double __speed;
    int __rank;
    QString __color;
};

#endif // BALL_H
