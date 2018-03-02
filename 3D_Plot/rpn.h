/*
 * rpn.h
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

class RPN_impl;

class RPN
{
public:

                    RPN(void);
                    RPN(RPN const &);
                    ~RPN(void);
    RPN&            operator=(RPN const &);

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

private:
    RPN_impl * impl;
};




