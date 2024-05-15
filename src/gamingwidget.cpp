#include "inc/gamingwidget.h"

GamingWidget::GamingWidget(QWidget *parent)
    : QWidget(parent)
{
    //设置主界面
    this->screen_size[0] = WIDTH;
    this->screen_size[1] = HEIGHT;
    this->setFixedSize(screen_size[0], screen_size[1]);
    this->setStyleSheet("background-color: #262626;");
    this->setCursor(Qt::CrossCursor);

    this->position[0] = screen_size[0] / 2 - 25;
    this->position[1] = screen_size[1] / 2 - 25;
    this->__game_time = 0;
    this->__a_few_time = 0;
    this->__is_gameover = false;
    this->__is_just_reseted = false;
    this->__have_winner = false;

    //食物
    srand((unsigned)time(NULL));
    for (int i = 0; i < FOOD_NUM; i++)
    {
        foods[i] = new Food(this);
        foods[i]->move(rand() % WIDTH, rand() % HEIGHT);
    }

    this->existing_food_num = FOOD_NUM;

    //你和其他玩家
    this->all_players[OTHERPLAYERS] = new Ball(this, 800, PLAYER_DEFAULT_SPEED, "#2dbc36", true);
    this->all_players[OTHERPLAYERS]->updatePos(position[0], position[1]);
    this->__is_gameover = false;
    for (int i = 0; i < OTHERPLAYERS; i++)
    {
        this->all_players[i] = new Ball(this, rand() % 120 + 30, OTHERS_AMEND_SPEED, QString("rgb(%1,%2,%3)")
                                  .arg(rand() % 225 + 15)
                                  .arg(rand() % 225 + 15)
                                  .arg(rand() % 225 + 15));
        all_players[i]->updatePos(rand() % (WIDTH-200) + 100, rand() % (HEIGHT-200) + 100);
    }

    //界面刷新
    this->__main_timer_id = this->startTimer(115);

    //更新随机坐标
    this->update_rand_pos = new QTimer(this);
    update_rand_pos->setInterval(4e3);
    connect(update_rand_pos, &QTimer::timeout, [&](){
        srand((unsigned)time(NULL));
        for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
        {
            if (this->all_players[i]->isMe())
                continue;
            int __i = i % OTHERPLAYERS;
            for (int j = __i; j < ALLPLAYERS_COUNTS; j++)
            {
                if (this->all_players[i]->getSize() > this->all_players[j]->getSize())
                {
                    this->__other_move_targetx[__i] = this->all_players[j]->getCircleX();
                    this->__other_move_targety[__i] = this->all_players[j]->getCircleY();
                }
                else
                {
                    int __index = rand() % FOOD_NUM;
                    while (!foods[__index]->isExist()) __index = rand() % FOOD_NUM;
                    this->__other_move_targetx[__i] = foods[__index]->getCircleX();
                    this->__other_move_targety[__i] = foods[__index]->getCircleY();
                }
            }
        }
        update_rand_pos->setInterval(rand() % 3000 + 1000);
    });
    update_rand_pos->start();

    this->buffer_timer = new QTimer(this);
    buffer_timer->setInterval(225);
    connect(buffer_timer, &QTimer::timeout, [&](){
        if (this->__is_just_reseted)
            this->__a_few_time++;
        if (this->__a_few_time > 6)
        {
            this->__a_few_time = 0;
            this->__is_just_reseted = false;
        }
        if (!this->__is_just_reseted)
            this->all_players[_getMyIdFromAllplayers()]->setColor("#2dbc36");
    });
    buffer_timer->start();

    //弱化背景
    this->weakening_bg = new QLabel(this);
    weakening_bg->resize(screen_size[0], screen_size[1]);
    weakening_bg->hide();
    weakening_bg->setStyleSheet("background-color: rgb(67, 67, 67, 102);");
    weakening_bg->setCursor(Qt::ArrowCursor);

    //重生按钮或结束语
    this->again_or_end = new QPushButton(this);
    again_or_end->resize(600, 100);
    again_or_end->move((screen_size[0] / 2) - (again_or_end->width() / 2), (screen_size[1] / 2) - (again_or_end->height() / 2));
    again_or_end->hide();
    again_or_end->setCursor(Qt::PointingHandCursor);
    again_or_end->setStyleSheet("QPushButton {"
                         "font-size: 46pt;"
                         "color: rgb(211, 211, 211);"
                         "background-color: rgb(0, 0, 0, 0);"
                         "}"
                         "QPushButton::hover {"
                         "color: rgb(59, 197, 114);"
                         "}");
    connect(again_or_end, &QPushButton::clicked, [&](){
        this->__is_gameover = false;
        this->all_players[_getMyIdFromAllplayers()]->hide();
        this->all_players[_getMyIdFromAllplayers()] = new Ball(this, 50, PLAYER_DEFAULT_SPEED, "#2dbc36", true);
        this->position[0] = rand() % (WIDTH - 100) + 50;
        this->position[1] = rand() % (HEIGHT - 100) + 50;
        this->all_players[_getMyIdFromAllplayers()]->updatePos(position[0], position[1]);
        again_or_end->hide();
        weakening_bg->hide();
        this->__is_just_reseted = true;
        this->__have_winner = false;
        this->__a_few_time = 0;
    });
}

void GamingWidget::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != this->__main_timer_id)
        return;

//BiggerDetermination
    _biggerDetermination();

//BlinkMe
    _blinkMe();

//IsGameOver
    _gameOver();

//GameWinner
    _gameWinner();

if (this->__is_gameover)
    return;

    this->__game_time++;

//SpeedDecline
    _speedDecline();

//OthersMoving
    _othersMoving();

//EatFood
    _eatFood();

//AutoResetOther
    _autoResetOther();

//CreateNewFood
    _createNewFood();

//EatingAndEated
    _eatingAndEated();

//AutoShowAndHidePlayers
    _autoShowAndHide();

//PlayerMoving
    _playerMoving();
}

int GamingWidget::_getMyIdFromAllplayers() const
{
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        if (this->all_players[i]->isMe())
        {
            return i;
        }
    }
    return -1;
}

void GamingWidget::sortPlayersBySize(Ball** players, const int& size)
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

void GamingWidget::_autoShowAndHide()
{
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        if (this->all_players[i]->isExist())
        {
            this->all_players[i]->show();
        }
        else this->all_players[i]->hide();
    }
}

void GamingWidget::_autoResetOther()
{
    if (this->__a_few_time % 2 != 0)
        return;
    srand((unsigned)time(NULL));
    for (int i = 0; i < OTHERPLAYERS; i++)
    {
        if (this->all_players[i]->isExist())
            continue;
        delete this->all_players[i];
        this->all_players[i] = new Ball(this, rand() % 170 + 30, OTHERS_AMEND_SPEED, QString("rgb(%1,%2,%3)")
                                   .arg(rand() % 225 + 25)
                                   .arg(rand() % 225 + 25)
                                   .arg(rand() % 225 + 25));
        all_players[i]->updatePos(rand() % (WIDTH-200) + 100, rand() % (HEIGHT-200) + 100);
    }
}

void GamingWidget::_showTipScreen(const QString& content)
{
    //background
    if (this->weakening_bg->isHidden()) this->weakening_bg->show();
    this->weakening_bg->raise();

    //foreground
    if (this->again_or_end->isHidden())
    {
        this->again_or_end->setText(content);
        this->again_or_end->show();
    }
    this->again_or_end->raise();
}

void GamingWidget::_gameOver()
{
    if (this->all_players[_getMyIdFromAllplayers()]->isExist())
        return;
    this->_showTipScreen("重生");
    this->setEnabled(true);
    this->__is_gameover = true;
}

void GamingWidget::_gameWinner()
{
    if (this->__have_winner)
    {
        this->weakening_bg->raise();
        this->again_or_end->raise();
        return;
    }
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        if (this->all_players[i]->getSize() >= BALL_MAX_SIZE)
        {
            this->__have_winner = true;
            const int move_to_up_scale = 43;
            this->_showTipScreen(QString("！第 %1 名！！").arg(this->all_players[_getMyIdFromAllplayers()]->getRank()));
            this->again_or_end->move(again_or_end->x(), again_or_end->y() - move_to_up_scale);
            again_or_end->setEnabled(false);
            this->__is_gameover = true;
            Zy::mSleep(4.5e3);
            again_or_end->setEnabled(true);
            again_or_end->setText("新游戏");
            again_or_end->move(again_or_end->x(), again_or_end->y() + move_to_up_scale);
        }
    }
}

void GamingWidget::_biggerDetermination()
{
    sortPlayersBySize(this->all_players, ALLPLAYERS_COUNTS);

    int count = ALLPLAYERS_COUNTS;
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        this->all_players[i]->raise();
        this->all_players[i]->setRank(count--);
    }
}

void GamingWidget::_blinkMe()
{
    if (!this->__is_just_reseted)
        return;
    if (this->__a_few_time % 2 == 0)
    {
        this->all_players[_getMyIdFromAllplayers()]->setColor("#c4ffbb");
    }
    else if (this->__a_few_time % 2 != 0)
    {
        this->all_players[_getMyIdFromAllplayers()]->setColor("#2dbc36");
    }
}

void GamingWidget::_speedDecline()
{
    double __decline_scale;
    for (int i = 0; i < ALLPLAYERS_COUNTS; i++)
    {
        __decline_scale = (all_players[i]->getSize() * all_players[i]->getSize()) / 131576;
        if (__decline_scale > 0)
        {
            if (all_players[i]->isMe())
            {
                all_players[i]->setSpeed(PLAYER_DEFAULT_SPEED - (__decline_scale));
            }
            else all_players[i]->setSpeed(OTHERS_AMEND_SPEED - (__decline_scale));
        }
    }
}

void GamingWidget::_playerMoving()
{
    //Mouse Position
    QPoint mouse = this->mapFromGlobal(QCursor::pos());
    int mx = mouse.x();
    int my = mouse.y();
    Ball* player = this->all_players[_getMyIdFromAllplayers()];
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
        if (all_players[i]->isMe())
            continue;
        int __i = i % OTHERPLAYERS;
        if (this->all_players[i]->isExist())
        {
            int curx = all_players[i]->getCircleX();
            int cury = all_players[i]->getCircleY();
            double speed = this->all_players[i]->getSpeed();
            if (__other_move_targetx[__i] > curx && __other_move_targety[__i] > cury)  //rightdown
            {
                curx += speed;
                cury += speed;
            }
            else if (__other_move_targetx[__i] > curx && __other_move_targety[__i] < cury)  //rightup
            {
                curx += speed;
                cury -= speed;
            }
            else if (__other_move_targetx[__i] < curx && __other_move_targety[__i] > cury)  //leftdown
            {
                curx -= speed;
                cury += speed;
            }
            else if (__other_move_targetx[__i] < curx && __other_move_targety[__i] < cury)  //leftup
            {
                curx -= speed;
                cury -= speed;
            }
            all_players[i]->updatePos(curx, cury);
        }
    }
}

void GamingWidget::_eatFood()
{
    for (int i = 0; i < FOOD_NUM; i++)
    {
        for (int j = 0; j < ALLPLAYERS_COUNTS; j++)
        {
            if (this->all_players[j]->isExist())
            {
                if (foods[i]->isExist())
                {
                    foods[i]->show();

                    //be eat
                    int cx = this->all_players[j]->getCircleX();
                    int cy = this->all_players[j]->getCircleY();
                    int fdcx = foods[i]->getCircleX();
                    int fdcy = foods[i]->getCircleY();
                    int d = sqrt(((cx - fdcx)*(cx - fdcx)) + ((cy - fdcy)*(cy - fdcy)));
                    if (d + (foods[i]->getSize() / 2) <= (this->all_players[j]->getSize() / 2))
                    {
                        foods[i]->setExist(false);
                        foods[i]->hide();
                        existing_food_num--;
                        if (this->all_players[j]->getSize() < BALL_MAX_SIZE)
                        {
                            this->all_players[j]->growUp(foods[i]->getSize());
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
            if (!(this->all_players[i]->isExist() && this->all_players[j]->isExist()))
                continue;
            if (this->all_players[i]->getSize() < this->all_players[j]->getSize())
            {
                int x1, x2, y1, y2;
                x1 = this->all_players[i]->getCircleX();
                x2 = this->all_players[j]->getCircleX();
                y1 = this->all_players[i]->getCircleY();
                y2 = this->all_players[j]->getCircleY();
                if (sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) + (this->all_players[i]->getSize() / 2) <= (this->all_players[j]->getSize() / 2))
                {
                    this->all_players[i]->setExist(false);
                    this->all_players[i]->hide();
                    if (this->all_players[j]->getSize() < BALL_MAX_SIZE)
                    {
                        this->all_players[j]->growUp(this->all_players[i]->getSize());
                    }
                }
            }
        }
    }
}

void GamingWidget::_createNewFood()
{
    if (existing_food_num < static_cast<unsigned int>((FOOD_NUM - static_cast<int>(this->__game_time / 6e2)) / 2))
    {
        for (int i = 0; i < static_cast<int>(FOOD_NUM - static_cast<int>(this->__game_time / 6e2)); i++)
        {
            if (!foods[i]->isExist())
            {
                delete foods[i];
                foods[i] = new Food(this);
                foods[i]->move(rand() % WIDTH, rand() % HEIGHT);
                foods[i]->lower();
                existing_food_num++;
            }
        }
    }
}

GamingWidget::~GamingWidget()
{

}
