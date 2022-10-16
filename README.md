# Fusion

# An interpretted C like programming language

To install the runtime for linux, download https://github.com/KickdeGans/Fusion/blob/main/install.sh.

How to install:
```C
sudo chmod +x ./install.sh
sudo ./install.sh
```
Then follow the instructions in the installer.

To verify if the runtime is installed, simply run:
```C
fusion --verify
```

# Examples:

## Printing text:
```C#
print("Hello world!"); //Prints without a newline
println("Hello world!"); //Prints with a newline
```

## Declaring a variables:
```C#
//Variables assigned in scope:

var myVariable = "something"; //Normal variable
const var myVariable = "something"; //Const variable

//Public / global variables:

public myPublicVariable = "something"; //Normal variable
const public myPublicVariable = "something"; //Public variable

```

## Functions:
```C#
//A function that doesn't take arguments:

define myFunc()
{
    ...
};

//A function that does take arguments:

define myFunc(argument)
{
    ...
};

//A function that returns something:

define myFunc()
{
    return "This value returned from a function!";
};
```

## Statements:
```C#
//An if statement:
if (condition)
{
    ...
};
//An else if statement:
elseif (condition)
{
    ...
};
//An else statement:
else
{
    ...
};
```

## Loops:
```C#
//A while loop:
while (condition)
{
    ...
};
//A until loop:
until (condition)
{
    ...
};
//A do while loop:
dowhile (condition)
{
    ...
};
//A do until loop:
dountil (condition)
{
    ...
};
//A for loop:
for (variableName => source)
{
    ...
};
```


This project is a fork of https://github.com/sebbekarlsson/hello
