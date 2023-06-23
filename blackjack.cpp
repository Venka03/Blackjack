#include <iostream>
#include <list>
#include <random>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <time.h>

using namespace std;

int get_random_number(int min, int max) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

class Card {
private:
    int value;
    int suit;
public:
    Card(){
        value = 0;
        suit = 0;
    }
    Card(int value, int suit) {
        if (suit < 1 || suit > 4)
            throw "Invalid card suit";
        if (value < 1 || value > 13)
            throw "Invalid card value";
        this->suit = suit;
        this->value = value;
    }
    Card(string val, char suit) {
        switch (suit){
            case 'c': case 'C':
                this->suit = 1;
                break;
            case 'h': case 'H':
                this->suit = 2;
                break;
            case 'd': case 'D':
                this->suit = 3;
                break;
            case 's': case 'S':
                this->suit = 4;
                break;
            default:
                throw "Invalid card suit";
        }
        if (isNumber(val)){
            int n = stoi(val);
            if (n < 1 || n > 13)
                throw "Invalid card value";
            this->value = n;
        }
        else {
            switch(val[0]){
                case 'j': case 'J':
                    this->value = 11;
                    break;
                case 'q': case 'Q':
                    this->value = 12;
                    break;
                case 'k': case 'K':
                    this->value = 13;
                    break;
                case 'a': case 'A':
                    this->value = 1;
                    break;
                default:
                    throw "Invalid card value";
            }
        }
    }
    int get_value(){ return value;}
    char get_value_c(){
        switch (value){
            case 11:
                return 'J';
            case 12:
                return 'Q';
            case 13:
                return 'K';
            case 1:
                return 'A';
        }
        return ' ';
    }
    char get_suit_name(){
        switch (suit){
            case 1:
                return 'C';
            case 2:
                return 'H';
            case 3:
                return 'D';
            case 4:
                return 'S';
            default:
                return ' ';
        }
    }
    string get_suit(){
        switch (suit){
            case 1:
                return "♧";
            case 2:
                return "♥";
            case 3:
                return "♢";
            case 4:
                return "♤";
            default:
                return " ";
        }
    }
    void print_card(){
        if (value < 11 && value > 1)
            cout << value;
        else
            cout << get_value_c();
        cout << ' ' << get_suit() << ' ';
        
    }
    void print_card_info(){
        if (value < 11 && value > 1)
            cout << value;
        else
            cout << get_value_c();
        cout << ' ' << get_suit_name() << ' ';
        
    }
    int get_points(){
        if (value > 1 && value < 10)
            return value;
        if (value == 1)
            return 11;
        return 10;
    }
    static bool isNumber(const string& s){
        for (char const &ch : s) {
            if (isdigit(ch) == 0) return false;
        }
        return true;
    }

};

class Deck {
private:
    list<Card> deck;
    int number_of_cards;
    void shuffle1(){
        /*
        Fisher–Yates shuffle
        */
        int k = 0;
        int i = deck.size()-1;
        auto j = deck.begin();
        Card tmp;
        for (auto it = deck.rbegin(); it!=deck.rend(); it++, i--){
            int k = get_random_number(0, i);
            advance(j, k);
            tmp = *it;
            *it = *j;
            *j = tmp;
            j = deck.begin();
        }    
    }
    void shuffle2(){
        /*
        Perfect shuffle
        */
        Card *arr1 = new Card[(number_of_cards+1) / 2];
        Card *arr2 = new Card[number_of_cards / 2];
        int i = 0;
        for (auto it = deck.begin(); it!=deck.end(); it++, i++){
            if (i < (number_of_cards+1)/2)
                arr1[i] = *it;
            else
                arr2[i - number_of_cards / 2] = *it;
        }
        i = 0;
        for (auto it = deck.begin(); it!=deck.end(); it++, i++){
            if (i % 2 == 0)
                *it = arr1[i / 2];
            else
                *it = arr2[(i-1) / 2];
        }
        delete [] arr1;
        delete [] arr2;
    }
public:
    Deck(){
        deck_refill();
    }
    void deck_refill(){
        deck.clear();
        for (int i=1; i<=13; i++){
            for (int j=1; j<=4; j++)
                deck.push_back(Card(i, j));
        }
        this->number_of_cards = 52;
    }
    Card deal_card(){
        if (number_of_cards < 1)
            throw "There are no more cards in pack";
        
        auto it = deck.end();
        it--;
        Card c = *it;
        deck.erase(it);
        number_of_cards--;
        return c;
    }
    void deal_cards(list<Card> &cards, int n){
        if (number_of_cards < n)
            throw "There are no enough cards in pack";
        int i = 0;
        for (auto it = deck.rbegin(); it!=deck.rend(); it++, i++){
            if (i == n) break;
            cards.push_front(*it);
            number_of_cards--;
        }
        auto it = deck.begin();
        advance(it, number_of_cards);
        deck.erase(it, deck.end());
    }
    void shuffle(){
        int n = get_random_number(1, 100);
        for (int i=0; i<n; i++)
            shuffle1();
        n = get_random_number(1, 8); // deck of 52 cards needs 8 perfect shuffles to have same order
        for (int i=0; i<n; i++)
            shuffle2();
    }
    list<Card>::iterator begin(){ return deck.begin();}
    list<Card>::iterator end(){ return deck.end();}
};

class Dealer {
protected:
    list<Card> cards;
    int points;
    int num_aces;
public:
    Dealer (){ 
        this->num_aces = 0;
        this->points = 0;
    };
    void add_card(Card card){
        if (card.get_value() == 1){
            if (points + 11 > 21) points++;
            else {
                points += 11;
                num_aces++;
            }
        }
        else points += card.get_points();
        cards.push_back(card);
    }
    int get_points(){
        if (points > 21 && num_aces > 0){
            points -= 10;
            num_aces--;
        }
        return points;
    }
    void print_first(){ (*cards.begin()).print_card();}
    void print_cards(){
        for (Card c: cards)
            c.print_card();
    }
    void new_game(){
        cards.clear();
        points = 0;
        num_aces = 0;
    }
};

class Player: public Dealer {
private:
    list<Card> cards;
    int money;
    string name;
public:
    Player(string name, int money=100) : Dealer(){
        this->name = name;
        if (money <= 0)
            throw "Amount of money should be positive";
        this->money = money;
    }
    int get_money(){ return money;}
    void gain_money(int bet){ this->money += bet; }
    void lose_money(int bet){ this->money -= bet; }
    string get_name(){ return name;}
};

void lower(string &str) {
    /* 
    make string lowercase
    */
    for (char &c : str) 
        c = tolower(c);
}

int main(){
    srand(time(0));
    Deck d;
    int money, bet;
    string name, answer;
    bool game = true;
    cout << "Introduce your name: ";
    cin >> name;
    cout << "Introduce amount of money, " << name << ": ";
    cin >> money;
    Player player (name, money);
    Dealer dealer;
    
    while (player.get_money() > 0 && game){
        d.deck_refill();
        d.shuffle();
        cout << "How much money do you want to bet? ";
        cin >> bet;
        while (bet > player.get_money()){
            cout << player.get_name() << ", you cannot bet more than you have\n";
            cout << "How much money do you want to bet? ";
            cin >> bet;
        }

        player.add_card(d.deal_card());
        dealer.add_card(d.deal_card());
        player.add_card(d.deal_card());
        dealer.add_card(d.deal_card());
        cout << "You have: ";
        player.print_cards();
        cout << " and " << player.get_points() << " points\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        cout << "Dealer has ";
        dealer.print_first();
        cout << "* *\n";
        cout << "Do you want to take more cards? ";
        cin >> answer;
        lower(answer);
        while (!(answer == "yes" || answer == "no")){
            cout << "There are two options: yes or no\n";
            cout << "Do you want to take more cards? ";
            cin >> answer;
            lower(answer);
        }
        while (answer == "yes" && player.get_points() <= 21){
            this_thread::sleep_for(chrono::milliseconds(500));
            player.add_card(d.deal_card());
            cout << "Now you have ";
            player.print_cards();
            cout << "and " << player.get_points() << " points\n";
            if (player.get_points() <= 21){
                cout << "Do you want to take more cards? ";
                cin >> answer;
                lower(answer);
                while (!(answer == "yes" || answer == "no")){
                    cout << "There are two options: yes or no\n";
                    cout << "Do you want to take more cards? ";
                    cin >> answer;
                    lower(answer);
                }
            }
        }
        if (player.get_points() > 21){
            cout << player.get_name() << ", you lost\n";
            player.lose_money(bet);
        }
        else {
            this_thread::sleep_for(chrono::milliseconds(500));
            cout << "Dealer has ";
            dealer.print_cards();
            cout << "and " << dealer.get_points() << " points\n";
            while (dealer.get_points() < 17){
                this_thread::sleep_for(chrono::milliseconds(500));
                dealer.add_card(d.deal_card());
                cout << "Dealer has ";
                dealer.print_cards();
                cout << "and " << dealer.get_points() << " points\n";
            }
            this_thread::sleep_for(chrono::milliseconds(500));
            if (dealer.get_points() > 21 || player.get_points() > dealer.get_points()){
                cout << player.get_name() << ", you won\n";
                player.gain_money(bet);
            }
            else {
                cout << player.get_name() << ", you lost\n";
                player.lose_money(bet);
            }
        }
        cout << "Your bank is " << player.get_money() << endl;
        if (player.get_money() > 0){
            cout << "Do you want to play more? ";
            cin >> answer;
            lower(answer);
            while (!(answer == "yes" || answer == "no")){
                cout << "There are two options: yes or no\n";
                cout << "Do you want to play more? ";
                cin >> answer;
                lower(answer);
            }
            if (answer != "yes") game = false;
            player.new_game();
            dealer.new_game();
        }
        else {
            cout << "You have lost all your money\n";
            cout << "Goodbye, " << player.get_name() << endl;
            game = false;
        }
    }
    
    return 0;
}