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

    }

    virtual ~Entity() {};
};

class bullet : public Entity
{
public:
    bullet()
    {
        name = "bullet";
    }

    void  update()
    {
        //dx = cos(angle * DEGTORAD) * 6;
        //dy = sin(angle * DEGTORAD) * 6;
        // angle+=rand()%6-3;

        if (x < 0)
        {
            dx = -dx;
        }
        if (x > W)
        {
            dx = -dx;
        }
        if (y < 0)
        {
            dy = -dy;
        }
        if (y > H)
        {
            dy = -dy;
        }
        x += dx;
        y += dy;
        //if (x > W || x<0 || y>H || y < 0) life = 0; // out of bounds => die

    }

};


class player : public Entity
{
public:
    bool thrust, back;
    int buffer;	

    player()
    {
        name = "player";
        buffer = 0;
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
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }

        if (dx + x < W && dx + x > 0 && dy + y < H && dy + y > 0) 
        {
            x += dx;
            y += dy;
        }
        /*
        if (x > W) x = 0; if (x < 0) x = W;
        if (y > H) y = 0; if (y < 0) y = H;*/
    }

};

/*class buff : public Entity
    {
    public:
        buff()
        {
        }
        buff(int t)
        {
            type = t;

        }
    };*/

class buffer : public Entity
{
public:
    buffer()
    {
        name = "buffer";
    }
    buffer(int t)
    {
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
    sf::Clock bufferClock;
    sf::Time time;
    sf::Time time2;
    sf::Time bufferTime;
    sf::Music bgm, start, endSound;
    bgm.openFromFile("audio effect/bgm.ogg");
    start.openFromFile("audio effect/start.ogg");
    endSound.openFromFile("audio effect/end.ogg");
    int pastTime = 0;
    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setKeyRepeatEnabled(false);
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7, t11, bigtank, bigbuff;
    //Texture b1, b2, b3;
    t1.loadFromFile("images/asteroids/tank_11.png");
    t11.loadFromFile("images/asteroids/tank_yellow_11.png");
    t2.loadFromFile("images/asteroids/background_blue.png");
    t3.loadFromFile("images/asteroids/explosions/type_C.png");
    t4.loadFromFile("images/asteroids/rock.png");
    //t5.loadFromFile("images/asteroids/fire_red.png");
    t5.loadFromFile("images/asteroids/bullet_circle3.png");
    t6.loadFromFile("images/asteroids/rock_small.png");
    t7.loadFromFile("images/asteroids/explosions/type_B.png");
    bigtank.loadFromFile("images/asteroids/bigtank.png");
    bigbuff.loadFromFile("images/asteroids/smallTank2.png");

    t1.setSmooth(true);
    t2.setSmooth(true);
    Sprite background(t2);

    // Animation(Texture& t, int x, int y, int w, int h, int count, float Speed) //rotating speed of rock etc.
    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    //Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sBullet(t5, 0, 0, 22, 22, 1, 0);
    Animation sPlayer(t1, 0, 0, 59, 59, 1, 0);
    Animation sPlayer2(t11, 0, 0, 59, 59, 1, 0);
    Animation sPlayer_go(t1, 0, 0, 59, 59, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
    Animation sBigTank(bigtank, 0, 0, 117, 118, 1, 0);
    Animation sBigBuff(bigbuff, 0, 0, 42, 42, 1, 0);
    //Animation Buffer1(b1, 0, 0, 40, 40, 1, 0);
    //Animation Buffer2(b2, 0, 0, 40, 40, 1, 0);
    //Animation Buffer3(b3, 0, 0, 40, 40, 1, 0);

    std::list<Entity*> entities;

    /*for (int i = 0; i < 15; i++) // 15 asteroids
    {
        asteroid* a = new asteroid();
        // void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1)
        a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
        entities.push_back(a);
    }*/

    player* p = new player();
    p->settings(sPlayer, 200, 200, 0, 20);
    p->name = "player1";
    entities.push_back(p);

    player* p2 = new player();
    p2->settings(sPlayer2, 400, 400, 0, 20);
    p2->name = "player2";
    entities.push_back(p2);

    int bulletNum = 3;
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
    Gameover.setPosition(500, 270);
    Gameover.setString("GAME OVER!!!");

    sf::Text StartMenu;
    StartMenu.setFont(font);
    StartMenu.setFillColor(Color::White);
    StartMenu.setCharacterSize(50);
    StartMenu.setPosition(0, H / 2);
    StartMenu.setString("PRESS ENTER TO START");

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

    sf::RectangleShape rec1;
    rec1.setSize(sf::Vector2f(100, 100));
    rec1.setFillColor(sf::Color::White);
    rec1.setPosition(300, 300);
    /////main loop/////
    while (app.isOpen())
    {
        
        app.clear();
        app.draw(background);
        if (score1 >= 5 || score2 >= 5)
        {
            bgm.stop();
            endSound.play();
            cin >> test;
            Game = false;
        }
        if (atStartMenu)
        {
            
            app.draw(StartMenu);
            //continue;
        }
        bufferTime = bufferClock.getElapsedTime();
        if (bufferTime.asSeconds() > 5)
        {
            for (auto i = entities.begin(); i != entities.end();)
            {
                Entity* e = *i;
                if (e->name == "bigbuffer") { e->life = false; i = entities.erase(i); delete e; }
                else i++;
            }
            buffer* big = new buffer();
            big->name = "bigbuffer";
            int randx = rand() % W + 1;
            int randy = rand() % H + 1;
            big->settings(sBigBuff, randx, randy, 0, 0);
            entities.push_back(big);
            bufferClock.restart();
        }
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space)
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
                        entities.push_back(b);
                        //bulletNum--;
                        //press++;
                    }

                }
            if (event.key.code == Keyboard::Enter)
            {
                atStartMenu = false;
                start.stop();
                bgm.play();
                
            }
            if (event.key.code == Keyboard::J)
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
                    gunshotSound.play();
                    //cout << p2->angle << endl;
                    //cout << b2->angle << endl;
                    entities.push_back(b2);
                }

            }
            text1.setString(to_string(score1));
            text2.setString(to_string(score2));

            if (event.key.code == Keyboard::Escape)
            {
                score1 = 0;
                score2 = 0;
                endSound.stop();
                Game = true;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
        if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
        else p->thrust = false;
        if (Keyboard::isKeyPressed(Keyboard::Down)) p->back = true;
        else p->back = false;

        if (Keyboard::isKeyPressed(Keyboard::D)) p2->angle += 3;
        if (Keyboard::isKeyPressed(Keyboard::A))  p2->angle -= 3;
        if (Keyboard::isKeyPressed(Keyboard::W)) p2->thrust = true;
        else p2->thrust = false;
        if (Keyboard::isKeyPressed(Keyboard::S)) p2->back = true;
        else p2->back = false;

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

                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
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

                        p2->settings(sPlayer2, 100, 200, 0, 20);
                        p2->dx = 0; p2->dy = 0;
                    }
                if (a->name == "player1" && b->name == "bigbuffer")
                {
                    if (isCollide(a, b))
                    {
                        b->life = false;
                        a->anim = sBigTank;
                        a->R = 40;
                    }
                }
            }



        for (auto e : entities)
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;

        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;

            e->update();
            e->anim.update();
            if (e->life == false) { i = entities.erase(i); delete e; }
            else i++;

        if (FloatRect(p->x, p->y, 3, 3).intersects(rec1.getGlobalBounds()))
        {
            p->thrust = false;
        }

        }
        //cout << Game << endl;
        //////draw//////
        //app.clear();
        //app.draw(background);

        for (auto i : entities)
            i->draw(app);
        //cout << press << endl;
        app.draw(text1);
        app.draw(text2);
        app.draw(score);
        app.draw(rec1);
        if (!Game)
            app.draw(Gameover);

        app.display();
    }
    return 0;
}

void drawWall_T(sf::RectangleShape& rec1, sf::RectangleShape& rec2, int x, int y, string direction) {
    int length = 300, width = 20;
    int half = (length - width) / 2;
    sf::Color color = sf::Color::Black;
    if (direction == "up") {
        rec1.setSize(sf::Vector2f(length, width));
        rec1.setFillColor(color);
        rec1.setPosition(x, y);
        rec2.setSize(sf::Vector2f(width, half));
        rec2.setFillColor(color);
        rec2.setPosition(x + half, y - half);
    }
    else if (direction == "down") {
        rec1.setSize(sf::Vector2f(length, width));
        rec1.setFillColor(color);
        rec1.setPosition(x, y);
        rec2.setSize(sf::Vector2f(width, half));
        rec2.setFillColor(color);
        rec2.setPosition(x + half, y);
    }
    else if (direction == "left") {
        rec1.setSize(sf::Vector2f(width, length));
        rec1.setFillColor(color);
        rec1.setPosition(x, y);
        rec2.setSize(sf::Vector2f(half, width));
        rec2.setFillColor(color);
        rec2.setPosition(x - half, y + half);
    }
    else if (direction == "right") {
        rec1.setSize(sf::Vector2f(width, length));
        rec1.setFillColor(color);
        rec1.setPosition(x, y);
        rec2.setSize(sf::Vector2f(half, width));
        rec2.setFillColor(color);
        rec2.setPosition(x, y + half);
    }
}