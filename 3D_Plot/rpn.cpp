/*
 * rpn.cpp
 *
 * Description:
 * -this class was written in order to be able to take RPN (Reverse Polish)
 * user input for the purpose of evaluation a wide variety of user-specified
 * mathematical expressions.
 *
 * Copyright (C) 2018 Andrew Joseph Bean

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef _WIN32
    double const M_PI = 3.14159265358979323846;
    double const M_PI_2 = 1.57079632679489661923;
    double const M_E = 2.71828182845904523536;
    double const M_LN2 = 0.693147180559945309417;
#endif

#include "rpn.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <ctype.h>
#include <stdio.h>
using namespace std;

int IsNumber(char const * String)
{
    // a '+' or a '-' is allowed at the beginning
    if(*String == '-' || *String == '+') String++;

    int Position;
    int NumPeriods = 0;
    // scan through the following characters that are
    // digits or decimal points
    for(Position = 0; String[Position] == '.'
        ||( (String[Position]<='9')&&(String[Position]>='0') ); Position++)
        NumPeriods += (String[Position] == '.');

    // there should only have been at most 1 decimal point
    // there should also have been at least one digit
    if(NumPeriods>1 || (Position==NumPeriods) ) return 0;

    // from here, either done or there is an exponential part
    String += Position;

    // if done, then the number is good
    if(*String == (char)0 ) return 1;

    // must have the e or E
    if(*String != 'e' && *String != 'E') return 0;

    String++;
    // can have either a - or + in front of the exponent
    if(*String == '-' || *String == '+') String++;
    int NumDigits = 0;

    // count the number of digits after the e, E, -, or +
    while( (*String >='0')&&(*String <='9') ){NumDigits++;String++;}

    // must have non-zero # of digits and end on the terminating zero
    if(NumDigits == 0 || *String != (char)0 ) return 0;

    return 1;
}

double ToNum(string Input)
{
    double ReadNum;
    sscanf(Input.c_str(),"%lf",&ReadNum);
    return ReadNum;
}

class RPN_impl;
typedef void (RPN_impl::*Operation)();

class RPN_Cmd
{
public:
    Operation theOp;
    // at most, one of the following would be used:
    double value;
    double* param;
    int nextPos;
};

struct StringLess
{
    bool operator() (string const & lhs, string const & rhs) const
    {
        return ( lhs.compare(rhs)<0 );
    }
};

/*
 *  Things that can be done:
 *      - push numbers to the stack
 *      - push parameters to stack by string label
 *      - evaluate function of stack data
 *      - jump to location by string label
 *      - (possible) save stack value to a (string named) variable
 *      - (possible) push value of a (string named) variable to the stack
 *
 *  All tokens are one of:
 *      - number
 *      - parameter label
 *      - function
 *      - jump label
 *  *If it is not a number, parameter, or function, it
 *      should be a jump label. However, jump labels
 *      should not have duplicates.
 *
 *  To "compile" a string command list:
 *      - break into list of string tokens
 *      - each token corresponds with a single command evaluation, except for jump labels.
 *      - determine to what command index a jump label refers (it's the command after the label)
 *      - create the list of commands, where each has it's own needed data:
 *          - number push cmd holds the number (double)
 *          - parameter push cmd holds ptr to the number (double*)
 *          - jump cmd holds jmp position for program counter update (int)
 *          - normal commands need no data
 */

/*
 functions to add:
 - 2^
 - q
 - rand
 */

// #define STACK1 Stack[Stack.size()-1]
#define STACK1 (Stack.back())
#define STACK2 Stack[Stack.size()-2]

class RPN_impl
{
public:
    int ProgCounter;
    int Error;
    vector<double> Stack; // starts empty
    vector<RPN_Cmd> Prog; // starts empty
    map<string,double*,StringLess> Params; // starts empty
    map<string,int,StringLess> Labels; // starts empty

    string RawCommands;

    static map<string,Operation,StringLess> const JumpLookup;
    static map<string,Operation,StringLess> create_JumpLookup();
    static map<string,Operation,StringLess> const CmdLookup;
    static map<string,Operation,StringLess> create_CmdLookup();

    // All functions of type 'Operation' :

    // Jumping functions first
    void        JGEZ();
    void        JGZ();
    void        JLEZ();
    void        JLZ();
    void        JMP();
    void        JNZ();
    void        JZ();
    void        LOOP();
    void        LOOPN();
    void        REDUCE();

    // Normal stack operations
    void        DEPTH();
    void        DROP();
    void        DROP2();
    void        DROPN();
    void        DUP();
    void        DUP2();
    void        DUPDUP();
    void        DUPN();
    void        NDUPN();
    void        NIP();
    void        OVER();
    void        PICK();
    void        PICK3();
    void        ROLL();
    void        ROLLD();
    void        ROT();
    void        SWAP();
    void        UNPICK();
    void        UNROT();

    void        ABS();
    void        ACOS();
    void        ACOSH();
    void        ACOT();
    void        ACOTH();
    void        ACSC();
    void        ACSCH();
    void        Add();
    void        AND();
    void        ASEC();
    void        ASECH();
    void        ASIN();
    void        ASINH();
    void        ATAN();
    void        ATAN2();
    void        ATANH();
    void        CBRT();
    void        CEIL();
    void        CLEAR();
    void        COS();
    void        COSH();
    void        COT();
    void        COTH();
    void        CSC();
    void        CSCH();
    void        DEC();
    void        Divide();
    void        ENDL();
    void        EQUAL();
    void        ERF();
    void        ERFC();
    void        EXP();
    void        EXP1();
    void        EXP2();
    void        FACT();
    void        FLOOR();
    void        HALF_PI();
    void        HB();
    void        HYPOT();
    void        INC();
    void        INV();
    void        ISINF();
    void        ISNAN();
    void        LESS();
    void        LESS_EQ();
    void        LGAMMA();
    void        LN2();
    void        LOG_10();
    void        LOG_2();
    void        LOG_E();
    void        MAX();
    void        MIN();
    void        MOD();
    void        MORE();
    void        MORE_EQ();
    void        Multiply();
    void        NAND();
    void        NEG();
    void        NoOp();
    void        NOR();
    void        NOT();
    void        NOT_EQUAL();
    void        ONE();
    void        OR();
    void        PAR();
    void        PI();
    void        POW();
    void        PRINT();
    void        PRINTN();
    void        QFUNC();
    void        QINV();
    void        SEC();
    void        SECH();
    void        SIGN();
    void        SIN();
    void        SINBOX();
    void        SINH();
    void        SQRT();
    void        SQUARE();
    void        STOP();
    void        Subtract();
    void        TAN();
    void        TANH();
    void        TEN_POW();
    void        TRIANGLE();
    void        TWO_PI();
    void        TWO_POW();
    void        XNOR();
    void        XOR();
    void        XROOT();
    void        ZERO();


    // Push operations
    void            PushNum();
    void            GetParam();

    RPN_impl(void);
    RPN_impl(RPN_impl const &);
    ~RPN_impl(void);
    RPN_impl&       operator=(RPN_impl const &);
    double &        operator[](int);
    double const &  operator[](int) const;
    int             Compile(char const *);
    int             Push(double);
    int             NewParameter(char const *,double*);
    void            ClearParameters();
    void            ResetStack(void);
    int             GetDepth(void) const;
    int             Eval(void);
    int             operator()(void);
};

map<string,Operation,StringLess> const RPN_impl::JumpLookup = RPN_impl::create_JumpLookup();
map<string,Operation,StringLess> const RPN_impl::CmdLookup = RPN_impl::create_CmdLookup();

map<string,Operation,StringLess> RPN_impl::create_JumpLookup()
{
    map<string,Operation,StringLess> m;
    m["jgez"] = &RPN_impl::JGEZ;
    m["jgz"] = &RPN_impl::JGZ;
    m["jlez"] = &RPN_impl::JLEZ;
    m["jlz"] = &RPN_impl::JLZ;
    m["jmp"] = &RPN_impl::JMP;
    m["jnz"] = &RPN_impl::JNZ;
    m["jz"] = &RPN_impl::JZ;
    m["loop"] = &RPN_impl::LOOP;
    m["loopn"] = &RPN_impl::LOOPN;
    m["reduce"]= &RPN_impl::REDUCE;
    return m;
}


map<string,Operation,StringLess> RPN_impl::create_CmdLookup()
{
    map<string,Operation,StringLess> m;
    m["depth"]      = &RPN_impl::DEPTH;
    m["drop"]       = &RPN_impl::DROP;
    m["drop2"]      = &RPN_impl::DROP2;
    m["dropn"]      = &RPN_impl::DROPN;
    m["dup"]        = &RPN_impl::DUP;
    m["dup2"]       = &RPN_impl::DUP2;
    m["dupdup"]     = &RPN_impl::DUPDUP;
    m["dupn"]       = &RPN_impl::DUPN;
    m["ndupn"]      = &RPN_impl::NDUPN;
    m["nip"]        = &RPN_impl::NIP;
    m["over"]       = &RPN_impl::OVER;
    m["pick"]       = &RPN_impl::PICK;
    m["pick3"]      = &RPN_impl::PICK3;
    m["roll"]       = &RPN_impl::ROLL;
    m["rolld"]      = &RPN_impl::ROLLD;
    m["rot"]        = &RPN_impl::ROT;
    m["swap"]       = &RPN_impl::SWAP;
    m["unpick"]     = &RPN_impl::UNPICK;
    m["unrot"]      = &RPN_impl::UNROT;

    m["abs"]        = &RPN_impl::ABS;
    m["acos"]       = &RPN_impl::ACOS;
    m["acosh"]      = &RPN_impl::ACOSH;
    m["acot"]       = &RPN_impl::ACOT;
    m["acoth"]      = &RPN_impl::ACOTH;
    m["acsc"]       = &RPN_impl::ACSC;
    m["acsch"]      = &RPN_impl::ACSCH;
    m["add"]        = &RPN_impl::Add;
    m["+"]          = &RPN_impl::Add;
    m["and"]        = &RPN_impl::AND;
    m["asec"]       = &RPN_impl::ASEC;
    m["asech"]      = &RPN_impl::ASECH;
    m["asin"]       = &RPN_impl::ASIN;
    m["asinh"]      = &RPN_impl::ASINH;
    m["atan"]       = &RPN_impl::ATAN;
    m["atan2"]      = &RPN_impl::ATAN2;
    m["atanh"]      = &RPN_impl::ATANH;
    m["cbrt"]       = &RPN_impl::CBRT;
    m["ceil"]       = &RPN_impl::CEIL;
    m["clear"]      = &RPN_impl::CLEAR;
    m["cos"]        = &RPN_impl::COS;
    m["cosh"]       = &RPN_impl::COSH;
    m["cot"]        = &RPN_impl::COT;
    m["coth"]       = &RPN_impl::COTH;
    m["csc"]        = &RPN_impl::CSC;
    m["csch"]       = &RPN_impl::CSCH;
    m["dec"]        = &RPN_impl::DEC;
    m["--"]         = &RPN_impl::DEC;
    m["div"]        = &RPN_impl::Divide;
    m["/"]          = &RPN_impl::Divide;
    m["endl"]       = &RPN_impl::ENDL;
    m["=="]         = &RPN_impl::EQUAL;
    m["erf"]        = &RPN_impl::ERF;
    m["erfc"]       = &RPN_impl::ERFC;
    m["exp"]        = &RPN_impl::EXP;
    m["e"]          = &RPN_impl::EXP1;
    m["exp(1)"]     = &RPN_impl::EXP1;
    m["exp(2)"]     = &RPN_impl::EXP2;
    m["fact"]       = &RPN_impl::FACT;
    m["!"]          = &RPN_impl::FACT;
    m["floor"]      = &RPN_impl::FLOOR;
    m["pi/2"]       = &RPN_impl::HALF_PI;
    m["hb"]         = &RPN_impl::HB;
    m["hypot"]      = &RPN_impl::HYPOT;
    m["inc"]        = &RPN_impl::INC;
    m["++"]         = &RPN_impl::INC;
    m["inv"]        = &RPN_impl::INV;
    m["isinf"]      = &RPN_impl::ISINF;
    m["isnan"]      = &RPN_impl::ISNAN;
    m["<"]          = &RPN_impl::LESS;
    m["<="]         = &RPN_impl::LESS_EQ;
    m["lgamma"]     = &RPN_impl::LGAMMA;
    m["ln2"]        = &RPN_impl::LN2;
    m["log"]        = &RPN_impl::LOG_10;
    m["log10"]      = &RPN_impl::LOG_10;
    m["lg"]         = &RPN_impl::LOG_2;
    m["log2"]       = &RPN_impl::LOG_2;
    m["ln"]         = &RPN_impl::LOG_E;
    m["loge"]       = &RPN_impl::LOG_E;
    m["max"]        = &RPN_impl::MAX;
    m["min"]        = &RPN_impl::MIN;
    m["mod"]        = &RPN_impl::MOD;
    m[">"]          = &RPN_impl::MORE;
    m[">="]         = &RPN_impl::MORE_EQ;
    m["mul"]        = &RPN_impl::Multiply;
    m["*"]          = &RPN_impl::Multiply;
    m["nand"]       = &RPN_impl::NAND;
    m["neg"]        = &RPN_impl::NEG;
    m["noop"]       = &RPN_impl::NoOp;
    m["nor"]        = &RPN_impl::NOR;
    m["not"]        = &RPN_impl::NOT;
    m["!="]         = &RPN_impl::NOT_EQUAL;
    m["one"]        = &RPN_impl::ONE;
    m["n1"]         = &RPN_impl::ONE;
    m["or"]         = &RPN_impl::OR;
    m["par"]        = &RPN_impl::PAR;
    m["pi"]         = &RPN_impl::PI;
    m["pow"]        = &RPN_impl::POW;
    m["^"]          = &RPN_impl::POW;
    m["print"]      = &RPN_impl::PRINT;
    m["printn"]     = &RPN_impl::PRINTN;
    m["q"]          = &RPN_impl::QFUNC;
    m["qfunc"]      = &RPN_impl::QFUNC;
    m["qinv"]       = &RPN_impl::QINV;
    m["sec"]        = &RPN_impl::SEC;
    m["sech"]       = &RPN_impl::SECH;
    m["sign"]       = &RPN_impl::SIGN;
    m["sin"]        = &RPN_impl::SIN;
    m["sinbox"]     = &RPN_impl::SINBOX;
    m["sinh"]       = &RPN_impl::SINH;
    m["sqrt"]       = &RPN_impl::SQRT;
    m["^.5"]        = &RPN_impl::SQRT;
    m["square"]     = &RPN_impl::SQUARE;
    m["sq"]         = &RPN_impl::SQUARE;
    m["^2"]         = &RPN_impl::SQUARE;
    m["stop"]       = &RPN_impl::STOP;
    m["sub"]        = &RPN_impl::Subtract;
    m["-"]          = &RPN_impl::Subtract;
    m["tan"]        = &RPN_impl::TAN;
    m["tanh"]       = &RPN_impl::TANH;
    m["triangle"]   = &RPN_impl::TRIANGLE;
    m["10^"]        = &RPN_impl::TEN_POW;
    m["twopi"]      = &RPN_impl::TWO_PI;
    m["2pi"]        = &RPN_impl::TWO_PI;
    m["2^"]         = &RPN_impl::TWO_POW;
    m["xnor"]       = &RPN_impl::XNOR;
    m["xor"]        = &RPN_impl::XOR;
    m["xroot"]      = &RPN_impl::XROOT;
    m["zero"]       = &RPN_impl::ZERO;
    m["n0"]         = &RPN_impl::ZERO;
    return m;
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

RPN::RPN(void)
{
    impl = new RPN_impl();
}

RPN::RPN(RPN const & other)
{
    impl = new RPN_impl(*other.impl);
}

RPN::~RPN(void)
{
    delete impl;
}

RPN& RPN::operator=(RPN const & other)
{
    if(this == &other) return *this;
    delete impl;
    impl = new RPN_impl(*other.impl);
    return *this;
}

double & RPN::operator[](int Pos)
{
    return (*impl)[Pos];
}

double const & RPN::operator[](int Pos) const
{
    return (*impl)[Pos];
}

int RPN::Compile(char const * Input)
{
    return impl->Compile(Input);
}

int RPN::Push(double Val)
{
    return impl->Push(Val);
}

int RPN::NewParameter(char const * Label,double* Variable)
{
    return impl->NewParameter(Label,Variable);
}

void RPN::ClearParameters()
{
    impl->ClearParameters();
}

void RPN::ResetStack(void)
{
    impl->ResetStack();
}

int RPN::GetDepth(void) const
{
    return impl->GetDepth();
}

int RPN::Eval(void)
{
    return impl->Eval();
}

int RPN::operator()(void)
{
    return impl->Eval();
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////





RPN_impl::RPN_impl(void)
{
    Error = 0;
}

RPN_impl::RPN_impl(RPN_impl const & other) :
Error(other.Error),
Stack(other.Stack),
Prog(other.Prog),
Params(other.Params),
Labels(other.Labels)
{

}

RPN_impl::~RPN_impl(void)
{}

RPN_impl& RPN_impl::operator=(RPN_impl const & other)
{
    Error = other.Error;
    Stack = other.Stack;
    Prog = other.Prog;
    Params = other.Params;
    Labels = other.Labels;
    return *this;
}

double & RPN_impl::operator[](int pos)
{
    return Stack[Stack.size()-1-pos];
}

double const & RPN_impl::operator[](int pos) const
{
    return Stack[Stack.size()-1-pos];
}

int RPN_impl::Compile(char const * Input)
{
    Error = 0;
    // first, construct list of tokens
    vector<string> Tokens;
    RawCommands = string(Input);
    while(*Input)
    {
        int len = 0;
        while( *Input && !isgraph(*Input)) Input++;
        // *Input is now a char with graphical representation, or NULL
        while(isgraph(Input[len])) len++;
        // len=0 => no more tokens
        // else, add the token to the list
        if(len>0)
            Tokens.push_back(string(Input,len));
        Input+=len;
    }

    Prog.clear();
    Labels.clear();
    // analyse the tokens list
    // find all labels
    int CmdNum=0;
    for(unsigned long i=0;i<Tokens.size();i++)
    {
        if(IsNumber(Tokens[i].c_str())) CmdNum++;
        else if(Params.count(Tokens[i])) CmdNum++;
        else if(CmdLookup.count(Tokens[i])) CmdNum++;
        else if(JumpLookup.count(Tokens[i])) { CmdNum++;i++; } // ignore next token, should be label
        else if(Labels.count(Tokens[i])) return (Error=2); // label already exists
        else Labels[Tokens[i]] = CmdNum;
    }
    // verify token after a jump is a label
    for(unsigned long i=0;i<Tokens.size();i++)
    {
        if(JumpLookup.count(Tokens[i])) // found a jump command
        {
            if(i+1 >= Tokens.size()) return (Error=3); // verify there is a following token
            i++;
            if(Labels.count(Tokens[i])==0) return (Error=4); // verify that token is a label
        }
    }
    // Tokens list is a valid program. Fill in the commands list.
    for(unsigned long i=0;i<Tokens.size();i++)
    {
        RPN_Cmd tempCmd;
        if(IsNumber(Tokens[i].c_str()))
        {
            tempCmd.theOp = &RPN_impl::PushNum;
            tempCmd.value = ToNum(Tokens[i]);
            Prog.push_back(tempCmd);
        }
        else if(Params.count(Tokens[i]))
        {
            tempCmd.theOp = &RPN_impl::GetParam;
            tempCmd.param = Params[Tokens[i]];
            Prog.push_back(tempCmd);
        }
        else if(CmdLookup.count(Tokens[i]))
        {
            tempCmd.theOp = CmdLookup.at(Tokens[i]);
            Prog.push_back(tempCmd);
        }
        else if(JumpLookup.count(Tokens[i]))
        {
            tempCmd.theOp = JumpLookup.at(Tokens[i]);
            i++;
            tempCmd.nextPos = Labels[Tokens[i]];
            Prog.push_back(tempCmd);
        }
        else if(Labels.count(Tokens[i]))
        {}
        else // This shouldn't happen ever.
        {
            return (Error=5);
        }
    }
    return Error;
}

int RPN_impl::Push(double number)
{
    Stack.push_back(number);
    return Stack.size();
}

int RPN_impl::NewParameter(char const * newStr,double* newPtr)
{
    // if(Params.count(string(newStr))) return -1;
    Params[string(newStr)] = newPtr;
    return 0;
}

void RPN_impl::ClearParameters()
{
    Params.clear();
}

void RPN_impl::ResetStack(void)
{
    Stack.clear();
}

int RPN_impl::GetDepth(void) const
{
    return Stack.size();
}

int RPN_impl::Eval(void)
{
    if(Error>1)
    {
        cerr<<"cannot execute...\n";
        return Error;
    }
    Error=0;
    ProgCounter = 0;
    while(!Error && (unsigned long)ProgCounter < Prog.size())
    {
        (this->*Prog[ProgCounter].theOp)();
    }
    if(Error)
    {
        cerr << "Execution error...\n";
        cerr << RawCommands << endl;
        cerr << "Error=" << Error << endl;
    }
    return Error;
}

int RPN_impl::operator()(void)
{
    return Eval();
}





////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/***********************************************************\
|***********************************************************|
\***********************************************************/
// This is the section of functions of type Operation
// these are of the form:
//      void RPN_impl::Operation();


// These are loop functions
void RPN_impl::JGEZ()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    double i = STACK1;
    Stack.pop_back();
    if(i>=0)
    {
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::JGZ()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    double i = STACK1;
    Stack.pop_back();
    if(i>0)
    {
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::JLEZ()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    double i = STACK1;
    Stack.pop_back();
    if(i<=0)
    {
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::JLZ()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    double i = STACK1;
    Stack.pop_back();
    if(i<0)
    {
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::JMP()
{
    ProgCounter = Prog[ProgCounter].nextPos;
}
void RPN_impl::JNZ()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    double i = STACK1;
    Stack.pop_back();
    if(i!=0)
    {
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::JZ()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    double i = STACK1;
    Stack.pop_back();
    if(i==0)
    {
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::LOOP()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    if(STACK2>1)
    {
        STACK2--;
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::LOOPN()
{
    if(Stack.size()<1||(long)Stack.size()<(long)STACK1+1||(unsigned long)STACK1<1)
    {
        Error = 1;
        return;
    }
    double pos = STACK1;
    Stack.pop_back();
    if(Stack[Stack.size()-pos]>1)
    {
        Stack[Stack.size()-pos]--;
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}
void RPN_impl::REDUCE()
{
    if(Stack.size()<1||(int)STACK1<2)
    {
        Error = 1;
        return;
    }
    int Amnt = (int)STACK1;
    Stack.pop_back();
    if(Stack.size()>=(unsigned long)Amnt)
    {
        ProgCounter = Prog[ProgCounter].nextPos;
    }
    else
    {
        ProgCounter++;
    }
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//These are stack operations

void RPN_impl::DEPTH()
{
    Stack.push_back(Stack.size());
    ProgCounter++;
}
void RPN_impl::DROP()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::DROP2()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    Stack.pop_back();
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::DROPN()
{
    if(Stack.size()<1||(long)Stack.size()<(long)STACK1+1||(int)STACK1<0)
    {
        Error = 1;
        return;
    }
    int i = (int)STACK1;
    Stack.pop_back();
    Stack.resize(Stack.size()-i);
    ProgCounter++;
}
void RPN_impl::DUP()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    Stack.push_back(STACK1);
    ProgCounter++;
}
void RPN_impl::DUP2()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    Stack.push_back(STACK2);
    Stack.push_back(STACK2);
    ProgCounter++;
}
void RPN_impl::DUPDUP()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    Stack.push_back(STACK1);
    Stack.push_back(STACK1);
    ProgCounter++;
}
void RPN_impl::DUPN()
{
    if(Stack.size()<1||(long)Stack.size()<(long)STACK1+1||(int)STACK1<0)
    {
        Error = 1;
        return;
    }
    int NumCopy = (int)STACK1;
    Stack.pop_back();
    for(int j=0;j<NumCopy;j++)
    {
        Stack.push_back(Stack[Stack.size()-NumCopy]);
    }
    ProgCounter++;
}
void RPN_impl::NDUPN()
{
    if(Stack.size()<2||(int)STACK1<0)
    {
        Error = 1;
        return;
    }
    int copies = (int)STACK1;
    double number = STACK2;
    Stack.pop_back();
    Stack.pop_back();
    for(int i = 0;i<copies;i++)
    {
        Stack.push_back(number);
    }
    Stack.push_back((double)copies);
    ProgCounter++;
}
void RPN_impl::NIP()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = STACK1;
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::OVER()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    Stack.push_back(STACK2);
    ProgCounter++;
}
void RPN_impl::PICK()
{
    if(Stack.size()<1||(long)Stack.size()<(long)STACK1+1||(int)STACK1<1)
    {
        Error = 1;
        return;
    }
    int i = (int)(STACK1);
    STACK1 = Stack[Stack.size()-1-i];
    ProgCounter++;
}
void RPN_impl::PICK3()
{
    if(Stack.size()<3)
    {
        Error = 1;
        return;
    }
    Stack.push_back(Stack[Stack.size()-3]);
    ProgCounter++;
}
void RPN_impl::ROLL()
{
    if(Stack.size()<1||(long)Stack.size()<(long)STACK1+1||(int)STACK1<0)
    {
        Error = 1;
        return;
    }
    int i = (int)STACK1;
    Stack.pop_back();
    double temp = Stack[Stack.size()-i];
    for(int j=i;j>1;j--)
    {
        Stack[Stack.size()-j] = Stack[Stack.size()-j+1];
    }
    STACK1 = temp;
    ProgCounter++;
}
void RPN_impl::ROLLD()
{
    if(Stack.size()<1||(long)Stack.size()<(long)STACK1+1||(int)STACK1<0)
    {
        Error = 1;
        return;
    }
    int i = (int)STACK1;
    Stack.pop_back();
    double temp = STACK1;
    for(int j=1;j<i;j++)
    {
        Stack[Stack.size()-j] = Stack[Stack.size()-j-1];
    }
    Stack[Stack.size()-i] = temp;
    ProgCounter++;
}
void RPN_impl::ROT()
{
    if(Stack.size()<3)
    {
        Error = 1;
        return;
    }
    double i = Stack[Stack.size()-3];
    Stack[Stack.size()-3] = STACK2;
    STACK2 = STACK1;
    STACK1 = i;
    ProgCounter++;
}
void RPN_impl::SWAP()
{
    if(Stack.size()<2)
    {
        Error=1;
        return;
    }
    double i = STACK1;
    STACK1 = STACK2;
    STACK2 = i;
    ProgCounter++;
}
void RPN_impl::UNPICK()
{
    if(Stack.size()<2||(long)Stack.size()<(long)STACK1+2||(int)STACK1<0)
    {
        Error = 1;
        return;
    }
    int Spot = (int)STACK1;
    Stack.pop_back();
    double theValue = (int)STACK1;
    Stack.pop_back();
    Stack[Stack.size()-Spot] = theValue;
    ProgCounter++;
}
void RPN_impl::UNROT()
{
    if(Stack.size()<3)
    {
        Error = 1;
        return;
    }
    double i = STACK1;
    STACK1 = STACK2;
    STACK2 = Stack[Stack.size()-3];
    Stack[Stack.size()-3] = i;
    ProgCounter++;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

// These are mathematical operations

void RPN_impl::ABS()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    if(STACK1<0)
        STACK1 = (-STACK1);
    ProgCounter++;
}
void RPN_impl::ACOS()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = acos(STACK1);
    ProgCounter++;
}
void RPN_impl::ACOSH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = acosh(STACK1);
//  STACK1 = log(STACK1+sqrt(STACK1*STACK1-1));
    ProgCounter++;
}
void RPN_impl::ACOT()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = atan(1/STACK1);
    ProgCounter++;
}
void RPN_impl::ACOTH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/STACK1;
    STACK1 = 0.5*log((1.0+STACK1)/(1.0-STACK1));
    ProgCounter++;
}
void RPN_impl::ACSC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = asin(1/STACK1);
    ProgCounter++;
}
void RPN_impl::ACSCH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/STACK1;
    STACK1 = log(STACK1+sqrt(STACK1*STACK1+1));
    ProgCounter++;
}
void RPN_impl::Add()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 += STACK1;
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::AND()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
#ifdef _WIN32
    STACK2 = (double)((long int)STACK2 & (long int)STACK1);
#else
    STACK2 = (double)((long long)STACK2 & (long long)STACK1);
#endif
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::ASEC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = acos(1/STACK1);
    ProgCounter++;
}
void RPN_impl::ASECH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/STACK1;
    STACK1 = log(STACK1+sqrt(STACK1*STACK1-1));
    ProgCounter++;
}
void RPN_impl::ASIN()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = asin(STACK1);
    ProgCounter++;
}
void RPN_impl::ASINH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = asinh(STACK1);
//  STACK1 = log(STACK1+sqrt(STACK1*STACK1+1));
    ProgCounter++;
}
void RPN_impl::ATAN()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = atan(STACK1);
    ProgCounter++;
}
void RPN_impl::ATAN2()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = atan2(STACK2,STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::ATANH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = atanh(STACK1);
//  STACK1 = 0.5*log((1.0+STACK1)/(1.0-STACK1));
    ProgCounter++;
}
void RPN_impl::CBRT()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = cbrt(STACK1);
    ProgCounter++;
}
void RPN_impl::CEIL()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = ceil(STACK1);
    ProgCounter++;
}
void RPN_impl::CLEAR()
{
    Stack.clear();
    ProgCounter++;
}
void RPN_impl::COS()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = cos(STACK1);
    ProgCounter++;
}
void RPN_impl::COSH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = cosh(STACK1);
    ProgCounter++;
}
void RPN_impl::COT()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/tan(STACK1);
    ProgCounter++;
}
void RPN_impl::COTH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/tanh(STACK1);
    ProgCounter++;
}
void RPN_impl::CSC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/sin(STACK1);
    ProgCounter++;
}
void RPN_impl::CSCH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/sinh(STACK1);
    ProgCounter++;
}
void RPN_impl::DEC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 --;
    ProgCounter++;
}
void RPN_impl::Divide()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 /= STACK1;
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::ENDL()
{
    cout<<endl;
    ProgCounter++;
}
void RPN_impl::EQUAL()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = double(STACK2 == STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::ERF()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = erf(STACK1);
    ProgCounter++;
}
void RPN_impl::ERFC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = erfc(STACK1);
    ProgCounter++;
}
void RPN_impl::EXP()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = exp(STACK1);
    ProgCounter++;
}
void RPN_impl::EXP1()
{
    Stack.push_back(M_E);
    ProgCounter++;
}
void RPN_impl::EXP2()
{
    Stack.push_back(M_E*M_E);
    ProgCounter++;
}
void RPN_impl::FACT()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = exp(lgamma(STACK1+1));
    ProgCounter++;
}
void RPN_impl::FLOOR()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = floor(STACK1);
    ProgCounter++;
}
void RPN_impl::HALF_PI()
{
    Stack.push_back(M_PI_2);
    ProgCounter++;
}
void RPN_impl::HB()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    if(STACK1 == 0 || STACK1 == 1)
    {
        STACK1 = 0;
    }
    else
    {
        STACK1 = (-STACK1*log(STACK1) -
                                 (1-STACK1)*log(1-STACK1))/M_LN2;
    }
    ProgCounter++;
}
void RPN_impl::HYPOT()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = hypot(STACK2 , STACK1);
//  STACK2 = sqrt(STACK2*STACK2 + STACK1*STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::INC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 ++;
    ProgCounter++;
}
void RPN_impl::INV()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1 / STACK1;
    ProgCounter++;
}
void RPN_impl::ISINF()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = isinf(STACK1);
    ProgCounter++;
}
void RPN_impl::ISNAN()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = isnan(STACK1);
    ProgCounter++;
}
void RPN_impl::LESS()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = double(STACK2 < STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::LESS_EQ()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = double(STACK2 <= STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::LGAMMA()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = lgamma(STACK1);
    ProgCounter++;
}
void RPN_impl::LN2()
{
    Stack.push_back(M_LN2);
    ProgCounter++;
}
void RPN_impl::LOG_2()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = log(STACK1)/M_LN2;
    ProgCounter++;
}
void RPN_impl::LOG_10()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = log10(STACK1);
    ProgCounter++;
}
void RPN_impl::LOG_E()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = log(STACK1);
    ProgCounter++;
}
void RPN_impl::MAX()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    if(STACK1>STACK2 || isnan(STACK1)) STACK2=STACK1;
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::MIN()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    if(STACK1<STACK2 || isnan(STACK1)) STACK2=STACK1;
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::MOD()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = fmod(STACK2 , STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::MORE()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = double(STACK2 > STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::MORE_EQ()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = double(STACK2 >= STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::Multiply()
{
    if(Stack.size()<2)
    {
        cerr << "Stack.size() = " << Stack.size() << endl;
        cerr << "STACK1=" << STACK1 << endl;
        Error = 2;
        return;
    }
    STACK2 *= STACK1;
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::NAND()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
#ifdef _WIN32
    STACK2 = (double)~((long int)STACK2 & (long int)STACK1);
#else
    STACK2 = (double)~((long long)STACK2 & (long long)STACK1);
#endif
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::NEG()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = -STACK1;
    ProgCounter++;
}
void RPN_impl::NoOp()
{
    ProgCounter++;
}
void RPN_impl::NOR()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
#ifdef _WIN32
    STACK2 = (double)~((long int)STACK2 | (long int)STACK1);
#else
    STACK2 = (double)~((long long)STACK2 | (long long)STACK1);
#endif
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::NOT()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
#ifdef _WIN32
    STACK1 = (double)~(long int)STACK1;
#else
    STACK1 = (double)~(long long)STACK1;
#endif
    ProgCounter++;
}
void RPN_impl::NOT_EQUAL()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = double(STACK2 != STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::ONE()
{
    Stack.push_back(1.0f);
    ProgCounter++;
}
void RPN_impl::OR()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
#ifdef _WIN32
    STACK2 = (double)((long int)STACK2 | (long int)STACK1);
#else
    STACK2 = (double)((long long)STACK2 | (long long)STACK1);
#endif
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::PAR()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = STACK1*STACK2 / (STACK1+STACK2);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::PI()
{
    Stack.push_back(M_PI);
    ProgCounter++;
}
void RPN_impl::POW()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = pow(STACK2 , STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::PRINT()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    cout << STACK1 ;
    ProgCounter++;
}
void RPN_impl::PRINTN()
{
    if(Stack.size()<1 || Stack.size()<STACK1+1 || STACK1<1)
    {
        Error = 1;
        return;
    }
    cout << Stack[Stack.size()-1-STACK1] ;
    Stack.pop_back();
    ProgCounter++;
}
inline double qfunc(double in){return 0.5*erfc(in/sqrt(2.0));}
void RPN_impl::QFUNC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = qfunc(STACK1);
    ProgCounter++;
}
void RPN_impl::QINV()
{
    if(Stack.size()<1 || STACK1<=0.0 || STACK1 >= 1.0)
    {
        Error = 1;
        return;
    }
    double TEST1 = -1;
    double TEST2 = 1;
    double EST = (TEST2+TEST1)/2.0;
    int ITERS = 0;
    while(qfunc(TEST1)<STACK1) TEST1*=2;
    while(qfunc(TEST2)>STACK1) TEST2*=2;
    do
    {
        ITERS++;
        if(qfunc(EST) > STACK1) TEST1 = EST;
        else if(qfunc(EST) < STACK1) TEST2 = EST;
        else break;
        EST = (TEST2+TEST1)/2.0;
    }while(abs(TEST2-TEST1)>1.0e-10 && ITERS<1000);
    STACK1 = EST;
    ProgCounter++;
}
void RPN_impl::SEC()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/cos(STACK1);
    ProgCounter++;
}
void RPN_impl::SECH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = 1/cosh(STACK1);
    ProgCounter++;
}
void RPN_impl::SIGN()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    if(STACK1>0.0) STACK1 = 1.0;
    else if(STACK1<0.0) STACK1 = -1.0;
    else STACK1 = 0.0;
    ProgCounter++;
}
void RPN_impl::SIN()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = sin(STACK1);
    ProgCounter++;
}

double RPN_SineBox(double x,double width)
{
    if(x<=0.0 || x>=1.0)
        return 0.0;
    if(x<width/2)
        return 0.5-0.5*cos(2 * x / width * M_PI);
    if(x>(1-width/2))
        return 0.5-0.5*cos(2 * (1-x) / width * M_PI);
    return 1.0;
}

void RPN_impl::SINBOX()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = RPN_SineBox(STACK2,STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::SINH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = cosh(STACK1);
    ProgCounter++;
}
void RPN_impl::SQRT()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = sqrt(STACK1);
    ProgCounter++;
}
void RPN_impl::SQUARE()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = STACK1*STACK1;
    ProgCounter++;
}
void RPN_impl::STOP()
{
    ProgCounter=Prog.size();
}
void RPN_impl::Subtract()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 -= STACK1;
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::TAN()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = tan(STACK1);
    ProgCounter++;
}
void RPN_impl::TANH()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = tanh(STACK1);
    ProgCounter++;
}
void RPN_impl::TEN_POW()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = pow(10.0,STACK1);
    ProgCounter++;
}
double RPN_Triangle(double x,double width)
{
    if(x<=0.0 || x>=1.0)
        return 0.0;
    if(x<width/2)
        return (2 * x / width);
    if(x>(1-width/2))
        return (2 * (1-x) / width);
    return 1.0;
}

void RPN_impl::TRIANGLE()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = RPN_Triangle(STACK2,STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::TWO_PI()
{
    Stack.push_back(2*M_PI);
    ProgCounter++;
}
void RPN_impl::TWO_POW()
{
    if(Stack.size()<1)
    {
        Error = 1;
        return;
    }
    STACK1 = exp2(STACK1);
    ProgCounter++;
}
void RPN_impl::XNOR()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
#ifdef _WIN32
    STACK2 = (double)~((long int)STACK2 ^ (long int)STACK1);
#else
    STACK2 = (double)~((long long)STACK2 ^ (long long)STACK1);
#endif
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::XOR()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
#ifdef _WIN32
    STACK2 = (double)((long int)STACK2 ^ (long int)STACK1);
#else
    STACK2 = (double)((long long)STACK2 ^ (long long)STACK1);
#endif
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::XROOT()
{
    if(Stack.size()<2)
    {
        Error = 1;
        return;
    }
    STACK2 = pow(STACK2 , 1/STACK1);
    Stack.pop_back();
    ProgCounter++;
}
void RPN_impl::ZERO()
{
    Stack.push_back(0.0f);
    ProgCounter++;
}

//qwer


















































////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


void RPN_impl::PushNum()
{
    Stack.push_back(Prog[ProgCounter].value);
    ProgCounter++;
}

void RPN_impl::GetParam()
{
    Stack.push_back(*Prog[ProgCounter].param);
    ProgCounter++;
}








