#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  void *data;
  size_t len;
} Buffer;

char *hello()
{
  char * str = "Hello world!";
  char *result = malloc(strlen(str));
  strcpy(result, str);
  return result;
}

double add(double a, double b)
{
  double result = a + b;
  return result;
}

double str_len(char *s)
{
  double result = (double)(strlen(s));
  return result;
}

char *str_join(char *s1, char *s2)
{
  char *result = malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

bool is_true(bool a)
{
  bool result = a == true;
  return result;
}

bool is_even(double a)
{
  bool result = (int)a % 2 == 0;
  return result;
}

double max(Buffer values)
{
  uint16_t max = 0;
  uint16_t val;
  for (size_t i = 0; i < values.len / sizeof(uint16_t); i++)
  {
    val = ((uint16_t *)values.data)[i];
    if (val > max)
      max = val;
  }
  return (double)max;
}

double min(Buffer values)
{
  uint8_t min = ((uint8_t *)values.data)[0];
  uint8_t val;
  for (size_t i = 0; i < values.len / sizeof(uint8_t); i++)
  {
    val = ((uint8_t *)values.data)[i];
    if (val < min)
      min = val;
  }
  return (double)min;
}

Buffer prime_numbers(double limit)
{
  uint16_t *primes = malloc(sizeof(uint16_t) * limit);
  size_t total = 0;
  for (size_t i = 2; i <= limit; i++)
  {
    bool is_prime = true;
    for (size_t j = 2; j < i; j++)
    {
      if (i % j == 0)
      {
        is_prime = false;
        break;
      }
    }
    if (is_prime)
      primes[total++] = (uint16_t)i;
  }

  size_t len = sizeof(uint16_t) * total;
  Buffer result = {primes, len};
  return result;
}