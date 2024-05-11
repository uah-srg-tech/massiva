/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mathTools.cpp
 * Author: Aaron
 * 
 * Created on 11 de mayo de 2024, 22:28
 */

#include "MathTools.h"

unsigned long long MathTools::ullpow(unsigned long long base, unsigned int exp)
{
    unsigned long long result = 1ULL;
    while(exp)
    {
        if (exp & 1)
        {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }
    return result;
}