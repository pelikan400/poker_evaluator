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
#include <map>
#include <unistd.h>

#include "FiveOfSevenCardEvaluator.h"

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int FiveOfSevenCardEvaluator::find_fast( unsigned int u ) const
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

std::string FiveOfSevenCardEvaluator::evaluateToString( const unsigned int val ) const
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

std::string FiveOfSevenCardEvaluator::evaluateToString( const Hand& hand ) const
{
  return evaluateToString( evaluate( hand ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int FiveOfSevenCardEvaluator::evaluate( const Hand& hand ) const
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

unsigned int FiveOfSevenCardEvaluator::evaluateHandWithCommonCards( const PermutationMatrix& permutationMatrix, 
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

unsigned int FiveOfSevenCardEvaluator::evaluateHoldemHand( const Hand& holeCards, const Hand& commonCards ) const
{
  return evaluateHandWithCommonCards( permutationHoldem, holeCards, commonCards );
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int FiveOfSevenCardEvaluator::evaluateOmahaHand( const Hand& holeCards, const Hand& commonCards ) const
{
  return evaluateHandWithCommonCards( permutationOmaha, holeCards, commonCards );
}

