#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

int itr = 0;
int depth = 0;

class MyStack {
   public:
    void Push(int n) { return _stack.push(n); }
    int Top() {
        if (_stack.size() > 0) {
            return _stack.top();
        }
        return -999;
    }
    int Pop() {
        int n = _stack.top();
        _stack.pop();
        return n;
    }

   private:
    std::stack<int> _stack;
};
MyStack g_stack;

string RemoveSpace(const string& eq) {
    string ans;
    for (char c : eq) {
        if (c == ' ' || c == '\t') continue;
        ans += c;
    }
    return ans;
}
string ToUpper(const string& eq) {
    string ans;
    for (char c : eq) {
        if ('a' <= c && c <= 'z') {
            ans += c + 'A' - 'a';
        } else {
            ans += c;
        }
    }
    return ans;
}

int Evaluate(const string& eq);
int Expr(const string& eq);
int Term15(const string& eq);  // a||b
int Term14(const string& eq);  // a&&b
int Term11(const string& eq);  // a&b
int Term10(const string& eq);  // a==b, a!=b
int Term03(const string& eq);  // !a
int Term01(const string& eq);  // (a)
int Term00(const string& eq);  // number
int calc(const string& eq);
// int Factor(const string& eq);
// int OneOpe(const string& eq);
// int Number(const string& eq);

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Argument error\nUsage) test.exe \"equation\" \n");
        return 0;
    }

    const string tmp = RemoveSpace(argv[1]);
    const string eq = ToUpper(tmp);
    int ans = calc(eq);

    fprintf(stdout, "%d\n", ans);

    return 0;
}

int calc(const string& eq) {
    fprintf(stderr, "\nEQ: %s\n", eq.c_str());
    itr = 0;
    Expr(eq);
    int ans = g_stack.Pop();
    return ans;
}

void debug(const string& eq, const string& t) {
    string str;
    for (int i = 0; i < depth; i++) {
        str += "    ";
    }

    fprintf(stderr, "%s%s(dep[%d], itr(%d)=[%c], stack=[%d])\n", str.c_str(),
            t.c_str(), depth, itr, eq[itr] ? eq[itr] : ' ', g_stack.Top());
}

int Expr(const string& eq) {
    depth++;
    debug(eq, "Expr start");
    Term15(eq);
    debug(eq, "Expr End");
    depth--;
    return 1;
}

int Term15(const string& eq) {
    depth++;
    debug(eq, "Term15 start");
    Term14(eq);
    while (eq[itr]) {
        if (eq[itr] == '|' && eq[itr + 1] == '|') {
            itr += 2;
            int l = g_stack.Pop();
            Term14(eq);
            int r = g_stack.Pop();
            g_stack.Push(l || r);
        } else {
            break;
        }
    }

    debug(eq, "Term15 End");
    depth--;
    return 1;
}

// a && b
int Term14(const string& eq) {
    depth++;
    debug(eq, "Term14 start");
    Term11(eq);
    while (eq[itr]) {
        if (eq[itr] == '&' && eq[itr + 1] == '&') {
            itr += 2;
            int l = g_stack.Pop();
            Term11(eq);
            int r = g_stack.Pop();
            g_stack.Push(l && r);
        } else {
            break;
        }
    }

    debug(eq, "Term14 End");
    depth--;
    return 1;
}

// a & b
int Term11(const string& eq) {
    depth++;
    debug(eq, "Term11 start");
    Term10(eq);
    while (eq[itr]) {
        if (eq[itr] == '&' && eq[itr + 1] != '&') {
            itr++;
            int l = g_stack.Pop();
            Term10(eq);
            int r = g_stack.Pop();
            g_stack.Push(l & r);
        } else {
            break;
        }
    }
    debug(eq, "Term11 End");
    depth--;

    return 1;
}

// a == b, a != b
int Term10(const string& eq) {
    depth++;
    debug(eq, "Term10 start");

    Term03(eq);
    while (eq[itr]) {
        if (eq[itr] == '=' && eq[itr + 1] == '=') {
            itr += 2;
            int l = g_stack.Pop();
            Term03(eq);
            int r = g_stack.Pop();
            g_stack.Push(l == r);
        } else if (eq[itr] == '!' && eq[itr + 1] == '=') {
            itr += 2;
            int l = g_stack.Pop();
            Term03(eq);
            int r = g_stack.Pop();
            g_stack.Push(l != r);
        } else {
            break;
        }
    }
    debug(eq, "Term10 End");
    depth--;

    return 1;
}

// !a, +a, -a
int Term03(const string& eq) {
    depth++;
    debug(eq, "Term03 start");

    while (eq[itr]) {
        if (eq[itr] == '!' && eq[itr + 1] != '=') {
            itr++;
            Term01(eq);
            int t = g_stack.Pop();
            g_stack.Push(!t);
        } else if (eq[itr] == '+') {
            itr++;
            Term01(eq);
            int t = g_stack.Pop();
            g_stack.Push(+t);
        } else if (eq[itr] == '-') {
            itr++;
            Term01(eq);
            int t = g_stack.Pop();
            g_stack.Push(-t);
        } else {
            Term01(eq);
            break;
        }
    }

    debug(eq, "Term03 End");
    depth--;

    return 1;
}

// (a)
int Term01(const string& eq) {
    depth++;
    debug(eq, "Term01 start");

    while (eq[itr]) {
        if (eq[itr] == '(') {
            itr++;
            Expr(eq);
        } else {
            Term00(eq);
            break;
        }
        if (eq[itr] == ')') itr++;
    }
    debug(eq, "Term01 End");
    depth--;
    return 1;
}

inline bool IsHex(char c) {
    return ('A' <= c && c <= 'F') || ('0' <= c && c <= '9');
}
inline bool IsNum(char c) { return '0' <= c && c <= '9'; }

// Number
int Term00(const string& eq) {
    depth++;
    debug(eq, "Term00 start");

    int size = 0;
    if (eq[itr] == '0' && eq[itr + 1] == 'X') {
        size += 2;
        while (IsHex(eq[itr + size])) size++;
        int t = std::stoi(eq.substr(itr, size), nullptr, 16);
        g_stack.Push(t);
    } else {
        while (IsNum(eq[itr + size])) size++;
        int t = std::atoi(eq.substr(itr, size).c_str());
        g_stack.Push(t);
    }
    itr += size;

    debug(eq, "Term00 almost end");
    if (eq[itr] == ')') itr++;

    debug(eq, "Term00 End");
    depth--;
    return 1;
}
