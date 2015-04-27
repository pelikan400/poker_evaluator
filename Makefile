#
#
#

sourceDirectory = src
sourceFiles = $(sourceDirectory)/CardDeck.cc $(sourceDirectory)/FiveCardEvaluatorArrays.cc $(sourceDirectory)/FiveCardEvaluator.cc $(sourceDirectory)/Main.cc
headerFiles = $(sourceDirectory)/FiveCardEvaluator.h $(sourceDirectory)/CardDeck.h $(sourceDirectory)/Main.h
OS_SYSTEM = $(shell uname)

ifeq "$(OS_SYSTEM)" "Linux"
   CC = g++
   CC_OPTS = -g -std=c++11  -Wall -Wextra -pedantic-errors -pthread -I $(sourceDirectory)
else 
   CC = /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
   CC_OPTS = -std=c++11 -stdlib=libc++
endif

pokerEvaluator: $(sourceFiles) $(headerFiles)
	$(CC) $(CC_OPTS) $(sourceFiles)

