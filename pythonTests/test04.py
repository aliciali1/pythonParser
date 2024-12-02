#
# test04.py
#
# a nuPython program with if stmts, pointers
# should show parsing successful but execution not supported yet
print()
print("TEST CASE: test04.py")
print()

s = input('Enter an integer> ')
N = int(s)

if N < 0:
{
  print("negative")
  N = -N
}

p = &N

if N < 0:
{
  print('this should never happen')
}
elif N == 0:
{
  print('zero!')
  N = N + 1
  print(N)
}
else:
{
  *p = 123
}

print(N)

print()
print('DONE')
print()
