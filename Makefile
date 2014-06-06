#
#
#

sourceFiles = PokerFiveHandEvaluatorArrays.cc PokerFiveHandEvaluator.cc
headerFiles = PokerFiveHandEvaluator.h
OS_SYSTEM = $(shell uname)

ifeq "$(OS_SYSTEM)" "Linux"
   CC = g++
   CC_OPTS = -g -std=c++11
else 
   CC = /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
   CC_OPTS = -std=c++11 -stdlib=libc++
endif

pokerEvaluator: $(sourceFiles) $(headerFiles)
	$(CC) $(CC_OPTS) PokerFiveHandEvaluator.cc PokerFiveHandEvaluatorArrays.cc

