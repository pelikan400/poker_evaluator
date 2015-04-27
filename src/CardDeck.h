#ifndef CARD_DECK_H
#define CARD_DECK_H

#include <vector>
#include <string>
#include <random>

//////////////////////////////////////////////////////////////////////////////////////////

enum HandRank {
   HIGH_CARD,
   ONE_PAIR,
   TWO_PAIR,
   THREE_OF_A_KIND,
   STRAIGHT,
   FLUSH,
   FULL_HOUSE,
   FOUR_OF_A_KIND,
   STRAIGHT_FLUSH
};


enum CardRank {
   DEUCE = 0,
   TREY,
   FOUR,
   FIVE,
   SIX, 
   SEVEN,
   EIGHT,
   NINE,
   TEN,
   JACK,
   QUEEN,
   KING,
   ACE
};

enum CardSuit {
   CLUB = 0,
   DIAMOND,
   HEART,
   SPADE
};

//////////////////////////////////////////////////////////////////////////////////////////

class Card {
private:
   static unsigned int primes[];
   static std::string ranksAsString[];
   static std::string suitesAsString[];

   unsigned int raw_;
   unsigned int index_;
   mutable std::string asString_;

public:
   Card();
   Card( unsigned int index );
   Card( CardRank rank, CardSuit suit );

   inline unsigned int raw() const { return raw_; }
   inline unsigned int index() const { return index_; }
   const std::string& toString() const;
};

//////////////////////////////////////////////////////////////////////////////////////////


class Hand {
public:
   typedef std::vector< Card > Cards;

 private:
   Cards cards_;

public:
   Hand();
   Hand( const Cards& cards );
   Hand( std::initializer_list< Card > cardList );
   inline const Cards& cards() const { return cards_; }
   std::string toString() const;
   void add( const Card& card );
};

//////////////////////////////////////////////////////////////////////////////////////////

#define CARDS_IN_DECK 52

class CardDeck {
private:
   bool dealedCards_[ CARDS_IN_DECK ];
   bool lockedCards_[ CARDS_IN_DECK ];
   static Card cardDeck_[ CARDS_IN_DECK ];
   static bool cardsInitialized_;

   std::mt19937 generator_;
   std::uniform_int_distribution< unsigned int> distribution_;

private: 
   unsigned int randomCardIndex();
   void generateCardsForDeck();

public:
   CardDeck();
   void clean();
   void cleanAll();

   const Card& dealCard();
   const Card& dealCard( const unsigned int cardIndex );
   const Card& lockCard( const unsigned int cardIndex );
   const Card& lockCard( const std::string& cardAsString );
};



#endif
