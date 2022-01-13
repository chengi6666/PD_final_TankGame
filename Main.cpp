#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include <time.h>
#include <list>
#include <iostream>
#include <sstream>
#include <string>
using namespace sf;
using namespace std;

const int W = 971;  // 1200
const int H = 545;  // 800

float DEGTORAD = 0.017453f;

void drawWall(sf::RectangleShape&, sf::RectangleShape&, int, int, string);

class Animation
{
public:
    float Frame = 0, speed = 0;
    Sprite sprite;
    std::vector<IntRect> frames;

    Animation() {}

    Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
    {
        Frame = 0;
        speed = Speed;

        for (int i = 0; i < count; i++)
            frames.push_back(IntRect(x + i * w, y, w, h));

        sprite.setTexture(t);
        sprite.setOrigin(w / 2, h / 2);
        sprite.setTextureRect(frames[0]);
    }


    void update()
    {
        Frame += speed;
        int n = frames.size();
        if (Frame >= n) Frame -= n;
        if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
    }

    bool isEnd()
    {
        return Frame + speed >= frames.size();
    }

};


class Entity
{
public:
    float x = 0, y = 0, dx = 0, dy = 0, R = 0, angle = 0;
    bool life;
    int type;
    //int xFactor;
    //int yFactor;
    std::string name;
    Animation anim;

    Entity()
    {
        life = 1;
    }

    void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1)
    {
        anim = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
    }

    /*void setSize(int x, int y)
    {
        xFactor = x;
        yFactor = y;
    }*/

    virtual void update() {};

    void draw(RenderWindow& app)
    {
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90);
        app.draw(anim.sprite);
        //CircleShape circle(R);
        //circle.setFillColor(Color(255, 0, 0, 170));
        //circle.setPosition(x, y);
        //circle.setOrigin(R, R);
        //app.draw(circle);

    }

    virtual ~Entity() {};
};

class bullet : public Entity
{
public:
    int collision = 0;
    int speedFactor;
    bullet()
    {
        name = "bullet";
        speedFactor = 1;
    }

    void  update()
    {
        //dx = cos(angle * DEGTORAD) * 6;
        //dy = sin(angle * DEGTORAD) * 6;
        // angle+=rand()%6-3;

        if (x < 0 || x > W)
        {
            dx = -dx;
            angle += 180 - 2 * angle;
            collision++;
        }
        if (y < 0 || y > H)
        {
            dy = -dy;
            angle = 360 - angle;
            collision++;
        }
        float epsilon = 6;
        // First Wall
        if ((abs(x - 210) < epsilon || abs(x - 330) < epsilon) && (y >= 100 && y < 130))
        {
            dx = -dx;
            angle += 180 - 2 * angle;;
            collision++;
        }
        if ((abs(y - 100) < epsilon || abs(y - 130) < epsilon) && (x >= 210 && x < 330))
        {
            dy = -dy;
            angle = 360 - angle;
            collision++;
        }
        if ((abs(x - 330) < epsilon || abs(x - 360) < epsilon) && (y >= 100 && y < 250))
        {
            dx = -dx;
            angle += 180 - 2 * angle;
            collision++;
        }
        if ((abs(y - 100) < epsilon || abs(y - 250) < epsilon) && (x >= 330 && x < 360))
        {
            dy = -dy;
            angle = 360 - angle;
            collision++;
        }

        // Second Wall
        if ((abs(x - 400) < epsilon || abs(x - 670) < epsilon) && (y >= 350 && y < 380))
        {
            dx = -dx;
            angle += 180 - 2 * angle;
            collision++;
        }

        if ((abs(y - 350) < epsilon || abs(y - 380) < epsilon) && (x >= 400 && x < 670))
        {
            dy = -dy;
            angle = 360 - angle;
            collision++;
        }
        if ((abs(x - 520) < epsilon || abs(x - 550) < epsilon) && (y >= 260 && y < 350))
        {
            dx = -dx;
            angle += 180 - 2 * angle;
            collision++;
        }
        if ((abs(y - 260) < epsilon || abs(y - 350) < epsilon) && (x >= 520 && x < 550))
        {
            dy = -dy;
            angle = 360 - angle;
            collision++;
        }
        // Third Wall
        if ((abs(x - 720) < epsilon || abs(x - 840) < epsilon) && (y >= 80 && y < 110))
        {
            dx = -dx;
            angle += 180 - 2 * angle;
            collision++;
        }

        if ((abs(y - 80) < epsilon || abs(y - 110) < epsilon) && (x >= 720 && x < 840))
        {
            dy = -dy;
            angle = 360 - angle;
            collision++;
        }
        if ((abs(x - 720) < epsilon || abs(x - 750) < epsilon) && (y >= 110 && y < 200))
        {
            dx = -dx;
            angle += 180 - 2 * angle;
            collision++;
        }
        if ((abs(y - 110) < epsilon || abs(y - 200) < epsilon) && (x >= 720 && x < 750))
        {
            dy = -dy;
            angle = 360 - angle;
            collision++;
        }
        x += dx * speedFactor;
        y += dy * speedFactor;

        if (collision >= 5)
        {
            life = 0;
        }

    }

};


class player : public Entity
{
public:
    bool thrust, back, stop = false;
    int buffer;
    float speed = 0;
    player()
    {
        name = "player";
        buffer = 0;
    }
    void changeBuffer(int T, int who) {
        type = T;
        buffer = T;
    }

    void update()
    {
        if (thrust)
        {
            dx += cos(angle * DEGTORAD) * 0.2;
            dy += sin(angle * DEGTORAD) * 0.2;
        }
        else
        {
            dx *= 0.99;
            dy *= 0.99;
        }
        if (back)
        {
            dx -= cos(angle * DEGTORAD) * 0.2;
            dy -= sin(angle * DEGTORAD) * 0.2;
        }
        else
        {
            dx *= 0.99;
            dy *= 0.99;
        }
        int maxSpeed = 15;
        speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }

        if (!(dx + x < W && dx + x > 0 && dy + y < H && dy + y > 0) ||
            (dx + x < 360 && dx + x > 210 && dy + y < 130 && dy + y > 100) ||
            (dx + x < 360 && dx + x > 330 && dy + y < 250 && dy + y > 100) ||
            (dx + x < 550 && dx + x > 520 && dy + y < 380 && dy + y > 260) ||
            (dx + x < 670 && dx + x > 400 && dy + y < 380 && dy + y > 350) ||
            (dx + x < 840 && dx + x > 720 && dy + y < 110 && dy + y > 80) ||
            (dx + x < 750 && dx + x > 720 && dy + y < 200 && dy + y > 80))
        {
            stop = true;
        }
        else
        {
            stop = false;
        }
        /*if (dx + x < 647 && dx + x > 500 && dy + y < 250 && dy + y > 200)
        {
            stop = true;
        }*/
        if (!stop)
        {
            x += dx;
            y += dy;
        }
        /*
        if (x > W) x = 0; if (x < 0) x = W;
        if (y > H) y = 0; if (y < 0) y = H;*/
    }

};

class buff : public Entity {
public:


    buff()
    {
    }
    buff(int t) {
        type = t;

    }
};
bool isCollide(Entity* a, Entity* b)
{
    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        (a->R + b->R) * (a->R + b->R);
}


int main()
{
    srand(time(0));
    sf::Clock clock;
    sf::Clock clock2;
    sf::Clock clock3;
    sf::Time time;
    sf::Time time2;
    sf::Music bgm, start, endSound;
    bgm.openFromFile("audio effect/bgm.ogg");
    start.openFromFile("audio effect/start.ogg");
    endSound.openFromFile("audio effect/end.ogg");
    int pastTime = 0;
    RenderWindow app(VideoMode(W, H), "Tank Game!");
    app.setKeyRepeatEnabled(false);
    app.setFramerateLimit(60);

    auto image = sf::Image{};
    image.loadFromFile("images/tank/tank_green.png");

    app.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
    Texture t1, t2, t3, t4, t5, t6, t7, t11, bigtank, bigbuff, tControls, tBlock, tStartMenu, t8, st1, st2;
    Texture b1, b2, b3;
    t1.loadFromFile("images/tank/tank_green.png");
    t11.loadFromFile("images/tank/tank_yellow.png");
    t2.loadFromFile("images/tank/background_brown.png");
    t3.loadFromFile("images/tank/explosions/type_C.png");
    t5.loadFromFile("images/tank/bullet.png");
    //t5.loadFromFile("images/asteroids/bullet_circle3.png");
    t7.loadFromFile("images/tank/explosions/type_B.png");
    tControls.loadFromFile("images/new/controls2.png");
    tBlock.loadFromFile("images/tank/block3.png");
    tStartMenu.loadFromFile("images/tank/sky.png");

    t8.loadFromFile("images/tank/bullet_big.png");
    st1.loadFromFile("images/tank/tank_green_small.png");
    st2.loadFromFile("images/tank/tank_yellow_small.png");
    b1.loadFromFile("images/tank/bigBullet.png");
    b2.loadFromFile("images/tank/smallTank.png");
    b3.loadFromFile("images/tank/fastBullet.png");


    t1.setSmooth(true);
    t2.setSmooth(true);
    Sprite background(t2);
    Sprite startBackground(tStartMenu);

    // Animation(Texture& t, int x, int y, int w, int h, int count, float Speed) //rotating speed of rock etc.
    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sBullet(t5, 0, 0, 12, 35, 1, 0);
    //Animation sBullet(t5, 0, 0, 22, 22, 1, 0);
    Animation sPlayer(t1, 0, 0, 54, 56, 1, 0);
    Animation sPlayer2(t11, 0, 0, 53, 57, 1, 0);
    //Animation sPlayer_go(t1, 0, 0, 59, 59, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
    Animation sBigTank(bigtank, 0, 0, 117, 118, 1, 0);
    Animation sBigBuff(bigbuff, 0, 0, 42, 42, 1, 0);
    //Animation sBlock(tBlock, 0, 0, 49, 50, 1, 0);

    //Animation sBullet(t5, 0, 0, 25, 25, 16, 0);
    Animation sBigBullet(t8, 0, 0, 12, 65, 16, 0);
    Animation smallPlayer(st1, 0, 0, 30, 30, 1, 0);
    Animation smallPlayer2(st2, 0, 0, 30, 30, 1, 0);
    Animation Buffer1(b1, 0, 0, 40, 40, 1, 0);
    Animation Buffer2(b2, 0, 0, 40, 40, 1, 0);
    Animation Buffer3(b3, 0, 0, 40, 40, 1, 0);

    Sprite blocks[100];
    int n = 0;
    for (int i = 0; i < 5; i++)
    {
        blocks[n].setTexture(tBlock);
        blocks[n].setPosition(330, 100 + i * 30);
        n++;
    }
    for (int i = 1; i < 5; i++)
    {
        blocks[n].setTexture(tBlock);
        blocks[n].setPosition(180 + i * 30, 100);
        n++;
    }

    for (int i = 0; i < 9; i++)
    {
        blocks[n].setTexture(tBlock);
        blocks[n].setPosition(400 + i * 30, 350);
        n++;
    }
    for (int i = 1; i < 4; i++)
    {
        blocks[n].setTexture(tBlock);
        blocks[n].setPosition(520, 230 + i * 30);
        n++;
    }
    for (int i = 0; i < 4; i++)
    {
        blocks[n].setTexture(tBlock);
        blocks[n].setPosition(720 + i * 30, 80);
        n++;
    }
    for (int i = 1; i < 4; i++)
    {
        blocks[n].setTexture(tBlock);
        blocks[n].setPosition(720, 80 + i * 30);
        n++;
    }

    std::list<Entity*> entities;

    player* p = new player();
    p->settings(sPlayer, 200, 200, 0, 20);
    p->name = "player1";
    entities.push_back(p);

    player* p2 = new player();
    p2->settings(sPlayer2, 800, 150, 0, 20);
    p2->name = "player2";
    entities.push_back(p2);

    int score1 = 0, score2 = 0;
    sf::Font font;
    font.loadFromFile("content/ARCADECLASSIC.TTF");

    sf::Text text1;
    text1.setFont(font);
    text1.setFillColor(Color::Green);
    text1.setCharacterSize(100);
    text1.setPosition(200, 420);
    sf::Text text2;
    text2.setFont(font);
    text2.setFillColor(Color::Yellow);
    text2.setCharacterSize(100);
    text2.setPosition(700, 420);
    sf::Text score;
    score.setFont(font);
    score.setFillColor(Color::White);
    score.setCharacterSize(100);
    //score.setOrigin(W / 2, 420);
    score.setPosition(W / 2 - 150, 420);
    score.setString("SCORE");

    sf::Text Gameover;
    Gameover.setFont(font);
    Gameover.setFillColor(Color::White);
    Gameover.setCharacterSize(100);
    Gameover.setPosition(W / 2 - 250, H / 2 - 50);
    Gameover.setString("GAME OVER!!!");

    sf::Text StartMenu;
    StartMenu.setFont(font);
    StartMenu.setFillColor(Color::Black);
    StartMenu.setCharacterSize(50);
    StartMenu.setPosition(W / 2 - 250, H / 2 + 100);
    StartMenu.setString("PRESS  SPACE  TO  START");

    sf::Text Title;
    Title.setFont(font);
    Title.setFillColor(Color::Black);
    Title.setCharacterSize(200);
    Title.setPosition(0, H / 2 - 200);
    Title.setString("TANK\tCITY");

    bool atStartMenu = true;
    bool Game = true;
    //bgm.setPosition(0, 1, 10);
    bgm.setVolume(20);
    bgm.setLoop(true);
    start.play();
    start.setVolume(50);
    start.setLoop(true);
    endSound.setVolume(50);
    endSound.setLoop(true);

    sf::SoundBuffer gunshotBuffer, explosionBuffer;
    gunshotBuffer.loadFromFile("audio effect/gunshot.ogg");
    explosionBuffer.loadFromFile("audio effect/explosion.ogg");
    sf::Sound explosionSound, gunshotSound;
    explosionSound.setBuffer(explosionBuffer);
    //explosionSound.play();
    //explosionSound.setLoop(true);
    //explosionSound.setVolume(100);
    gunshotSound.setBuffer(gunshotBuffer);
    int test = 0;

    sf::RectangleShape controls;
    controls.setSize(sf::Vector2f(738, 440));
    controls.setPosition(0, 0);
    //controls.setFillColor(sf::Color::Blue);
    controls.setTexture(&tControls);
    bool seeControls = false;

    /////main loop/////
    while (app.isOpen())
    {
        sf::Time elapsedTime = clock3.getElapsedTime();
        if (elapsedTime.asSeconds() > 10) {
            for (auto i = entities.begin(); i != entities.end();)
            {
                Entity* e = *i;
                if (e->name == "buffer") { e->life = false; i = entities.erase(i); delete e; }
                else i++;
            }
            p->changeBuffer(0, 1);
            p2->changeBuffer(0, 2);

            //reset the tank size
            p->anim = sPlayer;
            p2->anim = sPlayer2;
            p->R = 20;
            p2->R = 20;

            buff* buffer = new buff();
            buffer->name = "buffer";
            int type = rand() % (3) + 1;
            int randX = rand() % W + 1;
            int randY = rand() % H + 1;
            switch (type) {
            case(1):
                buffer->settings(Buffer1, randX, randY, 0, 0);
                buffer->type = type;
                break;
            case(2):
                buffer->settings(Buffer2, randX, randY, 0, 0);
                buffer->type = type;
                break;
            case(3):
                buffer->settings(Buffer3, randX, randY, 0, 0);
                buffer->type = type;
                break;
            }
            entities.push_back(buffer);
            clock3.restart();
        }

        app.clear();

        if (atStartMenu)
        {
            app.draw(startBackground);
            app.draw(StartMenu);
            app.draw(Title);
            //continue;
        }
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (!atStartMenu)
            {
                if (event.key.code == Keyboard::Enter)
                {
                    time = clock.getElapsedTime();
                    if (time.asSeconds() > 2)
                    {
                        clock.restart();
                        bullet* b = new bullet();
                        b->name = "bullet1";
                        b->settings(sBullet, p->x, p->y, p->angle, 10);
                        b->dx = cos(b->angle * DEGTORAD) * 6;
                        b->dy = sin(b->angle * DEGTORAD) * 6;
                        gunshotSound.play();
                        if (p->type == 0 || p->type == 2) {
                            b->settings(sBullet, p->x, p->y, p->angle, 10);
                            //b->setSize(1, 1);
                        }
                        else if (p->type == 1) {
                            b->settings(sBigBullet, p->x, p->y, p->angle, 20);
                            b->R = 40;
                            //b->setSize(2, 2);
                        }
                        else if (p->type == 3) {
                            b->settings(sBullet, p->x, p->y, p->angle, 10);
                            b->speedFactor = 3;
                        }

                        entities.push_back(b);
                        //bulletNum--;
                        //press++;
                    }

                }

                if (event.key.code == Keyboard::Z)
                {
                    time2 = clock2.getElapsedTime();
                    if (time2.asSeconds() > 2)
                    {
                        clock2.restart();
                        bullet* b2 = new bullet();
                        b2->name = "bullet2";
                        b2->settings(sBullet, p2->x, p2->y, p2->angle, 10);
                        b2->dx = cos(b2->angle * DEGTORAD) * 6;
                        b2->dy = sin(b2->angle * DEGTORAD) * 6;
                        if (p2->type == 0 || p2->type == 2) {
                            b2->settings(sBullet, p2->x, p2->y, p2->angle, 10);
                            //b2->setSize(1, 1);
                        }
                        //make bullet bigger
                        else if (p2->type == 1) {
                            b2->settings(sBigBullet, p2->x, p2->y, p2->angle, 20);
                            // b2->setSize(2, 2);
                            b2->R = 40;
                        }
                        else if (p2->type == 3) {
                            b2->settings(sBullet, p2->x, p2->y, p2->angle, 10);
                            b2->speedFactor = 3;
                        }

                        gunshotSound.play();
                        entities.push_back(b2);
                    }

                }

                if (event.key.code == Keyboard::Escape)
                {
                    score1 = 0;
                    score2 = 0;
                    for (auto e : entities)
                    {
                        if (e->name == "bullet1" || e->name == "bullet2")
                        {
                            e->life = 0;
                        }
                    }
                    endSound.stop();
                    bgm.play();
                    Game = true;
                }
            }
            else
            {
                if (event.key.code == Keyboard::Space)
                {
                    atStartMenu = false;
                    start.stop();
                    bgm.play();
                }
                if (!seeControls)
                {
                    if (event.key.code == Keyboard::P)
                    {
                        seeControls = true;
                    }
                }
                else
                {
                    if (event.key.code == Keyboard::O)
                    {
                        seeControls = false;
                    }
                }
            }
        }
        if (!atStartMenu)
        {
            if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
            if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
            if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
            else p->thrust = false;
            if (Keyboard::isKeyPressed(Keyboard::Down)) p->back = true;
            else p->back = false;

            if (Keyboard::isKeyPressed(Keyboard::G)) p2->angle += 3;
            if (Keyboard::isKeyPressed(Keyboard::D))  p2->angle -= 3;
            if (Keyboard::isKeyPressed(Keyboard::R)) p2->thrust = true;
            else p2->thrust = false;
            if (Keyboard::isKeyPressed(Keyboard::F)) p2->back = true;
            else p2->back = false;
        }
        if (!Game)
            continue;

        for (auto a : entities)
            for (auto b : entities)
            {
                if (a->name == "player1" && b->name == "bullet2")
                    if (isCollide(a, b))
                    {
                        b->life = false;
                        score2++;
                        Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        explosionSound.play();
                        entities.push_back(e);
                        for (auto e : entities)
                        {
                            if (e->name == "bullet1" || e->name == "bullet2")
                            {
                                e->life = 0;
                            }
                        }
                        p->settings(sPlayer, 200, 200, 0, 20);
                        p->dx = 0; p->dy = 0;
                    }
                if (a->name == "player2" && b->name == "bullet1")
                    if (isCollide(a, b))
                    {
                        b->life = false;
                        score1++;
                        Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        explosionSound.play();
                        entities.push_back(e);
                        for (auto e : entities)
                        {
                            if (e->name == "bullet1" || e->name == "bullet2")
                            {
                                e->life = 0;
                            }
                        }
                        p2->settings(sPlayer2, 800, 150, 0, 20);
                        p2->dx = 0; p2->dy = 0;
                    }
                if (a->name == "player1" && b->name == "block")
                {
                    if (isCollide(a, b))
                    {
                        p->stop = true;
                    }
                }
                if (a->name == "player1" && b->name == "buffer")
                    if (isCollide(a, b))
                    {
                        b->life = false;

                        /*Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);
                        score2++;
                        */
                        /*
                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
                        p->dx = 0; p->dy = 0;
                        */
                        p->changeBuffer(b->type, 1);
                        if (b->type == 2) {
                            p->R = 10;
                            p->anim = smallPlayer;
                            p->draw(app);
                        }
                    }
                if (a->name == "player2" && b->name == "buffer")
                    if (isCollide(a, b))
                    {
                        b->life = false;

                        /*Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);
                        score2++;
                        */
                        /*
                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
                        p->dx = 0; p->dy = 0;
                        */
                        p2->changeBuffer(b->type, 2);
                        if (b->type == 2) {
                            p2->R = 10;
                            p2->anim = smallPlayer2;
                            p2->draw(app);
                        }

                    }
            }






        for (auto e : entities)
        {
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;
        }
        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;

            e->update();
            e->anim.update();
            if (e->life == false) { i = entities.erase(i); delete e; }
            else i++;


        }

        text1.setString(to_string(score1));
        text2.setString(to_string(score2));
        //////draw//////
        if (!atStartMenu)
        {
            app.draw(background);
            for (auto i : entities)
                i->draw(app);
            app.draw(text1);
            app.draw(text2);
            app.draw(score);
            for (int i = 0; i < n; i++)
            {
                app.draw(blocks[i]);
            }
            if (score1 >= 5 || score2 >= 5)
            {
                bgm.stop();
                endSound.play();
                for (auto e : entities)
                {
                    if (e->name == "bullet1" || e->name == "bullet2")
                    {
                        e->life = 0;
                    }
                }
                Game = false;
            }
            if (!Game)
                app.draw(Gameover);

        }
        else if (seeControls)
        {
            app.draw(controls);
        }
        //app.draw(controls);
        app.display();
    }
    return 0;
}

