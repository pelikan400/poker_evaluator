#ifndef POKER_FIVE_HAND_EVALUATOR_H
#define POKER_FIVE_HAND_EVALUATOR_H

#include <vector>
#include <string>
#include <random>
#include "CardDeck.h"

//////////////////////////////////////////////////////////////////////////////////////////

typedef std::vector< std::pair< std::vector< unsigned int >, std::vector< unsigned int > > >  PermutationMatrix;

//////////////////////////////////////////////////////////////////////////////////////////

class FiveCardEvaluator {
private:
   static unsigned short hash_adjust[];
   static unsigned short hash_values[];
   static unsigned short unique5[];
   static unsigned short flushes[];
   static std::string ranksAsString[];
   static PermutationMatrix permutationHoldem;
   static PermutationMatrix permutationOmaha;
   unsigned int find_fast( unsigned int u ) const;

   unsigned int evaluateHandWithCommonCards( const PermutationMatrix& permutationMatrix, 
					     const Hand& holeCards, const Hand& commonCards ) const;
   

public:
   unsigned int evaluate( const Hand& hand ) const;
   std::string evaluateToString( const Hand& hand ) const;
   std::string evaluateToString( const unsigned int val ) const;

   unsigned int evaluateHoldemHand( const Hand& holeCards, const Hand& commonCards ) const;
   unsigned int evaluateOmahaHand( const Hand& holeCards, const Hand& commonCards ) const;
};

#endif
