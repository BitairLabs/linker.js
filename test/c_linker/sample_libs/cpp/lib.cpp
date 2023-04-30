double cpp_add(double a, double b)
{
  double result = a + b;
  return result;
}

extern "C"
{
  double add(double a, double b)
  {
    return cpp_add(a, b);
  }
}