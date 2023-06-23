#include <iostream>
#include <list>
#include <random>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <time.h>

using namespace std;

int get_random_number(int min, int max) {
    // generate random integer number between certain two numbers
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
    int get_value(){ return value;}
    char get_value_c(){
        // get name of card with values 1 or bigger than 10 (Jack, Queen, King, Ace) 

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
    string get_suit(){
        // return symbols corresponding to each suit (Clubs, Hearts, Diamond, Spades)
        
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
        // prints value of card and symbol of its suit
        
        if (value < 11 && value > 1)
            cout << value;
        else
            cout << get_value_c();
        cout << ' ' << get_suit() << ' ';
        
    }
    int get_points(){
        // return points of a card
        
        if (value > 1 && value <= 10)
            return value;
        if (value == 1)
            return 11;
        return 10;
    }
};
class Deck {
private:
    list<Card> deck;
    int number_of_cards;
    void shuffle1(){
        // perform Fisher–Yates shuffle of deck
        
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
        // perform Perfect shuffle of deck
        
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
        // refill the deck with all cards or each suit
        deck.clear();
        for (int i=1; i<=13; i++){
            for (int j=1; j<=4; j++)
                deck.push_back(Card(i, j));
        }
        this->number_of_cards = 52;
    }
    Card deal_card(){
        // if there is at least one card in deck, deal one and delete it from deck
        
        if (number_of_cards < 1)
            throw "There are no more cards in pack";
        
        auto it = deck.end();
        it--;
        Card c = *it;
        deck.erase(it);
        number_of_cards--;
        return c;
    }
    void shuffle(){
        /*
        shuffle deck of cards
        firstly use random amount of Fisher–Yates shuffles
        then perform random amount of Perfect shuffles
        */
        int n = get_random_number(1, 100);
        for (int i=0; i<n; i++)
            shuffle1();
        n = get_random_number(1, 8); // deck of 52 cards needs 8 perfect shuffles to have same order as at the beginning
        for (int i=0; i<n; i++)
            shuffle2();
    }
    list<Card>::iterator begin(){ 
        // return iterator that points to the beginning of deck
        return deck.begin();}
    list<Card>::iterator end(){
        // return iterator that points to the end of deck
        return deck.end();}
};
class Dealer {
protected:
    list<Card> cards;
    int points;
    int num_aces; // is used when number of points exceed 21 and dealer has Ace
public:
    Dealer (){ 
        this->num_aces = 0;
        this->points = 0;
    };
    void add_card(Card card){
        // add card to dealer cards and count amount of points

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
        /*
        in case dealer has ace and points are exceeding 21, 
        decrease points by 10, since ace can both be 1 or 11
        return amount of points dealer has
        */
        if (points > 21 && num_aces > 0){
            points -= 10;
            num_aces--;
        }
        return points;
    }
    void print_first(){
        // print first card of dealer
        (*cards.begin()).print_card(); }
    void print_cards(){
        // print all cards of dealer
        for (Card c: cards)
            c.print_card();
    }
    void new_game(){
        // delete all old cards from list and set points and number of aces to zero
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
    int get_money(){ 
        // return amount of money of user 
        return money;}
    void gain_money(int bet){ 
        // add betted money
        this->money += bet; }
    void lose_money(int bet){ 
        // subtract betted money
        this->money -= bet; }
    string get_name(){ 
        // return name of user
        return name;}
    static Player create_player(){
        // ask essential information about user and return player with this data
        string name;
        int money;
        cout << "Introduce your name: ";
        cin >> name;
        cout << "Introduce amount of money, " << name << ": ";
        cin >> money;
        return Player (name, money);
    }
};
void lower(string &str) {
    // make string lowercase
    for (char &c : str) 
        c = tolower(c);
}
void game(Player player, Dealer dealer){
    // ask about bets, perform dealing and whole proccess of the game until user stops or has no money

    Deck d;
    int bet;
    string answer;
    bool game = true;
    while (player.get_money() > 0 && game){
        // refill and shuffle deck every game
        d.deck_refill();
        d.shuffle();
        cout << "How much money do you want to bet? ";
        cin >> bet;
        while (bet > player.get_money()){
            cout << player.get_name() << ", you cannot bet more than you have\n";
            cout << "How much money do you want to bet? ";
            cin >> bet;
        }
        // cards are dealt to user and dealer
        player.add_card(d.deal_card());
        dealer.add_card(d.deal_card());
        player.add_card(d.deal_card());
        dealer.add_card(d.deal_card());
        // show user's and dealer's cards information
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
        // when user ask for more cards, proceed the proccess of game until user stops or has no money
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
            // after user stoped, get cards for dealer until he has at least 17 or more than 21 
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
        // inform user about his money amount, stops in case user has no money or does not want to continue
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
}

int main(){
    srand(time(0)); // provide seed value to get every time different random numbers
    Dealer dealer;
    Player player = Player::create_player();
    
    game(player, dealer);

    return 0;
}