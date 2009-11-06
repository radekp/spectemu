/* 
 * Copyright (C) 1996-2004 Szeredi Miklos
 * Email: mszeredi@inf.bme.hu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See the file COPYING. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_TOK_NUM 256

typedef struct {
    int num;
    int type;
} Token;

#define ADD(tokens, i, n, t) \
  tokens[i].num = n; tokens[i].type = t; i++

#define BASEOFF 10

#define I_OPENBRACE  20
#define I_CLOSEBRACE 21
#define I_MINUS      22

#define I_NULL  0

#define T_NUM   1
#define T_NOT   2
#define T_NEG   3
#define T_MUL   4
#define T_DIV   5
#define T_MOD   6
#define T_ADD   7
#define T_SUB   8
#define T_SHL   9
#define T_SHR   10

#define P_NE    3
#define P_MU    2
#define P_AD    1
#define P_SH    0

static int line;
static int pos;

int read_item(char **sp, int *num)
{
    char *s;
    int val, base;
    s = *sp;
  
    base = 10;
    val = 0;

    while(*s == ' ' || *s == '\t') s++;
    if(!*s) {
        *sp = s;
        return I_NULL;
    }
  
    *sp = s+1;
    switch(*s) {
    case '+': return T_ADD;
    case '-': return I_MINUS;
    case '*': return T_MUL;
    case '/': return T_DIV;
    case '%': return T_MOD;
    case '~': return T_NOT;
    case '(': return I_OPENBRACE;
    case ')': return I_CLOSEBRACE;
    case '<':
        s++;
        if(*s != '<') return -1;
        *sp = s+1;
        return T_SHL;
    case '>':
        s++;
        if(*s != '>') return -1;
        *sp = s+1;
        return T_SHR;
    case '0':
        s++;
        if(*s == 'x') {
            s++;
            if(!*s) return -1;
            base = 16;
        }
        else base = 8;
        break;
    default:
        if(*s < '0' || *s > '9') {
            *sp = s;
            return I_NULL;
        }
    }
  
    for(;;) {
        if((*s >= '0' && *s <= '7') || 
           (base > 8 && *s >= '8' && *s <= '9')) val = val * base + (*s - '0');
        else if(base > 10 && 
                ((*s >= 'a' && *s <= 'f') ||
                 (*s >= 'A' && *s <= 'F'))) {
            if(*s < 'a') val = val * base + (*s - 'A' + 10);
            else         val = val * base + (*s - 'a' + 10);
        }
        else break;
    
        s++;
    }

    *num = val;
    *sp = s;
    return T_NUM;
}

int tokenize(char **exps, Token *tokens)
{
    int op;
    int i;
    int res, num;
    int base;

    op = 1;
    base = 0;

    for(i = 0;;) {
        res = read_item(exps, &num);
        if(res < 0) return res;
    
        switch(res) {
        case I_NULL:
            if(op || base != 0) return -1;
            return i;
        case T_NUM:
            if(!op) return -1;
            ADD(tokens, i, num, res);
            op = 0;
            break;
        case I_OPENBRACE:
            if(!op) {
                (*exps)--;
                if(op || base != 0) return -1;
                return i;
            }
            base += BASEOFF;
            /*    op = 1;               */
            break;
        case I_CLOSEBRACE:
            if(op) return -1;
            base -= BASEOFF;
            if(base < 0) return -1;
            /*    op = 0;               */
            break;
        case T_NOT:
            if(!op) return -1;
            ADD(tokens, i, base+P_NE, res);
            /*    op = 1;               */
            break;
        case T_SHL:
        case T_SHR:
            if(op) return -1;
            ADD(tokens, i, base+P_SH, res);
            op = 1;
            break;
        case T_ADD:
            if(op) return -1;
            ADD(tokens, i, base+P_AD, res);
            op = 1;
            break;
        case I_MINUS:
            if(!op) {
                ADD(tokens, i, base+P_AD, T_SUB);
                op = 1;
            }
            else {
                ADD(tokens, i, base+P_NE, T_NEG);
                /*      op = 1;             */
            }
            break;
        case T_MUL:
        case T_DIV:
        case T_MOD:
            if(op) return -1;
            ADD(tokens, i, base+P_MU, res);
            op = 1;
            break;
        default:
            return -1;
        }
    }
    return -1;
}

int calculate(Token *tokens, int start, int end, int *valp)
{
    int i;
    int minprec;
    int minpos;
    int lval, rval;
    int val;
    int res;

    if(start == end - 1) {
        *valp = tokens[start].num;
        return 0;
    }
  
    val = 0;
    minprec = -1;
    minpos = -1;
    for(i = start; i < end; i++) 
        if(tokens[i].type != T_NUM && (minprec < 0 || tokens[i].num <= minprec)) {
            minprec = tokens[i].num;
            minpos = i;
        }
  
    if(tokens[start].type != T_NUM && minprec == tokens[start].num) {
        res = calculate(tokens, start+1, end, &rval);
        if(res < 0) return res;
    
        if(tokens[start].type == T_NOT) val = ~rval;
        else                            val = -rval;
        *valp = val;
        return 0;
    }
  
    res = calculate(tokens, start, minpos, &lval);
    if(res < 0) return res;
    res = calculate(tokens, minpos+1, end, &rval);
    if(res < 0) return res;
  
    switch(tokens[minpos].type) {
    case T_SHL: 
        if(rval < 0) return -1;
        val = lval << rval; break;
    case T_SHR: 
        if(rval < 0) return -1;
        val = lval >> rval; break;
    case T_ADD: val = lval + rval; break;
    case T_SUB: val = lval - rval; break;
    case T_MUL: val = lval * rval; break;
    case T_DIV: val = lval / rval; break;
    case T_MOD: val = lval % rval; break;
    }

    *valp = val;
    return 0;
}

int cexpr(char **exps, int *val)
{
    static Token tokens[MAX_TOK_NUM];
    int res;

    res = tokenize(exps, tokens);
    if(res < 0) return res;
    if(res == 0) return -1;

    res = calculate(tokens, 0, res, val);
    if(res < 0) return res;

    return 0;
}


int mygetc(FILE *f)
{
    int c;
  
    c = getc(f);
    pos++;
    if(c == '\n') {
        line++;
        pos = 0;
    }
    return c;
}


int ignore_whitespaces(FILE *in)
{
    int c;

    while((c = mygetc(in)) != EOF && (c == ' ' || c == '\t'));
  
    if(c == EOF) return -1;
    else {
        ungetc(c, in);
        if(c == '\n') line--;
        if(pos > 0) pos--;
        return 0;
    }
}

int read_asm_line(FILE *in, char *buf)
{
    int c;
    int is_label = 0;
    int li = 0;

    if(ignore_whitespaces(in) == -1) return -1;

    while((c = mygetc(in)) != EOF && c != ';' && c != '\n') {
        if(c == ':') is_label = 1;
        buf[li] = c;
        li++;
    }
    buf[li] = '\0';

    if(c == EOF) return -1;
    else return is_label;
}

void put_label(char *buf, FILE *out)
{
    putc('\n', out);
    for(;*buf && *buf != ':';buf++) 
        if(*buf != ' ' && *buf != '\t') putc(*buf, out);
    if(*buf == ':') putc(*buf, out), buf++;
    for(;*buf; buf++) 
        if(*buf != ' ' && *buf != '\t') {
            fprintf(stderr, "Error: instruction after label at line %i, pos %i\n",
                    line, pos);
            exit(1);
        }
    putc('\n', out);
}


void put_asm(char *buf, FILE *out)
{
    int l;
    int lastb;
    int isc;

    lastb = 0;

    putc('\t', out);
    for(l=0;*buf && *buf != ' ' && *buf != '\t';buf++,l++) {
        putc(*buf, out);
        lastb = *buf;
    }

    putc(' ', out);
    for(;l < 7; l++) putc(' ', out);
    for(;*buf && *buf != '#';) {
        if(*buf == '+' || *buf == '$') {
            int val;
            int res;

            if(*buf == '$') isc = 1;
            else isc = 0;

            putc(*buf, out);
            buf++;
      
            res = cexpr(&buf, &val);
            if(res < 0) {
                fprintf(stderr, 
                        "Error: illegal arithmetic expression at line %i, pos %i\n",
                        line, pos);
                exit(1);
            }
      
            if(!isc) fprintf(out, "%i", val);
            else {
                if(lastb == 'w') val = val & 0xFFFF;
                else if(lastb == 'b') val = val & 0xFF;

                fprintf(out, "%i", val);
            }
        }
        else {
            if(*buf != ' ' && *buf != '\t') putc(*buf, out);
            buf++;
        }
    }
    putc('\n', out);
}

int main(void)
{
    FILE *in = stdin;
    FILE *out = stdout;
    static char buf[1024];
    int res;

    line = 0;
    pos = 0;


    while((res = read_asm_line(in, buf)) != -1) {
        if(buf[0] != '\0' && buf[0] != '#') {
            if(res == 0) put_asm(buf, out);
            else put_label(buf, out);
        }
    }
  
    return 0;
}
