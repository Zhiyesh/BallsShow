#include "inc/gamingwidget.h"

GamingWidget::GamingWidget(QWidget *parent)
    : QWidget(parent)
{
    //设置主界面
    this->ui_size[0] = WIDTH;
    this->ui_size[1] = HEIGHT;
    this->setFixedSize(ui_size[0], ui_size[1]);
    this->setStyleSheet("background-color: #262626;");
    this->setCursor(Qt::CrossCursor);

    this->position[0] = ui_size[0] / 2 - 25;
    this->position[1] = ui_size[1] / 2 - 25;
    this->__game_time = 0;

    //食物
    srand((unsigned)time(NULL));
    for (int i = 0; i < FOOD_NUM; i++)
    {
        foods[i] = new Food(this);
        foods[i]->move(rand() % WIDTH, rand() % HEIGHT);
    }

    this->existingfoodnum = FOOD_NUM;

    //你和其他玩家
    this->player = new Ball(this, 50, PLAYER_DEFAULT_SPEED, "#2dbc36", true);
    player->move(position[0], position[1]);
    this->__is_gameover = false;
    for (int i = 0; i < OTHERPLAYERS; i++)
    {
        this->allplayers[i] = new Ball(this, rand() % 90 + 30, OTHERS_AMEND_SPEED, QString("rgb(%1,%2,%3)")
                                  .arg(rand() % 225 + 15)
                                  .arg(rand() % 225 + 15)
                                  .arg(rand() % 225 + 15));
        allplayers[i]->updatePos(rand() % (WIDTH-200) + 100, rand() % (HEIGHT-200) + 100);
    }
    this->allplayers[OTHERPLAYERS] = player;

    //界面刷新
    this->__main_timer_id = this->startTimer(95);

    //更新随机坐标
    this->updateRandPos = new QTimer(this);
    updateRandPos->setInterval(4e3);
    connect(updateRandPos, &QTimer::timeout, [&](){
        srand((unsigned)time(NULL));
        for (int i = 0; i < OTHERPLAYERS; i++)
        {
            int __index = rand() % FOOD_NUM;
            while (!foods[__index]->isExist()) __index = rand() % FOOD_NUM;
            this->__other_move_targetx[i] = foods[__index]->getCircleX();
            this->__other_move_targety[i] = foods[__index]->getCircleY();
        }
        updateRandPos->setInterval(rand() % 3000 + 1000);
    });
    updateRandPos->start();

    //弱化背景
    this->weakeningbg = new QLabel(this);
    weakeningbg->resize(ui_size[0], ui_size[1]);
    weakeningbg->hide();
    weakeningbg->setStyleSheet("background-color: rgb(206, 206, 206, 45);");
    weakeningbg->setCursor(Qt::ArrowCursor);

    //重生按钮
    this->again = new QPushButton(this);
    again->resize(200, 100);
    again->move((ui_size[0] / 2) - (again->width() / 2), (ui_size[1] / 2) - (again->height() / 2));
    again->setText("重生");
    again->hide();
    again->setCursor(Qt::PointingHandCursor);
    again->setStyleSheet("QPushButton {"
                         "font-size: 46pt;"
                         "color: rgb(211, 211, 211);"
                         "background-color: rgb(0, 0, 0, 0);"
                         "}"
                         "QPushButton::hover {"
                         "color: rgb(59, 197, 114);"
                         "}");
    connect(again, &QPushButton::clicked, [&](){
        delete this->player;
        this->player = new Ball(this, 50, PLAYER_DEFAULT_SPEED, "#2dbc36", true);
        this->position[0] = rand() % (WIDTH - 100) + 50;
        this->position[1] = rand() % (HEIGHT - 100) + 50;
        player->move(position[0], position[1]);
        this->player->show();
        again->hide();
        weakeningbg->hide();
        this->__is_gameover = false;
    });
}

void GamingWidget::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != this->__main_timer_id) return;

    this->__game_time++;

//BiggerDetermination
    _biggerDetermination();

//SpeedDecline
    _speedDecline();

//OthersMoving
    _othersMoving();

//EatFood
    _eatFood();

//EatingAndEated
    _eatingAndEated();

//AutoResetOther
    _autoResetOther();

//CreateNewFood
    _createNewFood();

//IsGameOver
    _gameOver();
if (this->__is_gameover) return;

//PlayerMoving
    _playerMoving();
}

void GamingWidget::sortPlayersBySize(Ball** players, int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (players[j]->getSize() > players[j + 1]->getSize())
            {
        std::swap(players[j], players[j + 1]);
            }
        }
    }
}

void GamingWidget::_autoResetOther()
{
    for (int i = 0; i < OTHERPLAYERS; i++)
    {
        if (this->allplayers[i]->isExist()) continue;
        delete this->allplayers[i];
        this->allplayers[i] = new Ball(this, rand() % 90 + 30, OTHERS_AMEND_SPEED, QString("rgb(%1,%2,%3)")
                                   .arg(rand() % 225 + 25)
                                   .arg(rand() % 225 + 25)
                                   .arg(rand() % 225 + 25));
        allplayers[i]->updatePos(rand() % (WIDTH-200) + 100, rand() % (HEIGHT-200) + 100);
    }
}

void GamingWidget::_gameOver()
{
    if (!this->player->isExist())
    {
        this->weakeningbg->show();
        this->weakeningbg->raise();
        this->again->show();
        this->again->raise();
        this->__is_gameover = true;
    }
}

void GamingWidget::_biggerDetermination()
{
    sortPlayersBySize(this->allplayers, ALLPLAYERS_COUNTS);

    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        this->allplayers[i]->raise();
        if (this->allplayers[i]->isExist())
        {
            this->allplayers[i]->show();
        }
        else this->allplayers[i]->hide();
    }
}

void GamingWidget::_speedDecline()
{
    double __decline_scale;
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        __decline_scale = (allplayers[i]->getSize() * allplayers[i]->getSize()) / 161576;
        if (__decline_scale > 0)
        {
            if (allplayers[i]->isMe())
            {
                allplayers[i]->setSpeed(PLAYER_DEFAULT_SPEED - (__decline_scale));
            }
            else allplayers[i]->setSpeed(OTHERS_AMEND_SPEED - (__decline_scale));
        }
    }
}

void GamingWidget::_playerMoving()
{
    //Mouse Position
    QPoint mouse = this->mapFromGlobal(QCursor::pos());
    int mx = mouse.x();
    int my = mouse.y();
    double playerspeed = player->getSpeed();
    if (Ball::doubleAbs(mx - position[0]) > 10 || Ball::doubleAbs(my - position[1]) > 10)
    {
        float xgrow = 0.004f;
        float tan = Ball::doubleAbs((my - position[1]) / (mx - position[0]));
        tan = tan > 1 ? 1 : tan < 0.2 ? 0.2 : tan;

        if (my > position[1] && Ball::doubleAbs(mx - position[0]) < 5)  //direction: straightdown
        {
            float ytarget = position[1] + playerspeed;
            while (position[1] < ytarget)
            {
                position[1] += xgrow;
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                player->updatePos(position[0], position[1]);
            }
        }
        else if (my < position[1] && Ball::doubleAbs(mx - position[0]) < 5)  //direction: straightup
        {
            float ytarget = position[1] - playerspeed;
            while (position[1] > ytarget)
            {
                position[1] -= xgrow;
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                player->updatePos(position[0], position[1]);
            }
        }
        else if (mx > position[0] && my > position[1])  //direction: rightdown
        {
            float xtarget = position[0] + playerspeed;
            float ytarget = position[1] + (playerspeed * tan);
            while (position[0] < xtarget || position[1] < ytarget)
            {
                position[0] += xgrow;
                position[1] += xgrow * tan;
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                player->updatePos(position[0], position[1]);
            }
        }
        else if (mx > position[0] && my < position[1])  //direction: rightup
        {
            float xtarget = position[0] + playerspeed;
            float ytarget = position[1] - (playerspeed * tan);
            while (position[0] < xtarget || position[1] > ytarget)
            {
                position[0] += xgrow;
                position[1] -= xgrow * tan;
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                player->updatePos(position[0], position[1]);
            }
        }
        else if (mx < position[0] && my > position[1])  //direction: leftdown
        {
            float xtarget = position[0] - playerspeed;
            float ytarget = position[1] + (playerspeed * tan);
            while (position[0] > xtarget || position[1] < ytarget)
            {
                position[0] -= xgrow;
                position[1] += xgrow * tan;
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                player->updatePos(position[0], position[1]);
            }
        }
        else if (mx < position[0] && my < position[1])  //direction: leftup
        {
            float xtarget = position[0] - playerspeed;
            float ytarget = position[1] - (playerspeed * tan);
            while (position[0] > xtarget || position[1] > ytarget)
            {
                position[0] -= xgrow;
                position[1] -= xgrow * tan;
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                player->updatePos(position[0], position[1]);
            }
        }
    }
}

void GamingWidget::_othersMoving()
{
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        if (allplayers[i]->isMe()) continue;
        int __i = i % OTHERPLAYERS;
        if (this->allplayers[__i]->isExist())
        {
            int curx = allplayers[__i]->getCircleX();
            int cury = allplayers[__i]->getCircleY();
            if (__other_move_targetx[__i] > curx && __other_move_targety[__i] > cury)  //rightdown
            {
                curx += this->allplayers[__i]->getSpeed();
                cury += this->allplayers[__i]->getSpeed();
            }
            else if (__other_move_targetx[__i] > curx && __other_move_targety[__i] < cury)  //rightup
            {
                curx += this->allplayers[__i]->getSpeed();
                cury -= this->allplayers[__i]->getSpeed();
            }
            else if (__other_move_targetx[__i] < curx && __other_move_targety[__i] > cury)  //leftdown
            {
                curx -= this->allplayers[__i]->getSpeed();
                cury += this->allplayers[__i]->getSpeed();
            }
            else if (__other_move_targetx[__i] < curx && __other_move_targety[__i] < cury)  //leftup
            {
                curx -= this->allplayers[__i]->getSpeed();
                cury -= this->allplayers[__i]->getSpeed();
            }
            allplayers[__i]->updatePos(curx, cury);
        }
    }
}

void GamingWidget::_eatFood()
{
    for (int i = 0; i < FOOD_NUM; i++)
    {
        for (int j = 0; j < ALLPLAYERS_COUNTS; j++)
        {
            if (this->allplayers[j]->isExist())
            {
                if (foods[i]->isExist())
                {
                    foods[i]->show();

                    //be eat
                    int cx = this->allplayers[j]->getCircleX();
                    int cy = this->allplayers[j]->getCircleY();
                    int fdcx = foods[i]->getCircleX();
                    int fdcy = foods[i]->getCircleY();
                    int d = sqrt(((cx - fdcx)*(cx - fdcx)) + ((cy - fdcy)*(cy - fdcy)));
                    if (d + (foods[i]->getSize() / 2) <= (this->allplayers[j]->getSize() / 2))
                    {
                        foods[i]->setExist(false);
                        foods[i]->hide();
                        existingfoodnum--;
                        if (this->allplayers[j]->getSize() < BALL_MAX_SIZE)
                        {
                            this->allplayers[j]->growUp(foods[i]->getSize());
                        }
                    }
                }
                else foods[i]->hide();
            }
        }
    }
}

void GamingWidget::_eatingAndEated()
{
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        for (int j = i; j < ALLPLAYERS_COUNTS; j++)
        {
            //player[j] eat player[i]
            if (this->allplayers[i]->getSize() < this->allplayers[j]->getSize())
            {
                int x1, x2, y1, y2;
                x1 = this->allplayers[i]->getCircleX();
                x2 = this->allplayers[j]->getCircleX();
                y1 = this->allplayers[i]->getCircleY();
                y2 = this->allplayers[j]->getCircleY();
                if (sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) + (this->allplayers[i]->getSize() / 2) <= (this->allplayers[j]->getSize() / 2))
                {
                    this->allplayers[i]->setExist(false);
                    this->allplayers[i]->hide();
                    if (this->allplayers[j]->getSize() < BALL_MAX_SIZE)
                    {
                        this->allplayers[j]->growUp(this->allplayers[i]->getSize());
                    }
                }
            }
        }
    }
}

void GamingWidget::_createNewFood()
{
    if (existingfoodnum < static_cast<unsigned int>((FOOD_NUM - static_cast<int>(this->__game_time / 6e2)) / 2))
    {
        for (int i = 0; i < static_cast<int>(FOOD_NUM - static_cast<int>(this->__game_time / 6e2)); i++)
        {
            if (!foods[i]->isExist())
            {
                delete foods[i];
                foods[i] = new Food(this);
                foods[i]->move(rand() % WIDTH, rand() % HEIGHT);
                foods[i]->lower();
                existingfoodnum++;
            }
        }
    }
}

GamingWidget::~GamingWidget()
{

}
