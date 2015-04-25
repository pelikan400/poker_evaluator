
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <future>
#include <memory>
#include <map>
#include <unistd.h>

#include "CardDeck.h"

Card::Card()
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

Card CardDeck::cardDeck_[ CARDS_IN_DECK ];
bool CardDeck::cardsInitialized_ = false;

//////////////////////////////////////////////////////////////////////////////////////////

CardDeck::CardDeck()
  : generator_( std::chrono::system_clock::now().time_since_epoch().count() ),
    distribution_( 0, CARDS_IN_DECK - 1 )
{
  if( !cardsInitialized_ ) {
    generateCardsForDeck();
    cardsInitialized_ = true;
  }

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

const Card& CardDeck::dealCard( const unsigned int cardIndex )
{
  if( dealedCards_[ cardIndex ] ) {
    throw std::logic_error( "Card was allready dealed." );
  }
  dealedCards_[ cardIndex ] = true;
  return cardDeck_[ cardIndex ];
}

//////////////////////////////////////////////////////////////////////////////////////////

const Card& CardDeck::dealCard()
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

const Card& CardDeck::lockCard( const unsigned int cardIndex )
{
  lockedCards_[ cardIndex ] = true;
  return dealCard( cardIndex );
}

//////////////////////////////////////////////////////////////////////////////////////////

const Card& CardDeck::lockCard( const std::string&  )
{
  // TODO: scratch this dummy and implement it right
  return dealCard( 0 );
}

