#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>  
#include <chrono>

using namespace sf;
using namespace std;

const int W = 971;  // 1200
const int H = 545;  // 800

float DEGTORAD = 0.017453f;

class Animation
{
public:
    float Frame, speed;
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
    float x, y, dx, dy, R, angle;
    bool life;
    //for buffer
    int type;
    std::string name;
    Animation anim;
    //Entity* fromwho;
    int xFactor;
    int yFactor;
    Entity()
    {
        life = 1;
        type = 0;
    }

    void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1)
    {
        anim = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
    }

    void setSize(int x, int y) {
        xFactor = x;
        yFactor = y;
    }


    virtual void update() {};

    void draw(RenderWindow& app)
    {
       
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90);
        //anim.sprite.setScale(x,y);
        app.draw(anim.sprite);
     
        CircleShape circle(R);
        circle.setFillColor(Color(255, 0, 0, 170));  // red, why 4 parameters?
        circle.setPosition(x, y);
        circle.setOrigin(R, R);
        //circle.setRadius(R);
        //app.draw(circle);
    }

    virtual ~Entity() {};
};


class asteroid : public Entity
{
public:
    asteroid()
    {
        dx = rand() % 8 - 4;
        dy = rand() % 8 - 4;
        name = "asteroid";
    }

    void  update()
    {
        x += dx;
        y += dy;

        if (x > W) x = 0;  if (x < 0) x = W;    // out of bounds => to the origin
        if (y > H) y = 0;  if (y < 0) y = H;
    }

};



class bullet : public Entity
{
public:
    Entity* fromwho;

    bullet()
    {
        name = "bullet";
    }

    void  update()
    {
        dx = cos(angle * DEGTORAD) * 6;
        dy = sin(angle * DEGTORAD) * 6;
        // angle+=rand()%6-3;
        x += dx;
        y += dy;

        if (x > W || x<0 || y>H || y < 0) life = 0; // out of bounds => die
    }

};


class player : public Entity
{
public:
    bool thrust , back;
    int buffer;
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
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }


        if (dx + x < W && dx + x > 0 && dy + y < H && dy + y > 0) {
            x += dx;
            y += dy;
        }
        /*
        if (x > W) x = 0; if (x < 0) x = W;
        if (y > H) y = 0; if (y < 0) y = H;
        */
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
    sf::Time time;
    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7, t11,t8;
    Texture b1, b2, b3;
    t1.loadFromFile("images/asteroids/tank_11.png");
    t11.loadFromFile("images/asteroids/tank_yellow_11.png");
    t2.loadFromFile("images/asteroids/background_blue.png");
    t3.loadFromFile("images/asteroids/explosions/type_C.png");
    t4.loadFromFile("images/asteroids/rock.png");
    //t5.loadFromFile("images/asteroids/fire_red.png");
    t5.loadFromFile("images/asteroids/bullet_16.png");
    t8.loadFromFile("images/asteroids/big.png");
    t6.loadFromFile("images/asteroids/rock_small.png");
    t7.loadFromFile("images/asteroids/explosions/type_B.png");

    b1.loadFromFile("images/bigBullet.png");
    b2.loadFromFile("images/breakWall.png");
    b3.loadFromFile("images/smallTank.png");
    
    t1.setSmooth(true);
    t2.setSmooth(true);
    Sprite background(t2);

    // Animation(Texture& t, int x, int y, int w, int h, int count, float Speed) //rotating speed of rock etc.
    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    //Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sBullet(t5, 0, 0, 25, 25, 16, 0);
    Animation sBigBullet(t8, 0, 0, 50, 50, 16, 0);
    Animation sPlayer(t1, 0, 0, 59, 59, 1, 0);
    Animation sPlayer2(t11, 0, 0, 59, 59, 1, 0);
    Animation sPlayer_go(t1, 0, 0, 59, 59, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);

    Animation Buffer1(b1, 0, 0, 40, 40, 1, 0);
    Animation Buffer2(b2, 0, 0, 40, 40, 1, 0);
    Animation Buffer3(b3, 0, 0, 40, 40, 1, 0);

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
    int press = 0;
    int score1 = 0, score2 = 0;
    sf::Font font;
    font.loadFromFile("content/ARCADECLASSIC.TTF");

    sf::Font font2;
    font2.loadFromFile("content/ARCADECLASSIC.TTF");
    
    sf::Text text1;
    text1.setFont(font);

    sf::Text text2;
    text2.setFont(font2);
    text2.setString("SCORE");
    text2.setFillColor(Color::White);
    text2.setCharacterSize(100);
    text2.setPosition(700, 200);

    /////main loop/////
    while (app.isOpen())
    {
        sf::Time elapsedTime = clock.getElapsedTime();
        if (elapsedTime.asSeconds() > 10) {
            for (auto i = entities.begin(); i != entities.end();)
            {
                Entity* e = *i;
                if (e->name == "buffer") { e->life = false; i = entities.erase(i); delete e; }
                else i++;
            }
            p->changeBuffer(0,1);
            p2->changeBuffer(0, 2);

            
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
            clock.restart();
        }
        
        Event event;
        /*float thisShot = 0, lastShot = 0;
        if (bulletNum < 0)
        {
            //cout <<
            sf::Time coolTime = clock.getElapsedTime();
            if (coolTime.asSeconds() > 2)
                bulletNum = 3;
        }*/
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Enter)
                {
                    if (bulletNum > 0)
                    {
                        bullet* b = new bullet();
                        b->name = "bullet1";
                        if (p->type == 0) {
                            b->settings(sBullet, p->x, p->y, p->angle, 10);
                            b->setSize(1, 1);
                        }
                        else {
                            b->settings(sBigBullet, p->x, p->y, p->angle, 20);
                            b->R = 40;
                            b->setSize(2, 2);
                        }
                        entities.push_back(b);
                        //bulletNum--;
                        //press++;
                    }
                }
                if (event.key.code == Keyboard::J)
                {
                    if (bulletNum > 0)
                    {
                        bullet* b2 = new bullet();
                        b2->name = "bullet2";
                        if (p2->type == 0) {
                            b2->settings(sBullet, p2->x, p2->y, p2->angle, 10);
                            b2->setSize(1, 1);
                        }
                        else {
                            b2->settings(sBigBullet, p2->x, p2->y, p2->angle, 20);
                            b2->setSize(2, 2);
                            b2->R = 40;
                        }
                        entities.push_back(b2);
                        //bulletNum--;
                        //press++;
                    }

                }
            stringstream ss;
            ss << score1;
            text1.setString(ss.str().c_str());
            text1.setFillColor(Color(255, 0, 0, 170));
            text1.setCharacterSize(100);
            text1.setPosition(200, 420);

            /*stringstream ss2;
            ss2 << score2;
            text2.setString(ss2.str().c_str());
            text2.setFillColor(Color::White);
            text2.setCharacterSize(100);
            text2.setPosition(700, 300);*/
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


        for (auto a : entities)
            for (auto b : entities)
            {
                if (a->name == "asteroid" && b->name == "bullet")
                    if (isCollide(a, b))
                    {
                        a->life = false;
                        b->life = false;

                        Entity* e = new Entity();
                        e->settings(sExplosion, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);


                        for (int i = 0; i < 2; i++) // small asteroid
                        {
                            if (a->R == 15) continue;
                            Entity* e = new asteroid();
                            e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
                            entities.push_back(e);
                        }
                        press++;
                    }

                if (a->name == "player" && b->name == "asteroid")
                    if (isCollide(a, b))
                    {
                        b->life = false;
                        Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);

                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
                        p->dx = 0; p->dy = 0;
                    }
                if (a->name == "player1" && b->name == "bullet2")
                    if (isCollide(a, b))
                    {
                        b->life = false;

                        Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);
                        score2++;
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
                        entities.push_back(e);

                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
                        p->dx = 0; p->dy = 0;
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
                        p2-> type = b->type
                        p2->changeBuffer(b->type, 2);
                    }
            }


        if (p->thrust)  p->anim = sPlayer_go;
        else   p->anim = sPlayer;


        /*if (p2->thrust)  p2->anim = sPlayer_go;
        else   p2->anim = sPlayer;*/


        for (auto e : entities)
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;

        /*if (rand() % 150 == 0)
        {
            asteroid* a = new asteroid();
            a->settings(sRock, 0, rand() % H, rand() % 360, 25);
            entities.push_back(a);
        }*/

        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;

            e->update();
            e->anim.update();

            if (e->life == false) { i = entities.erase(i); delete e; }
            else i++;
        }


        //sf::Time time = clock.getElapsedTime();
        //cout << time.asSeconds() << endl;
        /*if (time.asSeconds() > 10)
            app.close();*/

            //////draw//////
        app.clear();
        app.draw(background);

        for (auto i : entities)
            i->draw(app);
        //cout << press << endl;
        app.draw(text1);
        app.draw(text2);
        app.display();
    }

    return 0;
}
