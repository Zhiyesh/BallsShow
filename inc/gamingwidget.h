#ifndef GAMINGWIDGET_H
#define GAMINGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QBasicTimer>
#include <QTimer>
#include <QCursor>
#include <random>
#include <ctime>
#include <cmath>
#include <QCoreApplication>
#include "inc/ball.h"
#include "inc/food.h"
#include "inc/sleep.h"

#include <QDebug>

#define WIDTH                   1460
#define HEIGHT                  980

#define BALL_SIZE               10
#define PLAYER_DEFAULT_SPEED    12
#define OTHERS_AMEND_SPEED      PLAYER_DEFAULT_SPEED - 1.8
#define BALL_MAX_SIZE           WIDTH - 300
#define FOOD_NUM                128
#define OTHERPLAYERS            6
#define ALLPLAYERS_COUNTS       OTHERPLAYERS + 1

class GamingWidget : public QWidget
{
    Q_OBJECT

public:
    GamingWidget(QWidget *parent = 0);
    ~GamingWidget();

public slots:
    void sortPlayersBySize(Ball **players, const int& size);

private slots:
    void _gameOver();
    void _gameWinner();
    void _biggerDetermination();
    void _blinkMe();
    void _speedDecline();
    void _playerMoving();
    void _othersMoving();
    void _eatFood();
    void _eatingAndEated();
    void _createNewFood();
    void _autoShowAndHide();
    void _autoResetOther();
    void _showTipScreen(const QString& content);
    int _getMyIdFromAllplayers() const;

protected:
    void timerEvent(QTimerEvent* event);

private:
    unsigned int ui_size[2];
    float position[2];
    int __main_timer_id;
    int __a_few_time;
    Ball* all_players[ALLPLAYERS_COUNTS];
    unsigned int __game_time;
    int __other_move_targetx[OTHERPLAYERS];
    int __other_move_targety[OTHERPLAYERS];
    QTimer* update_rand_pos;
    QTimer* buffer_timer;
    Food* foods[FOOD_NUM];
    unsigned int existing_food_num;
    QPushButton* again_or_end;
    QLabel* weakening_bg;
    bool __is_gameover;
    bool __is_just_reseted;
};

#endif // GAMINGWIDGET_H
