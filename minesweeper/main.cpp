#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
using namespace std;
class Textures
{
public:
    map<string, sf::Texture*> textures;
    Textures()
    {
        sf::Texture* debug = new sf::Texture;
        debug->loadFromFile("files/images/debug.png");
        textures["debug"] = debug;
        sf::Texture* digits = new sf::Texture;
        digits->loadFromFile("files/images/digits.png");
        textures["digits"] = digits;
        sf::Texture* face_happy = new sf::Texture;
        face_happy->loadFromFile("files/images/face_happy.png");
        textures["face_happy"] = face_happy;
        sf::Texture* face_lose = new sf::Texture;
        face_lose->loadFromFile("files/images/face_lose.png");
        textures["face_lose"] = face_lose;
        sf::Texture* face_win = new sf::Texture;
        face_win->loadFromFile("files/images/face_win.png");
        textures["face_win"] = face_win;
        sf::Texture* flag = new sf::Texture;
        flag->loadFromFile("files/images/flag.png");
        textures["flag"] = flag;
        sf::Texture* leaderboard = new sf::Texture;
        leaderboard->loadFromFile("files/images/leaderboard.png");
        textures["leaderboard"] = leaderboard;
        sf::Texture* mine = new sf::Texture;
        mine->loadFromFile("files/images/mine.png");
        textures["mine"] = mine;
        sf::Texture* number_1 = new sf::Texture;
        number_1->loadFromFile("files/images/number_1.png");
        textures["number_1"] = number_1;
        sf::Texture* number_2 = new sf::Texture;
        number_2->loadFromFile("files/images/number_2.png");
        textures["number_2"] = number_2;
        sf::Texture* number_3 = new sf::Texture;
        number_3->loadFromFile("files/images/number_3.png");
        textures["number_3"] = number_3;
        sf::Texture* number_4 = new sf::Texture;
        number_4->loadFromFile("files/images/number_4.png");
        textures["number_4"] = number_4;
        sf::Texture* number_5 = new sf::Texture;
        number_5->loadFromFile("files/images/number_5.png");
        textures["number_5"] = number_5;
        sf::Texture* number_6 = new sf::Texture;
        number_6->loadFromFile("files/images/number_6.png");
        textures["number_6"] = number_6;
        sf::Texture* number_7 = new sf::Texture;
        number_7->loadFromFile("files/images/number_7.png");
        textures["number_7"] = number_7;
        sf::Texture* number_8 = new sf::Texture;
        number_8->loadFromFile("files/images/number_8.png");
        textures["number_8"] = number_8;
        sf::Texture* pause = new sf::Texture;
        pause->loadFromFile("files/images/pause.png");
        textures["pause"] = pause;
        sf::Texture* play = new sf::Texture;
        play->loadFromFile("files/images/play.png");
        textures["play"] = play;
        sf::Texture* tile_hidden = new sf::Texture;
        tile_hidden->loadFromFile("files/images/tile_hidden.png");
        textures["tile_hidden"] = tile_hidden;
        sf::Texture* tile_revealed = new sf::Texture;
        tile_revealed->loadFromFile("files/images/tile_revealed.png");
        textures["tile_revealed"] = tile_revealed;
    }
};
string replaceLine(string original, const string& toReplace, const string& replacement) {
    size_t pos = 0;
    while ((pos = original.find(toReplace, pos)) != string::npos) {
        original.replace(pos, toReplace.length(), replacement);
        pos += replacement.length();
    }
    return original;
}
int Random(int min, int max){
    static mt19937 random_mt(random_device{}());
    uniform_int_distribution<int> dist(min, max);
    return dist(random_mt);
}
void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

class Tile{
public:
    Textures texturesMap;
    sf::Sprite tile;
    bool flag = false;
    bool bomb = false;
    bool revealed = false;
    int adjBombs = 0;
    vector<Tile*> neighbors;
    Tile() : texturesMap(){
        this -> tile.setTexture(*texturesMap.textures["tile_hidden"]);
    }
};
class welcomeMenu {
public:
    int height, width, mineCount, row, col;

    sf::RenderWindow window;
    welcomeMenu() {
        string rowS, colS, mine;
        ifstream file("files/config.cfg");
        if (!file.is_open()) {
            cerr << "No config file found" << endl;
        }
        getline(file, colS);
        getline(file, rowS);
        getline(file, mine);
        file.close();
        row = stoi(rowS);
        col = stoi(colS);
        height = stoi(rowS) * 32 + 100;
        width = stoi(colS) * 32;
        mineCount = stoi(mine);
        window.create(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
    }
};

class gameWindow{
public:
    Textures texturesMap;
    int height, width, mineCount, row, col;
    sf::RenderWindow window;
    vector<vector<bool>> mineField;
    vector<vector<Tile>> spriteMineField;
    vector<sf::Sprite> flags;
    vector<sf::Sprite> bombs;
    vector<sf::Sprite> numbers;
    bool reveal(Tile& iniTile, vector<vector<Tile>>& spriteMineField) {
        bool isBomb = false;
        for (int i = 0; i < iniTile.neighbors.size(); i++) {
            if (iniTile.neighbors[i]->bomb) {
                isBomb = true;
                break;
            }
        }
        if (!isBomb) {
            for (int i = 0; i < iniTile.neighbors.size(); i++) {
                if (!iniTile.neighbors[i]->flag && iniTile.neighbors[i]->tile.getScale() == sf::Vector2f(1, 1)) {
                    iniTile.neighbors[i]->tile.setScale(0, 0);
                    iniTile.neighbors[i]->revealed = true;
                    reveal(*iniTile.neighbors[i], spriteMineField);
                }
            }
        }
        return !isBomb;
    }
    void setNeighbors() {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                //above
                if (i > 0) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i-1][j]);
                //below
                if (i < row - 1) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i+1][j]);
                //left
                if (j > 0) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i][j-1]);
                //right
                if (j < col - 1) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i][j+1]);
                //top left
                if (i > 0 && j > 0) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i-1][j-1]);
                //top right
                if (i > 0 && j < col - 1) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i-1][j+1]);
                //bottom left
                if (i < row - 1 && j > 0) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i+1][j-1]);
                //bottom right
                if (i < row - 1 && j < col - 1) spriteMineField[i][j].neighbors.push_back(&spriteMineField[i+1][j+1]);
            }
        }
    }
    gameWindow(int height, int width, int mineCount, int row, int col) : texturesMap(){
        this -> height = height;
        this -> width = width;
        this -> mineCount = mineCount;
        this -> row = row;
        this -> col = col;
        window.create(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
        mineField.resize(row, vector<bool>(col, false));
        spriteMineField.resize(row);
        for(int i = 0; i < row; i++){
            spriteMineField[i].resize(col);
            for(int j = 0; j < col; j++){
                Tile UItile;
                UItile.tile.setPosition(j * 32, i * 32);
                spriteMineField[i][j] = UItile;
            }
        }
        setNeighbors();
        int currMineCnt = 0;
        while (currMineCnt < mineCount) {
            int randomX = Random(0, (col - 1));
            int randomY = Random(0, (row - 1));
            if(!mineField[randomY][randomX]) {
                mineField[randomY][randomX] = true;
                currMineCnt++;
            }
        }//randomize mines
        for(int i = 0; i < mineField.size(); i++){
            for(int j = 0; j < mineField[i].size(); j++){
                if(mineField[i][j]){
                    spriteMineField[i][j].bomb = true;
                    sf::Sprite bomb;
                    bomb.setTexture(*texturesMap.textures["mine"]);
                    bomb.setPosition(spriteMineField[i][j].tile.getPosition());
                    bombs.push_back(bomb);
                }
            }
        }//bomb sprites
        for(int i = 0; i < spriteMineField.size(); i++){
            for(int j = 0; j < spriteMineField[i].size(); j++){
                for(int k = 0; k < spriteMineField[i][j].neighbors.size(); k++){
                    if (spriteMineField[i][j].neighbors[k]->bomb && (!spriteMineField[i][j].bomb)){
                        spriteMineField[i][j].adjBombs += 1;
                    }
                }
                if(spriteMineField[i][j].adjBombs == 1){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_1"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
                else if(spriteMineField[i][j].adjBombs == 2){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_2"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
                else if(spriteMineField[i][j].adjBombs == 3){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_3"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
                else if(spriteMineField[i][j].adjBombs == 4){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_4"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
                else if(spriteMineField[i][j].adjBombs == 5){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_5"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
                else if(spriteMineField[i][j].adjBombs == 6){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_6"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
                else if(spriteMineField[i][j].adjBombs == 7){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_7"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
                else if(spriteMineField[i][j].adjBombs == 8){
                    sf::Sprite tileNumber;
                    tileNumber.setTexture(*texturesMap.textures["number_8"]);
                    tileNumber.setPosition(spriteMineField[i][j].tile.getPosition());
                    numbers.push_back(tileNumber);
                }
            }
        }
    }
};
class leaderboardWindow{
public:
    int height, width;
    sf::RenderWindow window;
    sf::Text title;
    sf::Text data;
    sf::Font font;
    leaderboardWindow(int height, int width){
        this -> height = height * 16 + 50;
        this -> width = width * 16;
        this -> window.create(sf::VideoMode(this->width, this->height), "Minesweeper", sf::Style::Close);
        font.loadFromFile("files/font.ttf");
        ifstream file("files/leaderboard.txt");
        string leaderboardData;
        string first, second, third, fourth, fifth;
            getline(file, leaderboardData, ',');
            first += leaderboardData + "\t";
            getline(file, leaderboardData);
            first += leaderboardData + "\n\n";
            getline(file, leaderboardData, ',');
            second += leaderboardData + "\t";
            getline(file, leaderboardData);
            second += leaderboardData + "\n\n";
            getline(file, leaderboardData, ',');
            third += leaderboardData + "\t";
            getline(file, leaderboardData);
            third += leaderboardData + "\n\n";
            getline(file, leaderboardData, ',');
            fourth += leaderboardData + "\t";
            getline(file, leaderboardData);
            fourth += leaderboardData + "\n\n";
            getline(file, leaderboardData, ',');
            fifth += leaderboardData + "\t";
            getline(file, leaderboardData);
            fifth += leaderboardData + "\n\n";
        this -> title.setString("LEADERBOARD");
        this -> title.setStyle((sf::Text::Bold | sf::Text::Underlined));
        this -> title.setCharacterSize(20);
        this -> title.setFont(font);
        setText(this->title, this -> width/2, this -> height/2 - 120);
        this -> data.setString("1.\t" + first + "2.\t" + second + "3.\t" + third + "4.\t" + fourth + "5.\t" + fifth);
        this -> data.setStyle((sf::Text::Bold));
        this -> data.setCharacterSize(18);
        this -> data.setFont(font);
        setText(this->data, this -> width/2, this -> height/2 + 20);
    }
};

int main() {
    Textures texturesMap;
    bool won = false;
    bool gameRun = false;
    welcomeMenu WELCOME;
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    string name;
    sf::RectangleShape background(sf::Vector2f(WELCOME.width, WELCOME.height));
    background.setFillColor(sf::Color::Blue);
    bool debugButton = false;
    bool paused = false;
    bool gameRunning = false;
    bool leaderboardOn = false;
    while (WELCOME.window.isOpen()) {
        sf::Event event;
        while (WELCOME.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                WELCOME.window.close();
            if (event.type == sf::Event::TextEntered) {
                if (((event.text.unicode >= 65 && event.text.unicode <= 90) || (event.text.unicode >= 97 && event.text.unicode <= 122) || (event.text.unicode == 32)) && name.length() < 10) {
                    name += tolower(static_cast<char>(event.text.unicode));
                    name[0] = toupper(name[0]);

                }
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace && !name.empty()) {
                name.pop_back();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return && !name.empty()) {
                gameRun = true;
                WELCOME.window.close();
            }
        }

        sf::Text welcomeMessage("WELCOME TO MINESWEEPER!", font, 24);
        setText(welcomeMessage, WELCOME.width / 2, WELCOME.height / 2 - 150);
        welcomeMessage.setStyle(sf::Text::Bold | sf::Text::Underlined);

        sf::Text namePrompt("Enter your name:", font, 20);
        setText(namePrompt, WELCOME.width / 2, WELCOME.height / 2 - 75);
        namePrompt.setStyle(sf::Text::Bold);

        sf::Text nameInput(name + "|", font, 18);
        setText(nameInput, WELCOME.width / 2, WELCOME.height / 2 - 45);
        nameInput.setStyle(sf::Text::Bold);
        nameInput.setColor(sf::Color::Yellow);

        WELCOME.window.clear();
        WELCOME.window.draw(background);
        WELCOME.window.draw(welcomeMessage);
        WELCOME.window.draw(namePrompt);
        WELCOME.window.draw(nameInput);
        WELCOME.window.display();
    }
    while (gameRun) {
        gameRun = false;
        gameWindow GAME(WELCOME.height, WELCOME.width, WELCOME.mineCount, WELCOME.row, WELCOME.col);
        sf::RectangleShape gameBackground(sf::Vector2f(GAME.width, GAME.height));
        gameBackground.setFillColor(sf::Color::White);
        sf::Sprite numberStrip;
        numberStrip.setTexture(*texturesMap.textures["digits"]);
        sf::Sprite happyFace;
        happyFace.setTexture(*texturesMap.textures["face_happy"]);
        happyFace.setPosition((((GAME.col/2.0) * 32) - 32), ((GAME.row + 0.5) * 32));
        sf::Sprite winFace;
        winFace.setTexture(*texturesMap.textures["face_win"]);
        winFace.setPosition((((GAME.col/2.0) * 32) - 32), ((GAME.row + 0.5) * 32));
        sf::Sprite loseFace;
        loseFace.setTexture(*texturesMap.textures["face_lose"]);
        loseFace.setPosition((((GAME.col/2.0) * 32) - 32), ((GAME.row + 0.5) * 32));
        sf::Sprite debug;
        debug.setTexture(*texturesMap.textures["debug"]);
        debug.setPosition((GAME.col * 32) - 304, 32 * (GAME.row + 0.5));
        sf::Sprite leaderBoardButton;
        leaderBoardButton.setTexture(*texturesMap.textures["leaderboard"]);
        leaderBoardButton.setPosition((GAME.col * 32) - 176, 32 * (GAME.row + 0.5));
        sf::Sprite play;
        play.setTexture(*texturesMap.textures["play"]);
        play.setPosition((GAME.col * 32) - 240, 32 * (GAME.row + 0.5));
        sf::Sprite pause;
        pause.setTexture(*texturesMap.textures["pause"]);
        pause.setPosition((GAME.col * 32) - 240, 32 * (GAME.row + 0.5));
        sf::Texture texture;
        texture.loadFromFile("files/images/tile_revealed.png");
        texture.setRepeated(true);
        sf::Sprite revealed;
        revealed.setTexture(texture);
        revealed.setTextureRect(sf::IntRect(0,0,800,512));
        bool lost = false;
        bool showMines = false;
        int counter = GAME.mineCount;
        auto start = chrono::high_resolution_clock::now();
        chrono::seconds timePointWhenPaused = chrono::seconds(0);
        chrono::seconds timePointWhenUnpaused = chrono::seconds(0);
        while (GAME.window.isOpen() && !lost) {
            sf::Event event;
            while (GAME.window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    GAME.window.close();
                if (event.type == sf::Event::MouseButtonPressed){
                    if (event.mouseButton.button == sf::Mouse::Left){
                        int mouseX = event.mouseButton.x;
                        int mouseY = event.mouseButton.y;
                        if (happyFace.getGlobalBounds().contains(mouseX, mouseY)) {
                            debugButton = false;
                            GAME.window.close();
                            gameRun = true;
                        }
                        if (debug.getGlobalBounds().contains(mouseX, mouseY)) {
                            if (!debugButton){
                                debugButton = true;
                            }
                            else if (debugButton){
                                debugButton = false;
                            }
                        }
                        if (leaderBoardButton.getGlobalBounds().contains(mouseX, mouseY)) {
                            GAME.window.draw(revealed);
                            GAME.window.display();
                            auto end = chrono::high_resolution_clock ::now();
                            timePointWhenPaused = chrono::duration_cast<chrono::seconds>(end - start);
                            timePointWhenPaused -= timePointWhenUnpaused;
                            leaderboardOn = true;
                        }
                        if (pause.getGlobalBounds().contains(mouseX, mouseY)) {
                            if (!paused){
                                auto end = chrono::high_resolution_clock ::now();
                                timePointWhenPaused = chrono::duration_cast<chrono::seconds>(end - start);
                                timePointWhenPaused -= timePointWhenUnpaused;
                                paused = true;
                            }
                            else{
                                paused = false;
                            }

                        }
                        for(int i = 0; i < GAME.spriteMineField.size(); i++){
                            for(int j = 0; j < GAME.spriteMineField[i].size(); j++){
                                if (GAME.spriteMineField[i][j].tile.getGlobalBounds().contains(mouseX, mouseY) && !GAME.spriteMineField[i][j].flag && !GAME.spriteMineField[i][j].bomb) {
                                    bool bombAdj = false;
                                    for(int k = 0; k < GAME.spriteMineField[i][j].neighbors.size(); k++){
                                        if (GAME.spriteMineField[i][j].neighbors[k]->bomb){
                                            bombAdj= true;
                                        }
                                    }
                                    if (bombAdj){
                                        GAME.spriteMineField[i][j].tile.setScale(0,0);
                                        GAME.spriteMineField[i][j].revealed = true;

                                    }
                                    else if(!(bombAdj)){
                                        GAME.spriteMineField[i][j].tile.setScale(0,0);
                                        GAME.spriteMineField[i][j].revealed = true;
                                        GAME.reveal(GAME.spriteMineField[i][j], GAME.spriteMineField);
                                    }
                                    int hiddenTilesRem = GAME.row * GAME.col;
                                    for(int i = 0; i < GAME.spriteMineField.size(); i++){
                                        for(int j = 0; j < GAME.spriteMineField[i].size(); j++) {
                                            if(GAME.spriteMineField[i][j].revealed == true){
                                                hiddenTilesRem -= 1;
                                            }
                                        }
                                    }
                                    if(hiddenTilesRem == GAME.mineCount){
                                        auto end = chrono::high_resolution_clock ::now();
                                        timePointWhenPaused = chrono::duration_cast<chrono::seconds>(end - start);
                                        timePointWhenPaused -= timePointWhenUnpaused;
                                        won = true;

                                    }
                                }
                                else if (GAME.spriteMineField[i][j].tile.getGlobalBounds().contains(mouseX, mouseY) && !GAME.spriteMineField[i][j].flag && GAME.spriteMineField[i][j].bomb) {
                                    GAME.spriteMineField[i][j].tile.setScale(0,0);
                                    showMines = true;
                                }
                            }
                        }
                        if (showMines) {
                            for (int i = 0; i < GAME.spriteMineField.size(); i++) {
                                for (int j = 0; j < GAME.spriteMineField[i].size(); j++) {
                                    if (GAME.spriteMineField[i][j].bomb) {
                                        GAME.spriteMineField[i][j].tile.setScale(0, 0);
                                    }

                                }
                            }
                            GAME.window.clear();
                            GAME.window.draw(gameBackground);
                            GAME.window.draw(happyFace);
                            GAME.window.draw(debug);
                            GAME.window.draw(leaderBoardButton);
                            GAME.window.draw(play);
                            GAME.window.draw(pause);
                            GAME.window.draw(revealed);
                            for(int i = 0; i < GAME.bombs.size(); i++){
                                GAME.window.draw(GAME.bombs[i]);
                            }
                            for (int i = 0; i < GAME.numbers.size(); i++){
                                GAME.window.draw(GAME.numbers[i]);
                            }
                            for(int i = 0; i < GAME.spriteMineField.size(); i++){
                                for(int j = 0; j < GAME.spriteMineField[i].size(); j++){
                                    GAME.window.draw(GAME.spriteMineField[i][j].tile);
                                }
                            }
                            for(int i = 0; i < GAME.flags.size(); i++){
                                GAME.window.draw(GAME.flags[i]);
                            }

                            GAME.window.display();
                            auto end = chrono::high_resolution_clock ::now();
                            timePointWhenPaused = chrono::duration_cast<chrono::seconds>(end - start);
                            timePointWhenPaused -= timePointWhenUnpaused;
                            lost = true;
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Right){
                        int mouseX = event.mouseButton.x;
                        int mouseY = event.mouseButton.y;
                        for(int i = 0; i < GAME.spriteMineField.size(); i++){
                            for(int j = 0; j < GAME.spriteMineField[i].size(); j++){
                                if ((GAME.spriteMineField[i][j].tile.getGlobalBounds().contains(mouseX, mouseY)) && !(GAME.spriteMineField[i][j].flag)) {
                                    sf::Sprite flag;
                                    flag.setTexture(*texturesMap.textures["flag"]);
                                    flag.setPosition(GAME.spriteMineField[i][j].tile.getPosition());
                                    GAME.flags.push_back(flag);
                                    GAME.spriteMineField[i][j].flag = true;
                                    counter -= 1;
                                }
                                else if ((GAME.spriteMineField[i][j].tile.getGlobalBounds().contains(mouseX, mouseY)) && (GAME.spriteMineField[i][j].flag)){
                                    for(int k = 0; k < GAME.flags.size(); k++){
                                        if ((GAME.flags[k].getGlobalBounds().contains(mouseX, mouseY))){
                                            GAME.flags.erase(GAME.flags.begin() + k);
                                            GAME.spriteMineField[i][j].flag = false;
                                            counter += 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(leaderboardOn){
                leaderboardWindow leaderboard(GAME.row, GAME.col);
                sf::RectangleShape leaderboardBackground(sf::Vector2f(leaderboard.width, leaderboard.height));
                leaderboardBackground.setFillColor(sf::Color::Blue);
                while(leaderboard.window.isOpen()){
                    sf::Event event1;
                    leaderboard.window.clear();
                    leaderboard.window.draw(leaderboardBackground);
                    leaderboard.window.draw(leaderboard.title);
                    leaderboard.window.draw(leaderboard.data);
                    leaderboard.window.display();
                    while(leaderboard.window.pollEvent(event1)){
                        if(event1.type == sf::Event::Closed){
                            while (GAME.window.pollEvent(event))
                            {
                                if (event.type == sf::Event::MouseButtonPressed)
                                {
                                    continue;
                                }
                            }
                            auto end = chrono::high_resolution_clock ::now();
                            timePointWhenUnpaused = chrono::duration_cast<chrono::seconds>(end - start);
                            timePointWhenUnpaused -= timePointWhenPaused;
                            leaderboard.window.close();
                            leaderboardOn = false;
                        }
                    }
                }
            }
            if(won){
                ifstream file("files/leaderboard.txt");
                string leaderboardData;
                string toWrite, firstPlace, secondPlace, thirdPlace, fourthPlace, fifthPlace;
                string firstTime, secondTime, thirdTime, fourthTime, fifthTime, firstName, secondName, thirdName, fourthName, fifthName;
                getline(file, leaderboardData, ':');
                firstTime += leaderboardData;
                toWrite += leaderboardData + ":";
                firstPlace += leaderboardData + ":";
                getline(file, leaderboardData, ',');
                toWrite += leaderboardData + ",";
                firstPlace += leaderboardData + ",";
                firstTime += leaderboardData;
                getline(file, leaderboardData);
                firstName = leaderboardData;
                toWrite += leaderboardData + "\n";
                firstPlace += leaderboardData + "\n";
                getline(file, leaderboardData, ':');
                secondTime += leaderboardData;
                toWrite += leaderboardData + ":";
                secondPlace += leaderboardData + ":";
                getline(file, leaderboardData, ',');
                toWrite += leaderboardData + ",";
                secondPlace += leaderboardData + ",";
                secondTime += leaderboardData;
                getline(file, leaderboardData);
                secondName = leaderboardData;
                toWrite += leaderboardData + "\n";
                secondPlace += leaderboardData + "\n";
                getline(file, leaderboardData, ':');
                thirdTime += leaderboardData;
                toWrite += leaderboardData + ":";
                thirdPlace += leaderboardData + ":";
                getline(file, leaderboardData, ',');
                toWrite += leaderboardData + ",";
                thirdPlace += leaderboardData + ",";
                thirdTime += leaderboardData;
                getline(file, leaderboardData);
                thirdName = leaderboardData;
                toWrite += leaderboardData + "\n";
                thirdPlace += leaderboardData +"\n";
                getline(file, leaderboardData, ':');
                fourthTime += leaderboardData;
                toWrite += leaderboardData + ":";
                fourthPlace += leaderboardData + ":";
                getline(file, leaderboardData, ',');
                toWrite += leaderboardData + ",";
                fourthPlace += leaderboardData + ",";
                fourthTime += leaderboardData;
                getline(file, leaderboardData);
                fourthName = leaderboardData;
                toWrite += leaderboardData + "\n";
                fourthPlace += leaderboardData + "\n";
                getline(file, leaderboardData, ':');
                fifthTime += leaderboardData;
                toWrite += leaderboardData + ":";
                fifthPlace += leaderboardData + ":";
                getline(file, leaderboardData, ',');
                toWrite += leaderboardData + ",";
                fifthPlace += leaderboardData + ",";
                fifthTime += leaderboardData;
                getline(file, leaderboardData);
                fifthName = leaderboardData;
                toWrite += leaderboardData + "\n";
                fifthPlace += leaderboardData + "\n";
                file.close();
                auto end = chrono::high_resolution_clock ::now();
                auto timeElapsed = chrono::duration_cast<chrono::seconds>(end - start);
                timeElapsed = timePointWhenPaused;
                int finishTime = timeElapsed.count();
                int minutes = finishTime/60;
                string minutesS;
                string secondsS;
                if (minutes < 10){
                    minutesS = "0" + to_string(minutes);
                }
                else if (minutes >= 10){
                    minutesS = to_string(minutes);
                }
                int seconds = finishTime%60;
                if (seconds < 10){
                    secondsS = "0" + to_string(seconds);
                }
                else if (seconds >= 10){
                    secondsS = to_string(seconds);
                }
                vector<string> timeThresholds = {firstTime, secondTime, thirdTime, fourthTime, fifthTime};
                vector<string> placements = {firstPlace, secondPlace, thirdPlace, fourthPlace, fifthPlace};
                string recordedTime = minutesS + secondsS;
                bool foundThreshold = false;
                for (int i = 0; i < timeThresholds.size(); ++i) {
                    if (foundThreshold){
                        break;
                    }
                    if (recordedTime < timeThresholds[i]) {
                        for (int j = placements.size() - 1; j > i; j--){
                                timeThresholds[j] = timeThresholds[j - 1];
                                placements[j] = placements[j - 1];
                        }
                        for (int k = 0; k < placements.size(); k++){
                            placements[k].erase(remove(placements[k].begin(), placements[k].end(), '*'), placements[k].end());
                        }
                        placements[i] = "//";
                        timeThresholds[i] = recordedTime;
                        toWrite = placements[0] + placements[1] + placements[2] + placements[3] + placements [4];
                        toWrite = replaceLine(toWrite, "//", minutesS + ":"  + secondsS + ", " + name + "*" + "\n");
                        foundThreshold = true;
                        ofstream leaderboard("files/leaderboard.txt", ofstream::out | ofstream::trunc);
                        leaderboard << toWrite;
                        leaderboard.close();
                    }
                }
            }
            while(won){
                sf::Event event;
                while (GAME.window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed){
                        GAME.window.close();
                        return 0;
                    }
                    if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            int mouseX = event.mouseButton.x;
                            int mouseY = event.mouseButton.y;
                            if (winFace.getGlobalBounds().contains(mouseX, mouseY)) {
                                won = false;
                                GAME.window.close();
                                gameRun = true;
                            }
                            if (leaderBoardButton.getGlobalBounds().contains(mouseX, mouseY)) {
                                GAME.window.draw(revealed);
                                GAME.window.display();
                                leaderboardOn = true;
                            }
                        }
                    }
                    if(leaderboardOn){
                        leaderboardWindow leaderboard(GAME.row, GAME.col);
                        sf::RectangleShape leaderboardBackground(sf::Vector2f(leaderboard.width, leaderboard.height));
                        leaderboardBackground.setFillColor(sf::Color::Blue);
                        while(leaderboard.window.isOpen()){
                            sf::Event event1;
                            leaderboard.window.clear();
                            leaderboard.window.draw(leaderboardBackground);
                            leaderboard.window.draw(leaderboard.title);
                            leaderboard.window.draw(leaderboard.data);
                            leaderboard.window.display();
                            while(leaderboard.window.pollEvent(event1)){
                                if(event1.type == sf::Event::Closed){
                                    while (GAME.window.pollEvent(event))
                                    {
                                        if (event.type == sf::Event::MouseButtonPressed)
                                        {
                                            continue;
                                        }
                                    }
                                    auto end = chrono::high_resolution_clock ::now();
                                    timePointWhenUnpaused = chrono::duration_cast<chrono::seconds>(end - start);
                                    timePointWhenUnpaused -= timePointWhenPaused;
                                    leaderboard.window.close();
                                    leaderboardOn = false;
                                }

                            }
                        }
                    }
                }
                GAME.window.clear();
                GAME.window.draw(gameBackground);
                GAME.window.draw(winFace);
                GAME.window.draw(debug);
                GAME.window.draw(leaderBoardButton);
                GAME.window.draw(pause);
                GAME.window.draw(revealed);
                bool isNegative = counter < 0;
                int absCounter = abs(counter);
                int hundredsPlace = absCounter / 100;
                int tensPlace = (absCounter % 100) / 10;
                int onesPlace = absCounter % 10;
                if (isNegative)
                {
                    sf::Sprite negativeSign;
                    negativeSign.setTexture(*texturesMap.textures["digits"]);
                    negativeSign.setTextureRect(sf::IntRect((21 * 10), 0, 21, 32));
                    negativeSign.setPosition(12, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(negativeSign);
                }
                sf::Sprite hundredsDigit;
                hundredsDigit.setTexture(*texturesMap.textures["digits"]);
                hundredsDigit.setTextureRect(sf::IntRect((21 * hundredsPlace), 0, 21, 32));
                hundredsDigit.setPosition(33 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(hundredsDigit);
                sf::Sprite tensDigit;
                tensDigit.setTexture(*texturesMap.textures["digits"]);
                tensDigit.setTextureRect(sf::IntRect((21 * (isNegative || counter >= 10 ? tensPlace : 0)), 0, 21, 32));
                tensDigit.setPosition(54 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(tensDigit);
                sf::Sprite onesDigit;
                onesDigit.setTexture(*texturesMap.textures["digits"]);
                onesDigit.setTextureRect(sf::IntRect((21 * onesPlace), 0, 21, 32));
                onesDigit.setPosition(75 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(onesDigit);
                for(int i = 0; i < GAME.bombs.size(); i++){
                    GAME.window.draw(GAME.bombs[i]);
                }
                for (int i = 0; i < GAME.numbers.size(); i++){
                    GAME.window.draw(GAME.numbers[i]);
                }
                for(int i = 0; i < GAME.spriteMineField.size(); i++){
                    for(int j = 0; j < GAME.spriteMineField[i].size(); j++){
                        GAME.window.draw(GAME.spriteMineField[i][j].tile);
                        if(GAME.spriteMineField[i][j].bomb){
                            sf::Sprite flag;
                            flag.setTexture(*texturesMap.textures["flag"]);
                            flag.setPosition(GAME.spriteMineField[i][j].tile.getPosition());
                            GAME.flags.push_back(flag);
                            GAME.spriteMineField[i][j].flag = true;
                        }
                    }
                }
                for(int i = 0; i < GAME.flags.size(); i++){
                    GAME.window.draw(GAME.flags[i]);
                }
                auto end = chrono::high_resolution_clock ::now();
                auto timeElapsed = chrono::duration_cast<chrono::seconds>(end - start);
                timeElapsed = timePointWhenPaused;
                auto oneMinute = chrono::seconds(60);
                auto one = chrono::seconds(1);
                auto minutesElapsed = timeElapsed / oneMinute;
                int minutesTimer = minutesElapsed;
                int tensPlaceMinutes = floor(minutesTimer / 10);
                int onesPlaceMinutes = minutesTimer % 10;
                sf::Sprite firstMinuteDigit;
                firstMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstMinuteDigit.setTextureRect(sf::IntRect((21 * tensPlaceMinutes), 0, 21, 32));
                firstMinuteDigit.setPosition((GAME.col * 32) - 97, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstMinuteDigit);
                sf::Sprite secondMinuteDigit;
                secondMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondMinuteDigit.setTextureRect(sf::IntRect((21 * onesPlaceMinutes), 0, 21, 32));
                secondMinuteDigit.setPosition((GAME.col * 32) - 76, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondMinuteDigit);
                auto secondsElapsed = (timeElapsed % oneMinute) / one;
                int secondsTimer = secondsElapsed;
                int tensPlaceSeconds = floor(secondsTimer / 10);
                sf::Sprite firstSecondDigit;
                firstSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstSecondDigit.setTextureRect(sf::IntRect((21 * tensPlaceSeconds), 0, 21, 32));
                firstSecondDigit.setPosition((GAME.col * 32) - 54, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstSecondDigit);
                int onesPlaceSeconds = floor(secondsTimer % 10);
                sf::Sprite secondSecondDigit;
                secondSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondSecondDigit.setTextureRect(sf::IntRect((21 * onesPlaceSeconds), 0, 21, 32));
                secondSecondDigit.setPosition((GAME.col * 32) - 33, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondSecondDigit);
                GAME.window.display();
            }
            while(paused) {
                sf::Event event;
                while (GAME.window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed){
                        GAME.window.close();
                        return 0;
                    }
                    if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            int mouseX = event.mouseButton.x;
                            int mouseY = event.mouseButton.y;
                            if (happyFace.getGlobalBounds().contains(mouseX, mouseY)) {
                                GAME.window.close();
                                debugButton = false;
                                paused = false;
                                gameRun = true;
                            }
                            if (leaderBoardButton.getGlobalBounds().contains(mouseX, mouseY)) {
                                GAME.window.draw(revealed);
                                GAME.window.display();
                                leaderboardOn = true;
                            }
                            if (pause.getGlobalBounds().contains(mouseX, mouseY)) {
                                auto end = chrono::high_resolution_clock ::now();
                                timePointWhenUnpaused = chrono::duration_cast<chrono::seconds>(end - start);
                                timePointWhenUnpaused -= timePointWhenPaused;
                                paused = false;
                            }
                        }
                    }
                    if(leaderboardOn){
                        leaderboardWindow leaderboard(GAME.row, GAME.col);
                        sf::RectangleShape leaderboardBackground(sf::Vector2f(leaderboard.width, leaderboard.height));
                        leaderboardBackground.setFillColor(sf::Color::Blue);
                        while(leaderboard.window.isOpen()){
                            sf::Event event1;
                            leaderboard.window.clear();
                            leaderboard.window.draw(leaderboardBackground);
                            leaderboard.window.draw(leaderboard.title);
                            leaderboard.window.draw(leaderboard.data);
                            leaderboard.window.display();
                            while(leaderboard.window.pollEvent(event1)){
                                if(event1.type == sf::Event::Closed){
                                    while (GAME.window.pollEvent(event))
                                    {
                                        if (event.type == sf::Event::MouseButtonPressed)
                                        {
                                            continue;
                                        }
                                    }
                                    auto end = chrono::high_resolution_clock ::now();
                                    timePointWhenUnpaused = chrono::duration_cast<chrono::seconds>(end - start);
                                    timePointWhenUnpaused -= timePointWhenPaused;
                                    leaderboard.window.close();
                                    leaderboardOn = false;
                                }
                            }

                        }
                    }
                    GAME.window.clear();
                    GAME.window.draw(gameBackground);
                    GAME.window.draw(happyFace);
                    GAME.window.draw(debug);
                    GAME.window.draw(leaderBoardButton);
                    GAME.window.draw(play);
                    GAME.window.draw(revealed);
                    bool isNegative = counter < 0;
                    int absCounter = abs(counter);
                    int hundredsPlace = absCounter / 100;
                    int tensPlace = (absCounter % 100) / 10;
                    int onesPlace = absCounter % 10;
                    if (isNegative)
                    {
                        sf::Sprite negativeSign;
                        negativeSign.setTexture(*texturesMap.textures["digits"]);
                        negativeSign.setTextureRect(sf::IntRect((21 * 10), 0, 21, 32));
                        negativeSign.setPosition(12, 32 * (GAME.row + 0.5) + 16);
                        GAME.window.draw(negativeSign);
                    }
                    sf::Sprite hundredsDigit;
                    hundredsDigit.setTexture(*texturesMap.textures["digits"]);
                    hundredsDigit.setTextureRect(sf::IntRect((21 * hundredsPlace), 0, 21, 32));
                    hundredsDigit.setPosition(33 , 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(hundredsDigit);
                    sf::Sprite tensDigit;
                    tensDigit.setTexture(*texturesMap.textures["digits"]);
                    tensDigit.setTextureRect(sf::IntRect((21 * (isNegative || counter >= 10 ? tensPlace : 0)), 0, 21, 32));
                    tensDigit.setPosition(54 , 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(tensDigit);
                    sf::Sprite onesDigit;
                    onesDigit.setTexture(*texturesMap.textures["digits"]);
                    onesDigit.setTextureRect(sf::IntRect((21 * onesPlace), 0, 21, 32));
                    onesDigit.setPosition(75 , 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(onesDigit);
                    auto end = chrono::high_resolution_clock ::now();
                    auto timeElapsed = chrono::duration_cast<chrono::seconds>(end - start);
                    timeElapsed = timePointWhenPaused;
                    auto oneMinute = chrono::seconds(60);
                    auto one = chrono::seconds(1);
                    auto minutesElapsed = timeElapsed / oneMinute;
                    int minutesTimer = minutesElapsed;
                    int tensPlaceMinutes = floor(minutesTimer / 10);
                    int onesPlaceMinutes = minutesTimer % 10;
                    sf::Sprite firstMinuteDigit;
                    firstMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                    firstMinuteDigit.setTextureRect(sf::IntRect((21 * tensPlaceMinutes), 0, 21, 32));
                    firstMinuteDigit.setPosition((GAME.col * 32) - 97, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(firstMinuteDigit);
                    sf::Sprite secondMinuteDigit;
                    secondMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                    secondMinuteDigit.setTextureRect(sf::IntRect((21 * onesPlaceMinutes), 0, 21, 32));
                    secondMinuteDigit.setPosition((GAME.col * 32) - 76, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(secondMinuteDigit);
                    auto secondsElapsed = (timeElapsed % oneMinute) / one;
                    int secondsTimer = secondsElapsed;
                    int tensPlaceSeconds = floor(secondsTimer / 10);
                    sf::Sprite firstSecondDigit;
                    firstSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                    firstSecondDigit.setTextureRect(sf::IntRect((21 * tensPlaceSeconds), 0, 21, 32));
                    firstSecondDigit.setPosition((GAME.col * 32) - 54, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(firstSecondDigit);
                    int onesPlaceSeconds = floor(secondsTimer % 10);
                    sf::Sprite secondSecondDigit;
                    secondSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                    secondSecondDigit.setTextureRect(sf::IntRect((21 * onesPlaceSeconds), 0, 21, 32));
                    secondSecondDigit.setPosition((GAME.col * 32) - 33, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(secondSecondDigit);
                    GAME.window.display();
                }
            }
            while(lost) {
                sf::Event event;
                while (GAME.window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed){
                        GAME.window.close();
                        return 0;
                    }
                    if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            int mouseX = event.mouseButton.x;
                            int mouseY = event.mouseButton.y;
                            if (happyFace.getGlobalBounds().contains(mouseX, mouseY)) {
                                GAME.window.close();
                                lost = false;
                                debugButton = false;
                                gameRun = true;
                            }
                            if (leaderBoardButton.getGlobalBounds().contains(mouseX, mouseY)) {
                                GAME.window.draw(revealed);
                                GAME.window.display();
                                leaderboardOn = true;
                            }
                        }
                    }
                }
                if(leaderboardOn){
                    leaderboardWindow leaderboard(GAME.row, GAME.col);
                    sf::RectangleShape leaderboardBackground(sf::Vector2f(leaderboard.width, leaderboard.height));
                    leaderboardBackground.setFillColor(sf::Color::Blue);
                    while(leaderboard.window.isOpen()){
                        sf::Event event1;
                        leaderboard.window.clear();
                        leaderboard.window.draw(leaderboardBackground);
                        leaderboard.window.draw(leaderboard.title);
                        leaderboard.window.draw(leaderboard.data);
                        leaderboard.window.display();
                        while(leaderboard.window.pollEvent(event1)){
                            if(event1.type == sf::Event::Closed){
                                while (GAME.window.pollEvent(event))
                                {
                                    if (event.type == sf::Event::MouseButtonPressed)
                                    {
                                        continue;
                                    }
                                }
                                auto end = chrono::high_resolution_clock ::now();
                                timePointWhenUnpaused = chrono::duration_cast<chrono::seconds>(end - start);
                                timePointWhenUnpaused -= timePointWhenPaused;
                                leaderboard.window.close();
                                leaderboardOn = false;
                            }
                        }
                    }
                }
                GAME.window.clear();
                GAME.window.draw(gameBackground);
                GAME.window.draw(loseFace);
                GAME.window.draw(debug);
                GAME.window.draw(leaderBoardButton);
                GAME.window.draw(pause);
                GAME.window.draw(revealed);
                bool isNegative = counter < 0;
                int absCounter = abs(counter);
                int hundredsPlace = absCounter / 100;
                int tensPlace = (absCounter % 100) / 10;
                int onesPlace = absCounter % 10;
                if (isNegative)
                {
                    sf::Sprite negativeSign;
                    negativeSign.setTexture(*texturesMap.textures["digits"]);
                    negativeSign.setTextureRect(sf::IntRect((21 * 10), 0, 21, 32));
                    negativeSign.setPosition(12, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(negativeSign);
                }
                sf::Sprite hundredsDigit;
                hundredsDigit.setTexture(*texturesMap.textures["digits"]);
                hundredsDigit.setTextureRect(sf::IntRect((21 * hundredsPlace), 0, 21, 32));
                hundredsDigit.setPosition(33 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(hundredsDigit);
                sf::Sprite tensDigit;
                tensDigit.setTexture(*texturesMap.textures["digits"]);
                tensDigit.setTextureRect(sf::IntRect((21 * (isNegative || counter >= 10 ? tensPlace : 0)), 0, 21, 32));
                tensDigit.setPosition(54 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(tensDigit);
                sf::Sprite onesDigit;
                onesDigit.setTexture(*texturesMap.textures["digits"]);
                onesDigit.setTextureRect(sf::IntRect((21 * onesPlace), 0, 21, 32));
                onesDigit.setPosition(75 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(onesDigit);
                for(int i = 0; i < GAME.bombs.size(); i++){
                    GAME.window.draw(GAME.bombs[i]);
                }
                for (int i = 0; i < GAME.numbers.size(); i++){
                    GAME.window.draw(GAME.numbers[i]);
                }
                for(int i = 0; i < GAME.spriteMineField.size(); i++){
                    for(int j = 0; j < GAME.spriteMineField[i].size(); j++){
                        GAME.window.draw(GAME.spriteMineField[i][j].tile);
                    }
                }
                auto end = chrono::high_resolution_clock ::now();
                auto timeElapsed = chrono::duration_cast<chrono::seconds>(end - start);
                timeElapsed = timePointWhenPaused;
                auto oneMinute = chrono::seconds(60);
                auto one = chrono::seconds(1);
                auto minutesElapsed = timeElapsed / oneMinute;
                int minutesTimer = minutesElapsed;
                int tensPlaceMinutes = floor(minutesTimer / 10);
                int onesPlaceMinutes = minutesTimer % 10;
                sf::Sprite firstMinuteDigit;
                firstMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstMinuteDigit.setTextureRect(sf::IntRect((21 * tensPlaceMinutes), 0, 21, 32));
                firstMinuteDigit.setPosition((GAME.col * 32) - 97, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstMinuteDigit);
                sf::Sprite secondMinuteDigit;
                secondMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondMinuteDigit.setTextureRect(sf::IntRect((21 * onesPlaceMinutes), 0, 21, 32));
                secondMinuteDigit.setPosition((GAME.col * 32) - 76, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondMinuteDigit);
                auto secondsElapsed = (timeElapsed % oneMinute) / one;
                int secondsTimer = secondsElapsed;
                int tensPlaceSeconds = floor(secondsTimer / 10);
                sf::Sprite firstSecondDigit;
                firstSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstSecondDigit.setTextureRect(sf::IntRect((21 * tensPlaceSeconds), 0, 21, 32));
                firstSecondDigit.setPosition((GAME.col * 32) - 54, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstSecondDigit);
                int onesPlaceSeconds = floor(secondsTimer % 10);
                sf::Sprite secondSecondDigit;
                secondSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondSecondDigit.setTextureRect(sf::IntRect((21 * onesPlaceSeconds), 0, 21, 32));
                secondSecondDigit.setPosition((GAME.col * 32) - 33, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondSecondDigit);
                GAME.window.display();
            }

            if (!debugButton){
                GAME.window.clear();
                GAME.window.draw(gameBackground);
                GAME.window.draw(happyFace);
                GAME.window.draw(debug);
                GAME.window.draw(leaderBoardButton);
                GAME.window.draw(play);
                GAME.window.draw(pause);
                GAME.window.draw(revealed);
                bool isNegative = counter < 0;
                int absCounter = abs(counter);
                int hundredsPlace = absCounter / 100;
                int tensPlace = (absCounter % 100) / 10;
                int onesPlace = absCounter % 10;
                if (isNegative)
                {
                    sf::Sprite negativeSign;
                    negativeSign.setTexture(*texturesMap.textures["digits"]);
                    negativeSign.setTextureRect(sf::IntRect((21 * 10), 0, 21, 32));
                    negativeSign.setPosition(12, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(negativeSign);
                }
                sf::Sprite hundredsDigit;
                hundredsDigit.setTexture(*texturesMap.textures["digits"]);
                hundredsDigit.setTextureRect(sf::IntRect((21 * hundredsPlace), 0, 21, 32));
                hundredsDigit.setPosition(33 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(hundredsDigit);
                sf::Sprite tensDigit;
                tensDigit.setTexture(*texturesMap.textures["digits"]);
                tensDigit.setTextureRect(sf::IntRect((21 * (isNegative || counter >= 10 ? tensPlace : 0)), 0, 21, 32));
                tensDigit.setPosition(54 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(tensDigit);
                sf::Sprite onesDigit;
                onesDigit.setTexture(*texturesMap.textures["digits"]);
                onesDigit.setTextureRect(sf::IntRect((21 * onesPlace), 0, 21, 32));
                onesDigit.setPosition(75 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(onesDigit);
                for(int i = 0; i < GAME.bombs.size(); i++){
                    GAME.window.draw(GAME.bombs[i]);
                }
                for (int i = 0; i < GAME.numbers.size(); i++){
                    GAME.window.draw(GAME.numbers[i]);
                }
                for(int i = 0; i < GAME.spriteMineField.size(); i++){
                    for(int j = 0; j < GAME.spriteMineField[i].size(); j++){
                        GAME.window.draw(GAME.spriteMineField[i][j].tile);
                    }
                }
                for(int i = 0; i < GAME.flags.size(); i++){
                    GAME.window.draw(GAME.flags[i]);
                }
                auto end = chrono::high_resolution_clock ::now();
                auto timeElapsed = chrono::duration_cast<chrono::seconds>(end - start);
                timeElapsed -= timePointWhenUnpaused;
                auto oneMinute = chrono::seconds(60);
                auto one = chrono::seconds(1);
                auto minutesElapsed = timeElapsed / oneMinute;
                int minutesTimer = minutesElapsed;
                int tensPlaceMinutes = floor(minutesTimer / 10);
                int onesPlaceMinutes = minutesTimer % 10;
                sf::Sprite firstMinuteDigit;
                firstMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstMinuteDigit.setTextureRect(sf::IntRect((21 * tensPlaceMinutes), 0, 21, 32));
                firstMinuteDigit.setPosition((GAME.col * 32) - 97, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstMinuteDigit);
                sf::Sprite secondMinuteDigit;
                secondMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondMinuteDigit.setTextureRect(sf::IntRect((21 * onesPlaceMinutes), 0, 21, 32));
                secondMinuteDigit.setPosition((GAME.col * 32) - 76, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondMinuteDigit);
                auto secondsElapsed = (timeElapsed % oneMinute) / one;
                int secondsTimer = secondsElapsed;
                int tensPlaceSeconds = floor(secondsTimer / 10);
                sf::Sprite firstSecondDigit;
                firstSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstSecondDigit.setTextureRect(sf::IntRect((21 * tensPlaceSeconds), 0, 21, 32));
                firstSecondDigit.setPosition((GAME.col * 32) - 54, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstSecondDigit);
                int onesPlaceSeconds = floor(secondsTimer % 10);
                sf::Sprite secondSecondDigit;
                secondSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondSecondDigit.setTextureRect(sf::IntRect((21 * onesPlaceSeconds), 0, 21, 32));
                secondSecondDigit.setPosition((GAME.col * 32) - 33, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondSecondDigit);
                GAME.window.display();
            }
            else if (debugButton){
                GAME.window.clear();
                GAME.window.draw(gameBackground);
                GAME.window.draw(happyFace);
                GAME.window.draw(debug);
                GAME.window.draw(leaderBoardButton);
                GAME.window.draw(play);
                GAME.window.draw(pause);
                GAME.window.draw(revealed);
                bool isNegative = counter < 0;
                int absCounter = abs(counter);
                int hundredsPlace = absCounter / 100;
                int tensPlace = (absCounter % 100) / 10;
                int onesPlace = absCounter % 10;
                if (isNegative)
                {
                    sf::Sprite negativeSign;
                    negativeSign.setTexture(*texturesMap.textures["digits"]);
                    negativeSign.setTextureRect(sf::IntRect((21 * 10), 0, 21, 32));
                    negativeSign.setPosition(12, 32 * (GAME.row + 0.5) + 16);
                    GAME.window.draw(negativeSign);
                }
                sf::Sprite hundredsDigit;
                hundredsDigit.setTexture(*texturesMap.textures["digits"]);
                hundredsDigit.setTextureRect(sf::IntRect((21 * hundredsPlace), 0, 21, 32));
                hundredsDigit.setPosition(33 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(hundredsDigit);
                sf::Sprite tensDigit;
                tensDigit.setTexture(*texturesMap.textures["digits"]);
                tensDigit.setTextureRect(sf::IntRect((21 * (isNegative || counter >= 10 ? tensPlace : 0)), 0, 21, 32));
                tensDigit.setPosition(54 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(tensDigit);
                sf::Sprite onesDigit;
                onesDigit.setTexture(*texturesMap.textures["digits"]);
                onesDigit.setTextureRect(sf::IntRect((21 * onesPlace), 0, 21, 32));
                onesDigit.setPosition(75 , 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(onesDigit);
                for (int i = 0; i < GAME.numbers.size(); i++){
                    GAME.window.draw(GAME.numbers[i]);
                }
                for(int i = 0; i < GAME.spriteMineField.size(); i++){
                    for(int j = 0; j < GAME.spriteMineField[i].size(); j++){
                        GAME.window.draw(GAME.spriteMineField[i][j].tile);
                    }
                }
                for(int i = 0; i < GAME.bombs.size(); i++){
                    GAME.window.draw(GAME.bombs[i]);
                }
                for(int i = 0; i < GAME.flags.size(); i++){
                    GAME.window.draw(GAME.flags[i]);
                }
                auto end = chrono::high_resolution_clock ::now();
                auto timeElapsed = chrono::duration_cast<chrono::seconds>(end - start);
                timeElapsed -= timePointWhenUnpaused;
                auto oneMinute = chrono::seconds(60);
                auto one = chrono::seconds(1);
                auto minutesElapsed = timeElapsed / oneMinute;
                int minutesTimer = minutesElapsed;
                int tensPlaceMinutes = floor(minutesTimer / 10);
                int onesPlaceMinutes = minutesTimer % 10;
                sf::Sprite firstMinuteDigit;
                firstMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstMinuteDigit.setTextureRect(sf::IntRect((21 * tensPlaceMinutes), 0, 21, 32));
                firstMinuteDigit.setPosition((GAME.col * 32) - 97, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstMinuteDigit);
                sf::Sprite secondMinuteDigit;
                secondMinuteDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondMinuteDigit.setTextureRect(sf::IntRect((21 * onesPlaceMinutes), 0, 21, 32));
                secondMinuteDigit.setPosition((GAME.col * 32) - 76, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondMinuteDigit);
                auto secondsElapsed = (timeElapsed % oneMinute) / one;
                int secondsTimer = secondsElapsed;
                int tensPlaceSeconds = floor(secondsTimer / 10);
                sf::Sprite firstSecondDigit;
                firstSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                firstSecondDigit.setTextureRect(sf::IntRect((21 * tensPlaceSeconds), 0, 21, 32));
                firstSecondDigit.setPosition((GAME.col * 32) - 54, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(firstSecondDigit);
                int onesPlaceSeconds = floor(secondsTimer % 10);
                sf::Sprite secondSecondDigit;
                secondSecondDigit.setTexture(*GAME.texturesMap.textures["digits"]);
                secondSecondDigit.setTextureRect(sf::IntRect((21 * onesPlaceSeconds), 0, 21, 32));
                secondSecondDigit.setPosition((GAME.col * 32) - 33, 32 * (GAME.row + 0.5) + 16);
                GAME.window.draw(secondSecondDigit);
                GAME.window.display();
            }
        }
    }
    return 0;
}