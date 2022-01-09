#include <SFML/Graphics.hpp>
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
        dx = cos(angle * DEGTORAD) * 6;
        dy = sin(angle * DEGTORAD) * 6;
        // angle+=rand()%6-3;

        if (x < 0 || x > W)
        {
            dx = -dx;
        }
        if (y < 0 || y > H)
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

    player()
    {
        name = "player";
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

        x += dx;
        y += dy;

        if (x > W) x = 0; if (x < 0) x = W;
        if (y > H) y = 0; if (y < 0) y = H;
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
    int pastTime = 0;
    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setKeyRepeatEnabled(false);
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7, t11;
    t1.loadFromFile("images/asteroids/tank_11.png");
    t11.loadFromFile("images/asteroids/tank_yellow_11.png");
    t2.loadFromFile("images/asteroids/background_blue.png");
    t3.loadFromFile("images/asteroids/explosions/type_C.png");
    t4.loadFromFile("images/asteroids/rock.png");
    //t5.loadFromFile("images/asteroids/fire_red.png");
    t5.loadFromFile("images/asteroids/bullet_16.png");
    t6.loadFromFile("images/asteroids/rock_small.png");
    t7.loadFromFile("images/asteroids/explosions/type_B.png");

    t1.setSmooth(true);
    t2.setSmooth(true);
    Sprite background(t2);

    // Animation(Texture& t, int x, int y, int w, int h, int count, float Speed) //rotating speed of rock etc.
    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    //Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sBullet(t5, 0, 0, 25, 25, 16, 0);
    Animation sPlayer(t1, 0, 0, 59, 59, 1, 0);
    Animation sPlayer2(t11, 0, 0, 59, 59, 1, 0);
    Animation sPlayer_go(t1, 0, 0, 59, 59, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);


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

    bool cooldown = 0;
    bool Game = true;
    /////main loop/////
    while (app.isOpen())
    {
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::N)
                {
                    cooldown = 1;

                    if (bulletNum > 0)
                    {
                        bullet* b = new bullet();
                        b->name = "bullet1";
                        b->settings(sBullet, p->x, p->y, p->angle, 10);
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
                    b2->settings(sBullet, p2->x, p2->y, p2->angle, 10);
                    entities.push_back(b2);
                }

            }
            if (event.key.code == Keyboard::B)
            {
                cout << p2->angle;
            }
            text1.setString(to_string(score1));
            text2.setString(to_string(score2));

            if (event.key.code == Keyboard::Escape)
            {
                score1 = 0;
                score2 = 0;
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
                        entities.push_back(e);

                        p2->settings(sPlayer2, 100, 200, 0, 20);
                        p2->dx = 0; p2->dy = 0;
                    }
            }

        if (score1 >= 5 || score2 >= 5)
        {
            Game = false;
        }

        for (auto e : entities)
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;

        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;

            e->update();
            e->anim.update();
            if (e->name == "bullet1")
            {
                cout << "dx: " << e->dx << endl;
                cout << "dy: " << e->dy << endl;
            }
            if (e->life == false) { i = entities.erase(i); delete e; }
            else i++;
        }

        //////draw//////
        app.clear();
        app.draw(background);

        for (auto i : entities)
            i->draw(app);
        //cout << press << endl;
        app.draw(text1);
        app.draw(text2);
        app.draw(score);
        if (!Game)
            app.draw(Gameover);
        app.display();
    }

    return 0;
}