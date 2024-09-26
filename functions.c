# include "functions.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

static char _result[40];

typedef uint32_t array32_t[9];
typedef uint8_t array8_t[36];

static const array32_t zero32_t = {0, 0, 0, 0, 0, 0, 0, 0, 0};

typedef union
{
    array32_t a32;
    array8_t a8;
    __uint128_t v;
} array_t;

static int add(const char *first, const char *second);
static int sub(const char *first, const char *second);
static int mul(const char *first, const char *second);
static int div(const char *first, const char *second);

int performAction(enum EAction action, const char *first, const char *second, char **result)
{
    int res;
    switch (action)
    {
    case Action_Add:
        res = add(first, second);
        break;

    case Action_Sub:
        res = sub(first, second);
        break;

    case Action_Mul:
        res = mul(first, second);
        break;

    case Action_Div:
        res = div(first, second);
        break;

    default:
        res = 0;
        strcpy(_result, second);
        break;
    }
    *result = _result;
    return res;
}

inline static uint8_t _char_to_byte(char c)
{
    return (((c) >= 'A' ? (uint8_t)(((c) - 'A') + 10) : (uint8_t)((c) - '0')) & 0xF);
}

inline static uint8_t char_to_byte(const char *s)
{
    return (_char_to_byte(s[0]) << 4) | _char_to_byte(s[1]);
}

static void convert_parameter(const char *s, array8_t result)
{
    char ss[24 + 8 + 1];
    int i;
    const char *p = s, *pe = s + strlen(s);

    memset(result, 0, sizeof(array8_t));
    for (i = 0; p <= pe; ++i, ++p)
        if (p == pe || *p == ',')
        {
            int j = 0;
            char *c = ss;

            for (; j < (24 - i); ++c, ++j)
                *c = '0';
            if (i <= 24)
                for (p = s; p < (s + i); ++p, ++c)
                    *c = *p;
            if (++p < pe)
            {
                for (j = 0; p < pe && j < 8; ++p, ++c, ++j)
                    *c = *p;
            }
            else
                j = 0;
            for (i = 0; i < (8 - j); ++i, ++c)
                *c = '0';
            *c = 0;
            break;
        }
    for (i = 0, p = ss + (strlen(ss) - 2); p >= ss; p -= 2, ++i)
        result[i] = char_to_byte(p);
}


static void back_convert_parameter(char minus, array8_t rs, char *result)
{
    char *p = result;
    int i, j;
    int started = 0;

    if (minus)
        *p++ = minus;
    for (i = 15; i >= 4; --i)
    {
        if (rs[i])
        {
            if (!started)
            {
                started = 1;
                sprintf(p, "%X", rs[i]);
                p += strlen(p);
                continue;
            }
        }
        if (started)
        {
            sprintf(p, "%02X", rs[i]);
            p += strlen(p);
        }
    }
    if (!started)
        *p++ = '0';
    *p++ = ',';
    *p = 0;
    for (i = 0; i < 4; ++i)
        if (rs[i])
            break;
    if (i < 4)
        for (j = 3; j >= i; --j)
        {
            sprintf(p, "%02X", rs[j]);
            p += strlen(p);
        }
    for (p = result + (strlen(p) - 1); *p != ','; --p)
        if (*p == '0')
            *p = 0;
        else
            break;
}

static int add1(const char *first, const char *second, char *result)
{
    array8_t rf, rs;
    int i;
    uint16_t c = 0;

    convert_parameter(first, rf);
    convert_parameter(second, rs);
    for (i = 0; i < 16; ++i)
    {
        uint16_t r = (uint16_t)rf[i] + (uint16_t)rs[i] + c;

        c = r >> 8;
        rf[i] = (uint8_t)r;
    }
    back_convert_parameter(0, rf, result);
    return c ? 1 : 0;
}

static int sub1(array8_t first, array8_t second)
{
    int i;
    uint16_t c = 0;

    for (i = 0; i < 16; ++i)
    {
        uint16_t f = (uint16_t)first[i], s = (uint16_t)second[i], c1 = (f >= (s + c) ? 0 : 1);

        if (c1)
        {
            f += 0x100;
            f -= s + c;
        }
        else
            f -= s + c;
        c = c1;
        first[i] = (uint8_t)f;
    }
    return c ? 1 : 0;
}

static int add(const char *first, const char *second)
{
    int res;
    char rs[38];
    char *rss = _result;

    if (first[0] == '-' && second[0] != '-')
        // -a + b = b - a
        return sub(second, first + 1);
    if (first[0] != '-' && second[0] == '-')
        // a + (-b) = a - b
        return sub(first, second + 1);
    // -a + (-b) = -(a + b)
    // a + b = a + b
    res = add1(first + (first[0] == '-' ? 1 : 0), second + (second[0] == '-' ? 1 : 0), rs);
    rss[0] = first[0] == '-' ? '-' : 0;
    rss += first[0] == '-' ? 1 : 0;
    strcpy(rss, rs);
    return res;
}

static int sub(const char *first, const char *second)
{
    int res;
    char rs[38];
    char *rsss = _result;
    array8_t rsf, rss;

    if (first[0] == '-' && second[0] != '-')
    {
        // -a - b == -(a + b)
        res = add1(first + 1, second, rs);
        *rsss++ = '-';
        strcpy(rsss, rs);
        return res;
    }
    if (first[0] != '-' && second[0] == '-')
    {
        // a - (-b) = a + b
        res = add1(first, second + 1, rs);
        strcpy(rsss, rs);
        return res;
    }
    if (first[0] == '-')
        // -a - (-b) = b - a
        return sub(second + 1, first + 1);
    // a - b
    convert_parameter(first, rsf);
    convert_parameter(second, rss);
    if (sub1(rsf, rss))
    {
        memcpy(rss, rsf, sizeof(rss));
        convert_parameter("0,", rsf);
        sub1(rsf, rss);
        back_convert_parameter('-', rsf, _result);
    }
    else
        back_convert_parameter(0, rsf, _result);
    return 0;
}

static int mul(const char *first, const char *second)
{
    array_t rf, rs;
    int i, j;
    uint32_t addition = 0;
    bool ressign = false;
    array32_t ar[4];

    memset(ar, 0, sizeof(ar));
    if ((first[0] == '-' && second[0] != '-') || (first[0] != '-' && second[0] == '-'))
        ressign = true;
    convert_parameter(first + (first[0] == '-' ? 1 : 0), rf.a8);
    convert_parameter(second + (second[0] == '-' ? 1 : 0), rs.a8);
    if (memcmp(rf.a32, zero32_t, sizeof(array32_t)) == 0 || memcmp(rs.a32, zero32_t, sizeof(array32_t)) == 0)
    {
        strcpy(_result, "0,");
        return 0;
    }
    for (j = 0; j < 4; ++j)
        for (i = 0; i < 4; ++i)
        {
            uint64_t f = (uint64_t)rf.a32[i], s = (uint64_t)rs.a32[j], r = f * s + addition;

            addition = (uint32_t)(r >> 32);
            (ar[j])[j + i] = (uint32_t)r;
            if (i == 3 && addition)
            {
                (ar[j])[j + i + 1] = addition;
                addition = 0;
            }
        }
    memset(rf.a32, 0, sizeof(rf.a32));
    for (i = 0; i < 9; ++i)
        for (j = 0; j < 4; ++j)
            rf.a32[i] += (ar[j])[i];
    for (i = 0; i < 8; ++i)
    {
        rf.a32[i] = rf.a32[i + 1];
        rf.a32[i + 1] = 0;
    }
    back_convert_parameter(ressign ? '-' : 0, rf.a8, _result);
    addition = 0;
    for (i = 4; i < 9; ++i)
        addition |= rf.a32[i];
    return addition ? 1 : 0;
}

static int array_length(array8_t ar)
{
    int i;
    for (i = sizeof(array8_t) - 1; i >= 0; --i)
        if (ar[i])
            return i;
    return 0;
}

static void insert_array(array8_t ar, uint8_t what, int index)
{
    int i;

    if (index >= 0 && index < (int)sizeof(array8_t))
    {
        if (index == ((int)sizeof(array8_t) - 1))
        {
            ar[index] = what;
            return;
        }
        for (i = sizeof(array8_t) - 2; i >= index; --i)
        {
            ar[i + 1] = ar[i];
            if (i == index)
            {
                ar[i] = what;
                return;
            }
        }
    }
}

static int div(const char *first, const char *second)
{
    array_t rf, rs, res, frv, nfrv;
    int i = 0, j = 15;
    bool ressign = false;

    if ((first[0] == '-' && second[0] != '-') || (first[0] != '-' && second[0] == '-'))
        ressign = true;
    convert_parameter(first + (first[0] == '-' ? 1 : 0), rf.a8);
    convert_parameter(second + (second[0] == '-' ? 1 : 0), rs.a8);
    if (memcmp(rs.a32, zero32_t, sizeof(array32_t)) == 0)
    {
        char *p = _result;
        if (ressign)
            *p++ = '-';
        strcpy(p, "FFFFFFFFFFFFFFFFFFFFFFFF,FFFFFFFF");
        return -1;
    }
    if (memcmp(rf.a32, zero32_t, sizeof(array32_t)) == 0)
    {
        strcpy(_result, "0,");
        return 0;
    }
    res.v = rf.v / rs.v;
    for (i = 0; i < 4; ++i)
        insert_array(res.a8, 0, 0);
    memset(frv.a8, 0, sizeof(array8_t));
    memset(nfrv.a8, 0, sizeof(array8_t));
    frv.v = rf.v % rs.v;
    i = 4;
    while (frv.v)
    {
        frv.v *= 256;
        if (--i < 0)
            break;
        nfrv.v = frv.v;
        frv.v /= rs.v;
        if ((nfrv.v - frv.v * rs.v) == 0)
        {
            res.a8[i] = frv.v;
            break;
        }
        frv.v = nfrv.v;
    }
    back_convert_parameter(ressign ? '-' : 0, res.a8, _result);
    return 0;
}
