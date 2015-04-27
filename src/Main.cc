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

#include "FiveCardEvaluator.h"

#define MAX_MONTE_CARLO_SIMULATIONS  100000

//////////////////////////////////////////////////////////////////////////////////////////

float playHoldemWithFixedHoleCards( std::shared_ptr< FiveCardEvaluator > evaluator, 
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

template< class T >
std::vector< T > solveAllFutures( std::vector< std::future< T > >& futures ) 
{
   std::vector< T > resolvedValues; 
   for( auto& f : futures ) {
      T x = f.get();
      resolvedValues.push_back( x );
   }

   return resolvedValues;
}

//////////////////////////////////////////////////////////////////////////////////////////

void playHoldem( int numberOfOpponents )
{
   std::shared_ptr< FiveCardEvaluator > evaluator( new FiveCardEvaluator() );

   for( int i = 0; i < 13; ++i ) {
      std::vector< std::shared_ptr< Hand > > hands;
      std::vector< float > winningProbabilities;
      std::vector< std::future< float > > futureWinningProbabilities;
      for( int j = i; j < 13; ++j ) {
         if( i != j ) {
            std::shared_ptr< CardDeck > deck( new CardDeck() );
            std::shared_ptr< Hand > h( new Hand( { deck->lockCard( i * 4 ), deck->lockCard( j * 4 ) } ) );
            hands.push_back( h );
            futureWinningProbabilities.push_back( std::async( std::launch::async, playHoldemWithFixedHoleCards, evaluator, deck, h, numberOfOpponents ) );
         }
         
         {
            std::shared_ptr< CardDeck > deck( new CardDeck() );
            std::shared_ptr< Hand > h( new Hand( { deck->lockCard( i * 4 ), deck->lockCard( j * 4 + 1 ) } ) );
            hands.push_back( h );
            futureWinningProbabilities.push_back( std::async( std::launch::async, playHoldemWithFixedHoleCards, evaluator, deck, h, numberOfOpponents ) );
         }
      }

      winningProbabilities = solveAllFutures( futureWinningProbabilities );
      std::vector< std::shared_ptr< Hand > >::const_iterator handIterator = hands.begin();
      for( auto&  winningProbability : winningProbabilities ) {
         std::cout << (*handIterator)->toString() << winningProbability * 100 << "%" << std::endl;
         ++handIterator;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////

int testCardDeck()
{
   FiveCardEvaluator evaluator;
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

namespace {
   // std::map< std::string, std::string > getCommandLineOptions( int argc, char * argv[] ) 
   // {
   //    std::map< std::string, std::string > optionsMap;
   //    return optionsMap;
   // }
}

//////////////////////////////////////////////////////////////////////////////////////////

// int  main( int argc, char * argv[] )
int  main( int , char * [] )
{
  try {
    playHoldem( 9 );

    // FiveCardEvaluator evaluator;
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
