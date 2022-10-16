# Fusion

# An interpretted C like programming language


Hello world example:
```C
define main()
{
    var text = "Hello world!";
    printfn(text);
};
main();
```
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

Keywords:

    define              define a function
    var                 define a variable
    public              define a public variable
    const               define a constant variable
    if                  define an if statement
    else                define an else statement
    elseif              define an else if statement
    while               define a while loop
    until               define an until loop
    dowhile             define a dowhile loop
    dountil             define a dountil loop
    for                 define a for loop
    return              return a value in a function

Tokens:

    identifier name
    =                   equals
    ""                  string
    ;                   semicolon (end of line)
    (                   left parenthesis
    )                   right parenthesis
    {                   left brace
    }                   right brace
    [                   left bracket
    ]                   right bracket
    ,                   comma
    .                   dot
    end of file           
    ?                   conditional
    =>                  array pointer
    number

Boolean operators:
    ==                  equal to
    !=                  not equal to
    >                   greater than
    <                   less than
    >=                  equals greater than
    <=                  equals less than
    !                   not
    &&                  and
    ||                  or

Variable declarations:

    In scope:
        var name = value;
    Public:
        public name = value
    Constant in scope:
        const var name = value;
    Constant public:
        const public name = value;

Function declarations:

    No arguments:
        define name()
        {
            ...
        };
    Arguments:
        define name(argument)
        {
            ...
        };

Using statements:

    Boolean statement:
        (if/else/elseif/while/until/dowhile/dountil) (argument)
        {
            ...
        };
    For statement:
        for (variable_name => source)
        {
            ...
        };

Arrays:

    Normal array:
        var name = {
            "a",
            "b",
            "c"
        };
    Dictionary:
        var name = {
            "a": 1,
            "b": 2,
            "d": 3
        };


This project is a fork of https://github.com/sebbekarlsson/hello
