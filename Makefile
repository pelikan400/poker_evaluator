#
#
#

pokerEvaluator: PokerFiveHandEvaluatorArrays.cc PokerFiveHandEvaluator.cc PokerFiveHandEvaluator.h
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ -std=c++11 -stdlib=libc++ PokerFiveHandEvaluator.cc PokerFiveHandEvaluatorArrays.cc

