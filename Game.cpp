#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include <fstream>
#include <random>
#include <iostream>

using namespace sf;
using namespace std;

//-------------------------------------------GLOBAL VARIABLES-----------------------------------------------------
static Music levelAudio;
static Music startAudio;
static Music endAudio;
static SoundBuffer damageBuffer;
static Sound damageAudio;

bool reset = false;
int yourscore = 0;
int highscore = 0;
int score = 0;
int level = 1;
int lives = 3;
int preset = 100;
float timefreq = 0.1f;
float monsterspeed = 0.06f;
float slimespeed = 0.04f;
float flyspeed = 0.08f;
float speed = 0.1f;
float height = 100.0f;
//-------------------------------------------------NEW HIGH SCORE-------------------------------------------------
void newHighScore(RenderWindow& window)
{
    Font font;
    Color yellow = Color::Yellow;
    Color red = Color::Red;
    font.loadFromFile("strikefighter.ttf");

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    Sprite background(backgroundTexture);

    Text congrats("CONGRATULATIONS!", font, 40);
    congrats.setPosition(155, 80);
    congrats.setFillColor(yellow);

    Text record("New High Score: 0", font, 40);
    record.setPosition(130, 120);
    record.setFillColor(yellow);
    record.setString("New High Score: " + to_string(highscore));

    Text prompt("Press Enter to Continue! ", font, 30);
    prompt.setPosition(135, 300);
    prompt.setFillColor(red);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
            {
                return;
            }
        }
        window.clear();
        window.draw(background);
        window.draw(prompt);
        window.draw(record);
        window.draw(congrats);
        window.display();
    }
}
//----------------------------------------------UPDATE HIGH SCORE-------------------------------------------------
void updateHighScore()
{
    ifstream highscoreFile("highscore.txt");
    highscoreFile.is_open();
    highscoreFile >> highscore;
    highscoreFile.close();
    if (yourscore > highscore)
    {
        ofstream highscoreFile("highscore.txt");
        highscoreFile.is_open();
        highscoreFile << yourscore;
        highscore = yourscore;
        highscoreFile.close();
    }
}
//----------------------------------------------RESET GAME FUNCTION-----------------------------------------------
void displayHighScore(RenderWindow& window)
{
    updateHighScore();
    if (yourscore == highscore)
    {
        levelAudio.play();
        newHighScore(window);
    }
    Font font;
    Color red = Color::Red;
    Color blue = Color::Blue;
    Color purple = Color::Yellow;
    font.loadFromFile("strikefighter.ttf");

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    Sprite background(backgroundTexture);

    Text title("Rankings", font, 40);
    title.setPosition(228, 100);
    title.setFillColor(red);

    Text yourScore("--> Your Score: 0", font, 20);
    yourScore.setPosition(195, 200);
    yourScore.setFillColor(purple);
    yourScore.setString("--> Your Score: " + to_string(yourscore));

    Text highScore("--> High Score: 0", font, 20);
    highScore.setPosition(195, 225);
    highScore.setFillColor(purple);
    highScore.setString("--> High Score: " + to_string(highscore));

    Text play("--> Press 'Enter' to Restart", font, 20);
    play.setPosition(195, 300);
    play.setFillColor(blue);

    Text quit("--> Press 'Escape' to Quit", font, 20);
    quit.setPosition(195, 325);
    quit.setFillColor(blue);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
            {
                reset = true;
                return;
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }
        }
        window.clear();
        window.draw(background);
        window.draw(title);
        window.draw(yourScore);
        window.draw(highScore);
        window.draw(play);
        window.draw(quit);
        window.display();
    }
}
//----------------------------------------------RESET GAME FUNCTION-----------------------------------------------
void resetGame()
{
    yourscore = score;
    score = 0;
    level = 1;
    lives = 3;
    preset = 100;
    timefreq = 0.1f;
    monsterspeed = 0.06f;
    slimespeed = 0.04f;
    flyspeed = 0.08f;
    speed = 0.1f;
    height = 100.0f;
}
//----------------------------------------------GAME OVER FUNCTION------------------------------------------------
void gameOver(RenderWindow& window)
{
    Font font;
    Color red = Color::Red;
    Color blue = Color::Blue;
    font.loadFromFile("strikefighter.ttf");

    Text text("GAME OVER!", font, 70);
    text.setPosition(140, 200);
    text.setFillColor(red);

    Text prompt("Press 'Enter' to Contine", font, 30);
    prompt.setPosition(150, 300);
    prompt.setFillColor(blue);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
            {
                resetGame();
                displayHighScore(window);
                return;
            }
        }
        window.draw(text);
        window.draw(prompt);
        window.display();
    }
}
//-------------------------------------------------LIVES FUNCTION-------------------------------------------------
void playerLives(Text& livesText, RenderWindow& window)
{
    static Clock cooldownClock;
    static bool cooldownActive = false;
    static float cooldownDuration = 3.0f;
    if (!cooldownActive)
    {
        lives--;
        damageAudio.play();
        livesText.setString("Lives: " + to_string(lives));
        if (lives <= 0)
        {
            cooldownActive = true;
            endAudio.play();
            gameOver(window);
            return;
        }
        cooldownActive = true;
        cooldownClock.restart();
    }
    if (cooldownActive && cooldownClock.getElapsedTime().asSeconds() >= cooldownDuration)
    {
        cooldownActive = false;
    }
}
//--------------------------------------------INSTRUCTIONS MENU FUNCTION------------------------------------------
void instructionMenu(RenderWindow& window)
{
    Font font;
    Color red = Color::Red;
    font.loadFromFile("strikefighter.ttf");

    Texture InstructionTexture;
    InstructionTexture.loadFromFile("instructions.png");
    Sprite background(InstructionTexture);

    Text prompt("Press 'Enter' to Continue", font, 40);
    prompt.setPosition(36, 5);
    prompt.setFillColor(red);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
            {
                return;
            }
        }
        window.clear();
        window.draw(background);
        window.draw(prompt);
        window.display();
    }
}
//----------------------------------------------------LEVEL UP FUNCTIONS------------------------------------------
void levelUp(Text& levelText)
{
    if (score > preset)
    {
        speed = speed * 1.1;
        monsterspeed = monsterspeed * 1.1;
        flyspeed = flyspeed * 1.1;
        slimespeed = slimespeed * 1.1;
        level++;
        levelText.setString("Level: " + to_string(level));
        levelAudio.play();
        preset = preset + 100;
    }
}
//------------------------------------------------COLLISION CHECK FUNCTION----------------------------------------
bool collision(Sprite& dino, Sprite& ogre, Sprite& slime, Sprite& fly) {
    float minus = 0.7f;
    float minuss = 0.55f;
    FloatRect dinoo = dino.getGlobalBounds();
    FloatRect ogree = ogre.getGlobalBounds();
    FloatRect slimee = slime.getGlobalBounds();
    FloatRect flyy = fly.getGlobalBounds();

    dinoo.width = dinoo.width * minus;
    dinoo.height = dinoo.height * minus;
    ogree.width = ogree.width * minus;
    ogree.height = ogree.height * minuss;
    slimee.width = slimee.width * minus;
    slimee.height = slimee.height * minus;
    flyy.width = flyy.width * minus;
    flyy.height = flyy.height * minus;

    if (dinoo.intersects(ogree) || dinoo.intersects(flyy) || dinoo.intersects(slimee))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------SCORE UPDATER------------------------------------------------------------
void updateScore(RenderWindow& window, Text& scoreText, Clock& clock, Font& font)
{
    static float totaltime = 0.0f;
    float time = clock.restart().asSeconds();
    totaltime = totaltime + time;
    if (totaltime >= timefreq)
    {
        totaltime = totaltime - timefreq;
        score++;
        scoreText.setString("Score: " + to_string(score));
    }
    window.draw(scoreText);
}
//--------------------------------------------------START MENU FUNCTION-------------------------------------------
void startMenu(RenderWindow& window)
{
    Font font;
    Color red = Color::Red;
    Color blue = Color::Blue;
    Color yellow = Color::Yellow;
    updateHighScore();
    font.loadFromFile("strikefighter.ttf");

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    Sprite background(backgroundTexture);

    Text title("Game Menu", font, 40);
    title.setPosition(215, 100);
    title.setFillColor(red);

    Text record("High Score: 0", font, 40);
    record.setPosition(190, 150);
    record.setFillColor(yellow);
    record.setString("High Score: " + to_string(highscore));

    Text play("--> Press 'Enter' to Start", font, 20);
    play.setPosition(195, 300);
    play.setFillColor(blue);

    Text quit("--> Press 'Escape' to Quit", font, 20);
    quit.setPosition(195, 325);
    quit.setFillColor(blue);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
            {
                instructionMenu(window);
                startAudio.play();
                return;
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }
        }
        window.clear();
        window.draw(background);
        window.draw(title);
        window.draw(record);
        window.draw(play);
        window.draw(quit);
        window.display();
    }
}
//-----------------------------------------------PAUSE MENU FUNCTION----------------------------------------------
void pauseMenu(RenderWindow& window)
{
    Font font;
    Color red = Color::Red;
    Color blue = Color::Blue;
    font.loadFromFile("strikefighter.ttf");

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    Sprite background(backgroundTexture);

    Text exit("Press 'Escape' to Quit", font, 30);
    exit.setPosition(150, 100);
    exit.setFillColor(red);

    Text resume("Press 'Enter' to Resume", font, 30);
    resume.setPosition(130, 300);
    resume.setFillColor(red);

    Text warning("All Progress Will be Lost!", font, 20);
    warning.setPosition(180, 170);
    warning.setFillColor(blue);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
            {
                return;
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }
        }
        window.clear();
        window.draw(background);
        window.draw(resume);
        window.draw(exit);
        window.draw(warning);
        window.display();
    }
}
//-----------------------------------------------WASD MOVEMENT FUNCTIONS------------------------------------------
void upwards(sf::Sprite& sprite, bool& up, float speed)
{

    if (Keyboard::isKeyPressed(Keyboard::W) && !up)
    {
        up = true;
    }
    if (up) {
        float move = speed;
        sprite.move(0.0f, -move);
        if (sprite.getPosition().y < 215)
        {
            sprite.setPosition(sprite.getPosition().x, 215);
            up = false;
        }
    }
}
void downwards(Sprite& sprite, bool& down, float speed)
{
    if (Keyboard::isKeyPressed(Keyboard::S) && !down)
    {
        down = true;
    }
    if (down)
    {
        float move = speed;
        sprite.move(0.0f, move);
        if (sprite.getPosition().y > 440)
        {
            sprite.setPosition(sprite.getPosition().x, 440);
            down = false;
        }
    }
}
void right(Sprite& sprite, bool& right, float speed)
{
    if (Keyboard::isKeyPressed(Keyboard::D) && !right)
    {
        right = true;
    }
    if (right)
    {
        float move = speed;
        sprite.move(move, 0.0f);
        if (sprite.getPosition().x > 720)
        {
            sprite.setPosition(720, sprite.getPosition().y);
            right = false;
        }
    }
}
void left(Sprite& sprite, bool& left, float speed)
{
    if (Keyboard::isKeyPressed(Keyboard::A) && !left)
    {
        left = true;
    }
    if (left)
    {
        float move = speed;
        sprite.move(-move, 0.0f);
        if (sprite.getPosition().x < 25)
        {
            sprite.setPosition(25, sprite.getPosition().y);
            left = false;
        }
    }
}
//-------------------------------------------RANDOM NUMBER GENERATOR----------------------------------------------
void random(int& number) {

    random_device rd;
    mt19937 gen(rd());


    uniform_int_distribution<int> distribution(1, 6);
    int random = distribution(gen);
    number = random;
}
//---------------------------------------ENEMY MOVEMENT FUNCTIONS-------------------------------------------------
void enemove(Sprite& enemy, float speed, Clock clock, RenderWindow& window)
{
    int i = 5;
    Time monster = clock.restart();
    float deltaY = speed;
    enemy.move(-deltaY, 0.0f);
    if (enemy.getPosition().x < -10)
    {
        random(i);
        if (i == 1)
            enemy.setPosition(900, 230);
        else if (i == 2)
            enemy.setPosition(900, 300);
        else if (i == 3)
            enemy.setPosition(900, 330);
        else if (i == 4)
            enemy.setPosition(900, 370);
        else if (i == 5)
            enemy.setPosition(900, 410);
        else if (i == 6)
            enemy.setPosition(900, 410);
    }
}
void enemove1(Sprite& enemy, float speed, Clock clock, RenderWindow& window)
{
    int i = 5;
    Time monster = clock.restart();
    float deltaY = speed;
    enemy.move(-deltaY, 0.0f);
    if (enemy.getPosition().x < -150)
    {
        random(i);
        if (i == 1)
            enemy.setPosition(900, 250);
        else if (i == 2)
            enemy.setPosition(900, 340);
        else if (i == 3)
            enemy.setPosition(900, 350);
        else if (i == 4)
            enemy.setPosition(900, 370);
        else if (i == 5)
            enemy.setPosition(900, 450);
        else if (i == 6)
            enemy.setPosition(900, 450);
    }
}
void enemove2(Sprite& enemy, float speed, Clock clock, RenderWindow& window)
{
    int i = 5;
    Time monster = clock.restart();
    float deltaY = speed;
    enemy.move(-deltaY, 0.0f);
    if (enemy.getPosition().x < -90)
    {
        random(i);
        if (i == 1)
            enemy.setPosition(900, 235);
        else if (i == 2)
            enemy.setPosition(900, 310);
        else if (i == 3)
            enemy.setPosition(900, 335);
        else if (i == 4)
            enemy.setPosition(900, 370);
        else if (i == 5)
            enemy.setPosition(900, 425);
        else if (i == 6)
            enemy.setPosition(900, 425);
    }
}
//-------------------------------------------------END OF FUNCTIONS-----------------------------------------------

//--------------------------------------------------MAIN BODY-----------------------------------------------------
int main()
{
    //----------------------------------------------------WINDOW LOADER-----------------------------------------------
reset:
    RenderWindow window(VideoMode(750, 540), "Endless Runner");

    levelAudio.openFromFile("levelup.wav");
    startAudio.openFromFile("gamestart.wav");
    endAudio.openFromFile("gameover.wav");
    damageBuffer.loadFromFile("damage.wav");
    damageAudio.setBuffer(damageBuffer);

    startMenu(window);
    Event event;
    //-------------------------------------------------------TEXTURE LOADING------------------------------------------
    Texture texture;
    Texture enetex;
    Texture flytex;
    Texture slime;
    enetex.loadFromFile("enemy.png");
    texture.loadFromFile("playerrun.png");
    flytex.loadFromFile("fly.png");
    slime.loadFromFile("gs.png");

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    //----------------------------------------------------RECT SETTING------------------------------------------------
    IntRect dinorect(24, 0, 24, 24);
    IntRect upp(912, 0, 22, 24);
    IntRect downn(930, 0, 24, 24);
    IntRect hitt(956, 0, 24, 24);
    IntRect enerect(16, 0, 17, 16);
    IntRect flyrect(18, 0, 15, 18);
    IntRect slimerect(16, 0, 15, 16);
    //--------------------------------------------SPRITES LOADING-----------------------------------------------------
    Sprite dino(texture, dinorect);
    Sprite enemy(enetex, enerect);
    Sprite fly(flytex, flyrect);
    Sprite gs(slime, slimerect);
    Sprite background(backgroundTexture);
    dino.setScale(4.0f, 4.0f);
    enemy.setScale(4.7f, 4.7f);
    gs.setScale(4.0f, 4.0f);
    fly.setScale(3.8f, 3.8f);
    dino.setOrigin(dino.getLocalBounds().width / 2.0f, dino.getLocalBounds().height / 2.0f);
    enemy.setOrigin(enemy.getLocalBounds().width / 2.0f, enemy.getLocalBounds().height / 2.0f);
    gs.setOrigin(gs.getLocalBounds().width / 2.0f, gs.getLocalBounds().height / 2.0f);
    fly.setOrigin(fly.getLocalBounds().width / 2.0f, fly.getLocalBounds().height / 2.0f);
    bool up = false, down = false, rightt = false, leftt = false;
    int i = 3;
    //----------------------------------------SPAWNER-------------------------------------------------------------
    dino.setPosition(100, 285);
    random(i);
    if (i == 1)
        enemy.setPosition(800, -200);
    else if (i == 2)
        enemy.setPosition(800, 283);
    else if (i == 3)
        enemy.setPosition(800, -600);
    else if (i == 4)
        enemy.setPosition(800, 309);
    else if (i == 5)
        enemy.setPosition(800, -470);
    random(i);
    if (i == 1)
        fly.setPosition(800, 350);
    else if (i == 2)
        fly.setPosition(800, -180);
    else if (i == 3)
        fly.setPosition(800, 370);
    else if (i == 4)
        fly.setPosition(800, -239);
    else if (i == 5)
        fly.setPosition(800, 284);
    random(i);
    if (i == 1)
        gs.setPosition(800, -310);
    else if (i == 2)
        gs.setPosition(800, 280);
    else if (i == 3)
        gs.setPosition(800, -270);
    else if (i == 4)
        gs.setPosition(800, 300);
    else if (i == 5)
        gs.setPosition(800, -330);
    //----------------------------------------------FONT LOADER-------------------------------------------------------
    Color fontColor = Color::Red;
    Font font;
    font.loadFromFile("strikefighter.ttf");
    Text scoreText("Score: 0", font, 30);
    scoreText.setPosition(10, 10);
    scoreText.setFillColor(fontColor);
    Text levelText("Level: 1", font, 30);
    levelText.setPosition(580, 10);
    levelText.setFillColor(fontColor);
    Text livesText("Lives: 3", font, 30);
    livesText.setPosition(290, 10);
    livesText.setFillColor(fontColor);
    Clock clock;
    Clock clock2;
    Clock clock3;
    Clock clock4;
    //------------------------------------------MAIN WINDOW-------------------------------------------------------
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::EventType::Closed)
                window.close();
        }
        //----------------------------------------DINOSAUR IDLE ANIMATION-----------------------------------------
        if (clock2.getElapsedTime().asSeconds() > 0.04f)
        {
            if (dinorect.left >= 880)
                dinorect.left = 0;
            else
                dinorect.left += 24;
            dino.setTextureRect(dinorect);
            clock2.restart();
        }
        //----------------------------------------------MONSTERS ANIMATION------------------------------------------------
        if (clock3.getElapsedTime().asSeconds() > 0.02f)
        {
            if (enerect.left >= 170)
                enerect.left = 0;
            else
                enerect.left += 16;
            enemy.setTextureRect(enerect);

            if (flyrect.left >= 90)
                flyrect.left = 0;
            else
                flyrect.left += 18;
            fly.setTextureRect(flyrect);

            if (slimerect.left >= 79)
                slimerect.left = 0;
            else
                slimerect.left += 16;
            gs.setTextureRect(slimerect);
        }
        //--------------------------------------------------CROUCH & JUMP ANIMATION---------------------------------------
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            dino.setTextureRect(upp);
        }
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            dino.setTextureRect(downn);
        }
        //---------------------------------------------------COLLISION FUNCCTION CALL------------------------------------
        if (collision(dino, enemy, gs, fly))
        {
            dino.setTextureRect(hitt);
            playerLives(livesText, window);
            if (reset == true)
            {
                reset = false;
                goto reset;
            }
        }
        //----------------------------------------------WASD MOVEMENT & ENEMY MOVEMENT------------------------------------
        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            up = true;
            Time deltaTime = clock.restart();
            upwards(dino, up, speed);
        }
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            down = true;
            Time deltaTime = clock.restart();
            downwards(dino, down, speed);
        }
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            rightt = true;
            Time deltaTime = clock.restart();
            right(dino, rightt, speed);
        }
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            leftt = true;
            Time deltaTime = clock.restart();
            left(dino, leftt, speed);
        }
        if (up == true || down == true || rightt == true || leftt == true)
        {
            enemove1(fly, flyspeed, clock, window);
            enemove2(gs, slimespeed, clock, window);
            enemove(enemy, monsterspeed, clock, window);
            levelUp(levelText);
        }
        else
        {
            enemove1(fly, flyspeed, clock, window);
            enemove2(gs, slimespeed, clock, window);
            enemove(enemy, monsterspeed, clock, window);
            levelUp(levelText);
        }
        //------------------------------------------------PAUSE MENU------------------------------------------------------       
        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            pauseMenu(window);
            clock4.restart();
        }
        //-------------------------------------------------DISPLAY--------------------------------------------------------
        window.clear();
        window.draw(background);
        window.draw(levelText);
        window.draw(livesText);
        updateScore(window, scoreText, clock4, font);
        window.draw(dino);
        window.draw(enemy);
        window.draw(fly);
        window.draw(gs);
        window.display();
    }
    //-----------------------------------------------------END--------------------------------------------------------    
}