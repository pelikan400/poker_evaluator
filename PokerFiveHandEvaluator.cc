// Paul D. Senzee's Optimized Hand Evaluator
//   for Cactus Kev's Poker Hand Evaluator
//
// Replaces binary search with a perfect hash.
// If you replace eval_5hand with eval_5hand_fast, the products[] and values[] arrays in 'arrays.h' are unnecessary.
// With eval_5hand_fast, the 'allfive.c' test program runs about 2.7 times faster.
//
// (c) Paul D. Senzee.
// Portions (in eval_5hand_fast) (c) Kevin L. Suffecool.
// psenzee@yahoo.com
//
// Senzee 5
// http://senzee.blogspot.com

 


// TODO 
// - programm the card deck dealer
// - compute all cards  
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <future>
#include <memory>

#include "PokerFiveHandEvaluator.h"

Card::Card()
{
}

//////////////////////////////////////////////////////////////////////////////////////////

Card::Card( const std::string& x )
{
}

//////////////////////////////////////////////////////////////////////////////////////////

Card::Card( unsigned int index )
{
   index_ = index;
   CardRank rank = (CardRank) ( index >> 2 );
   CardSuit suit = (CardSuit) ( index & 0x03 );
   raw_ = primes[ rank ] | ( rank << 8 ) | ( 1 << ( suit + 12 ) ) | ( 1 << (16 + rank ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

Card::Card( CardRank rank, CardSuit suit )
{
  index_ = ( rank << 2 ) + suit;
  raw_ = primes[ rank ] | ( rank << 8 ) | ( 1 << ( suit + 12 ) ) | ( 1 << (16 + rank ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

const std::string& Card::toString() const
{
  if( asString_.empty() ) {
    CardRank rank = (CardRank) ( index_ >> 2 );
    CardSuit suit = (CardSuit) ( index_ & 0x03 );
    asString_ = ranksAsString[ rank ] + suitesAsString[ suit ];
  }

  return asString_;
}

//////////////////////////////////////////////////////////////////////////////////////////

Hand::Hand()
{
}

//////////////////////////////////////////////////////////////////////////////////////////

void Hand::add( const Card& card )
{
  cards_.push_back( card );
}

//////////////////////////////////////////////////////////////////////////////////////////

Hand::Hand( std::initializer_list< Card > cardList )
{
   for( Card c : cardList ) {
      cards_.push_back( c );
   }
}

//////////////////////////////////////////////////////////////////////////////////////////

std::string Hand::toString() const
{
  std::string handToString; 
  for( const Card& card : cards() ) {
    handToString += card.toString() + " ";
  }

  return handToString;
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int CardDeck::randomCardIndex()
{
  return distribution_( generator_ );
}

//////////////////////////////////////////////////////////////////////////////////////////

void CardDeck::generateCardsForDeck()
{
  for( int i = 0; i < CARDS_IN_DECK; ++i ) {
    Card c( i );
    cardDeck_[ i ] = c;
    lockedCards_[ i ] = false;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////

CardDeck::CardDeck()
  : generator_( std::chrono::system_clock::now().time_since_epoch().count() ),
    distribution_( 0, CARDS_IN_DECK - 1 )
{
  generateCardsForDeck();
  cleanAll();
}

//////////////////////////////////////////////////////////////////////////////////////////

void CardDeck::clean()
{
  for( int i = 0; i < CARDS_IN_DECK; ++i ) {
    dealedCards_[ i ] = lockedCards_[ i ];
  }
}

//////////////////////////////////////////////////////////////////////////////////////////

void CardDeck::cleanAll()
{
  for( int i = 0; i < CARDS_IN_DECK; ++i ) {
    lockedCards_[ i ] = false;
  }
  clean();
}

//////////////////////////////////////////////////////////////////////////////////////////

Card& CardDeck::dealCard( const unsigned int cardIndex )
{
  if( dealedCards_[ cardIndex ] ) {
    throw std::logic_error( "Card was allready dealed." );
  }
  dealedCards_[ cardIndex ] = true;
  return cardDeck_[ cardIndex ];
}

//////////////////////////////////////////////////////////////////////////////////////////

Card& CardDeck::dealCard()
{
  unsigned int cardIndex = randomCardIndex();
  int watchdogCounter = 0;
  while( dealedCards_[ cardIndex ] == true && watchdogCounter < 5 ) {
    cardIndex = randomCardIndex();
    ++watchdogCounter;
  } 

  if( watchdogCounter < 5 ) {
    return dealCard( cardIndex );
  }

  cardIndex = ( cardIndex + 1 ) % CARDS_IN_DECK;
  watchdogCounter = 0;
  while( dealedCards_[ cardIndex ] == true && watchdogCounter < CARDS_IN_DECK ) {
    cardIndex = ( cardIndex + 1 ) % CARDS_IN_DECK;
    ++watchdogCounter;
  } 

  if( watchdogCounter >= CARDS_IN_DECK ) {
    throw std::runtime_error( "No more cards left in deck." );
  }

  return dealCard( cardIndex );
}

//////////////////////////////////////////////////////////////////////////////////////////

Card& CardDeck::lockCard( const unsigned int cardIndex )
{
  lockedCards_[ cardIndex ] = true;
  return dealCard( cardIndex );
}

//////////////////////////////////////////////////////////////////////////////////////////

Card& CardDeck::lockCard( const std::string& cardAsString )
{
  // TODO: scratch this dummy and implement it right
  return dealCard( 0 );
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int PokerFiveHandEvaluator::find_fast( unsigned int u ) const
{
    unsigned a, b, r;
    u += 0xe91aaa35;
    u ^= u >> 16;
    u += u << 8;
    u ^= u >> 4;
    b  = (u >> 8) & 0x1ff;
    a  = ( u + (u << 2 ) ) >> 19;
    r  = a ^ hash_adjust[ b ];
    return r;
}

//////////////////////////////////////////////////////////////////////////////////////////

std::string PokerFiveHandEvaluator::evaluateToString( const unsigned int val ) const
{
  HandRank rank = HIGH_CARD; 
  if( val > 6185 ) {
    rank = HIGH_CARD;        // 1277 high card
  }
  else if( val > 3325 ) {
    rank = ONE_PAIR;         // 2860 one pair
  } 
  else if( val > 2467 ) {
    rank = TWO_PAIR;         //  858 two pair
  }
  else if( val > 1609 ) {
    rank = THREE_OF_A_KIND;  //  858 three-kind
  }
  else if( val > 1599 ) {
    rank = STRAIGHT;         //   10 straights
  }
  else if( val > 322 )  {
    rank = FLUSH;            // 1277 flushes
  }
  else if( val > 166 )  {
    rank = FULL_HOUSE;       //  156 full house
  }
  else if ( val > 10 ) {
    rank = FOUR_OF_A_KIND;   //  156 four-kind
  }   
  else {
    rank = STRAIGHT_FLUSH;                   //   10 straight-flushes
  }

  return ranksAsString[ rank ];
}

//////////////////////////////////////////////////////////////////////////////////////////

std::string PokerFiveHandEvaluator::evaluateToString( const Hand& hand ) const
{
  return evaluateToString( evaluate( hand ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int PokerFiveHandEvaluator::evaluate( const Hand& hand ) const
{
   unsigned int q = 0;
   unsigned int f = 0xf000;
   unsigned int v = 1;
   for( const Card& card : hand.cards() ) {
      const unsigned int& rawCard = card.raw();
      q |= rawCard;
      f &= rawCard;
      v *= rawCard  & 0xff;
   }
   q = q >> 16;

   if( f ) {
      return flushes[ q ]; // check for flushes and straight flushes
   }

   unsigned short s = unique5[ q ];
   if( s ) {
      return s;
   }
   
   return hash_values[ find_fast( v ) ];
}


//////////////////////////////////////////////////////////////////////////////////////////

unsigned int PokerFiveHandEvaluator::evaluateHandWithCommonCards( const PermutationMatrix& permutationMatrix, 
								  const Hand& holeCards, const Hand& commonCards ) const
{
  unsigned int bestValue = 9999;

  for( auto row : permutationMatrix ) {
    Hand h;
    auto indexHoleCards = row.first;
    auto indexCommonCards = row.second;
    for( unsigned int index : indexHoleCards ) {
      h.add( holeCards.cards()[ index ] );
    }
    for( unsigned int index : indexCommonCards ) {
      h.add( commonCards.cards()[ index ] );
    }
    unsigned int handValue = evaluate( h );
    if( handValue < bestValue ) {
      bestValue = handValue;
    }
  }

  // std::cout << holeCards.toString() << " " << commonCards.toString();
  // std::cout << " :  " << evaluateToString( bestValue ) << std::endl;
  
  return bestValue;
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int PokerFiveHandEvaluator::evaluateHoldemHand( const Hand& holeCards, const Hand& commonCards ) const
{
  return evaluateHandWithCommonCards( permutationHoldem, holeCards, commonCards );
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int PokerFiveHandEvaluator::evaluateOmahaHand( const Hand& holeCards, const Hand& commonCards ) const
{
  return evaluateHandWithCommonCards( permutationOmaha, holeCards, commonCards );
}

#define MAX_MONTE_CARLO_SIMULATIONS  10000

//////////////////////////////////////////////////////////////////////////////////////////

float playHoldemWithFixedHoleCards( std::shared_ptr< PokerFiveHandEvaluator > evaluator, 
				    std::shared_ptr< CardDeck > deck,
				    std::shared_ptr< Hand > holeCards, 
				    int numberOfOpponents )
{
  unsigned int looseCounter = 0;

  for( int i = 0; i < MAX_MONTE_CARLO_SIMULATIONS; ++i ) {
    Hand commonCards = { deck->dealCard(), deck->dealCard(), deck->dealCard(), deck->dealCard(), deck->dealCard() };
    unsigned int handValue = evaluator->evaluateHoldemHand( *holeCards, commonCards );
    for( int j = 0; j < numberOfOpponents; ++j ) {
      Hand opponentHoleCards = { deck->dealCard(), deck->dealCard() };
      unsigned int opponentHandValue = evaluator->evaluateHoldemHand( opponentHoleCards, commonCards );
      if( handValue > opponentHandValue ) {
	++looseCounter;
	break;
      }
    }

    deck->clean();
  }

  
  return 1.0 - (float) looseCounter / (float) MAX_MONTE_CARLO_SIMULATIONS;
}

//////////////////////////////////////////////////////////////////////////////////////////

void playHoldem( int numberOfOpponents )
{
   std::shared_ptr< PokerFiveHandEvaluator > evaluator( new PokerFiveHandEvaluator() );

   for( int i = 0; i < 13; ++i ) {
      for( int j = 0; j < 13; ++j ) {
         if( i != j ) {
            std::shared_ptr< CardDeck > deck( new CardDeck() );
            std::shared_ptr< Hand > h( new Hand( { deck->lockCard( i * 4 ), deck->lockCard( j * 4 ) } ) );
            std::future< float > winningProbabilityFuture = std::async( playHoldemWithFixedHoleCards, evaluator, deck, h, numberOfOpponents );
            // float winningProbability = winningProbabilityFuture.get();
            // float winningProbability = playHoldemWithFixedHoleCards( evaluator, deck, h, numberOfOpponents );
            // std::cout << h->toString() << "   suited " << winningProbability * 100 << "%" << std::endl;
            deck->cleanAll();
         }
         
         {
            std::shared_ptr< CardDeck > deck( new CardDeck() );
            std::shared_ptr< Hand > h( new Hand( { deck->lockCard( i * 4 ), deck->lockCard( j * 4 + 1 ) } ) );
            // float winningProbability = std::async( playHoldemWithFixedHoleCards, evaluator, deck, h, numberOfOpponents ).get();
            // float winningProbability = playHoldemWithFixedHoleCards( evaluator, deck, h, numberOfOpponents );
            // std::cout << h->toString() << ( i == j ? "   Pair   " : " unsuited " ) << winningProbability * 100 << "%" << std::endl;
            deck->cleanAll();
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////

int testCardDeck()
{
   PokerFiveHandEvaluator evaluator;
   CardDeck deck;
   
   for( int deckCounter = 0; deckCounter < 50000; ++deckCounter ) {
      deck.cleanAll();
      Hand h1 = { deck.dealCard(), deck.dealCard(), deck.dealCard(), deck.dealCard(), deck.dealCard() };
      Hand h2 = { deck.dealCard(), deck.dealCard(), deck.dealCard(), deck.dealCard(), deck.dealCard() };
      auto x1 = evaluator.evaluate( h1 );
      auto x2 = evaluator.evaluate( h2 );
      std::cout << h1.toString() ; 
      std::cout << " :  " << evaluator.evaluateToString( h1 ) << std::endl;
      std::cout << h2.toString() ; 
      std::cout << " :  " << evaluator.evaluateToString( h2 ) << std::endl;
      std::cout << ( x1 == x2 ? "Tie: both hands win" : x1 < x2 ? "First hand wins" : "Second hand wins" )  << std::endl << std::endl;
   } 
   
   return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////

int  main()
{
  try {
    playHoldem( 9 );

    // PokerFiveHandEvaluator evaluator;
    // 
    // // throw std::runtime_error( "No more cards left in deck." );
    // 
    // Hand h = { 0, 1, 5, 3, 4 };
    // // Hand h = { Card( "2s" ), Card( "2c" ), Card( "3s" ), Card( "Ts" ), Card( "Ac" ) };
    // 
    // auto x = evaluator.evaluate( h );
    // 
    // std::cout << "Evaluate " << h.toString() << " to: " << evaluator.evaluateToString( h ) << "  " << x << std::endl;
    // testCardDeck();
  } 
  catch( std::exception& ex ) { 
    std::cout << "Catched exception: " << ex.what() << std::endl;
  }
}
