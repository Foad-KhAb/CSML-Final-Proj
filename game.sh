#!/bin/bash

rm *.out &>/dev/null
rm *.o &>/dev/null

nasm ASM/R.s -felf64 -o R.o &>/dev/null
nasm ASM/S.s -felf64 -o S.o &>/dev/null
nasm ASM/C.s -felf64 -o C.o &>/dev/null
nasm ASM/G.s -felf64 -o G.o &>/dev/null
nasm ASM/SE.s -felf64 -o SE.o &>/dev/null
nasm ASM/SA.s -felf64 -o SA.o &>/dev/null
nasm ASM/EA.s -felf64 -o EA.o &>/dev/null
nasm ASM/EX.s -felf64 -o EX.o &>/dev/null
nasm ASM/EY.s -felf64 -o EY.o &>/dev/null

g++ main.cpp R.o S.o C.o G.o SE.o SA.o EA.o EX.o EY.o -o game.out -lraylib -no-pie &>/dev/null

echo "Let's play PONG!"

./game.out &>/dev/null

echo ":("

rm *.o &>/dev/null
rm *.out &>/dev/null

sleep 2
