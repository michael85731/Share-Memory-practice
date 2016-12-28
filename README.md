# Homework 01 - Share Memory
These program were writen by only `C`
It's a simple share memory demo.
## How to compile
I use `GCC` to compile these program.
Just type:
```terminal
gcc a.c -o a
```
same as file `b.c`.
## How to execute
Because the program will update share memory synchronize, so you have to execute both program at same time to see the result. It's better you open two terminal to see it.

Execute sequence:
1. b
2. a

b program will generate a share memory for program a to read, and it will continuously accept your input until you send terminal key word.
I will explain the input key word later.

when b program is executing, then execute program a.
a program will output the result that correspond to the input key word you enter at program b.

## Input key word
You can type everything in executing program b, and there's some key word you can keyin to tell program what do.
> The result will print at program a's screen.
- `@`
  This will print all input word you just typein, and result is arranged in alphabetical order.
- `# + word`
  Search command, you can type like:
  ```
  #abc
  ```
  and program a will shows that if you have ever input word `abc`.
- `e`
  Terminate the program both a and b.