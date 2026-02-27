# Vision Document
**Project Title:** Arithmetic Expression Evaluator in C++

**Version:** 1.0

**Date:** February 2026

# 1. Introduction

## 1.1 Purpose
The goal of this project is to design and implement a simple, accessible arithmetic expression interpreter that evaluates mathematical 
expressions correctly and efficiently. The system is intended to be clear and easy to use.

## 1.2 Project Overview
This project should be able to parse and evaluate arithmetic expressions entered by the user. 
Some examples of supported evaluations include:
- +, -, *, /, %, **
- Operator precedence
- Parentheses
- Unary operators
- Numeric constants

# 2. Problem Statement

## 2.1 Problem
This software is designed to parse and evaluate arithmetic expressions using PEMDAS and associativity rules.
Other necessary features include:
- Accept user input expressions
- Correctly evaluate given expressions
- Handle expressions with parentheses
- Report invalid expressions in a user-friendly way

## 2.2 Importance
This software is necessary to allow a user to enter an arithmetic expression and find the intended result accurately.
The user should be able to see the input mistakes they have made and be given the chance to try again.

# 3. Stakeholders
### Professor Hossein Saiedian
- Instructor
### Toye Oloko
- Teaching Assistant
### Development Team
- Designers, Implementers, Testers, and Documenters in the group

# 4. Key Features

## 4.1 Expression Parsing
- Tokenize the input expression
- Handle unary and binary operators
- Handle parentheses in evaluation
  
## 4.2 Operator Precedence
- Follows the PEMDAS rules
- Uses associativity rules
  
## 4.3 Evaluation Rules
- +, -, *, /, % uses left-to-right evaluation
- ** uses right-to-left evaluation
- Parentheses are used as default precedence
  
## 4.4 Error Handling
- Division by zero
- Invalid Expression: missing operands, missing parentheses, etc.
- Clear and easy to follow report of errors
  
## 4.5 User Interface
- Command-line input and output
- Easy to understand prompts, results, and errors

# 5. Assumptions and Criteria

## 5.1 Assumptions
- Coded in C++
- Uses object-oriented principles
- Follows UPEDU process

## 5.2 Criteria
The project will succeed if:
- Expressions can be accurately evaluated based on PEMDAS
- All errors are handled
- User-friendly and easy to use command-line
- Complete documentation throughout the project
- Full testing of the software before being finalized 
