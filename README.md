## signalscript

This is a fork of [a scripting language](http://code.google.com/p/tibiasignal/source/browse/#svn/trunk) that an old friend of mine made back in 2010.

The porject got abandoned, and I made this fork just to get familiar with the code, add some small features, and learn about abstract syntax trees.



## What the language had when Jeremic dropped the project:
* Fully working expressions.
* Conditional if/else blocks.
* Functions.
* All binary operators except modulo, exponent, +=, and -=.
* All unary operators except ++ and --.
* Class parsing but no actual functionality.
* Loop (while and for) parsing but no actual functionality.
* Parsing of break and continue statements but no actual functionality.
* Print() native function but no c-function exports.
* Basic string, number, boolean, and nil types.


## What I've added in this fork:
* Loop functionality (compilation and interpretation) for both while and for loops.
* Functionality (compilation and interpretation) for both break and continue statements.
* Class instance creation (via new operator).
* Instance-level scoping for classes.
* Member function calls for classes (parameters are acting up, though).
* Unary operators ++ and -- (postfix).
* String unescaping to support characters such as "\n" and "\t".
* Many bug fixes with error reporting (std::strings were being passed into printf() as opposed to their .c_str() member calls).
* Some changes to the overall structure of the architecture and virtual processor.
* C++ function exporting
* Switch statements (lexing, parsing, compilation, and interpretation).
* Function keyword.
* Improvements to many binary operators.
* Numeric values which allow decimal places.
* Optimizations / cleanups to the VM code.
* Parsing and AST creation for tables (Currently no compilation or interpretation, though).


## What I want to add before I inevitably get tired of playing with the language:
* Class members abstract the table interface (like in Lua).
* Fix parameter bugs for class member functions.
* Global variables.
* Remaining binary operators (modulo, exponent, +=, and -=).


## Here's what the syntax/usage currently looks like (no class usage here, though, since it's pretty broken):

    function testWhile()
    {
        print("Expect 'haahaahaa'\n ");
        n = 0;
        while (true)
        {
            n = n + 1;
            print("h");

            i = 0;
            while (i < 2)
            {
                i = i + 1;
                print("a");
            }


            if (n == 3)
                break;
            else
                continue;
        }
        print("\n");
    }


    function testFor()
    {
        print("Expect '012345 -- 54321 -- 13579'\n ");
        for (i = 0; i <= 5; i = i + 1)
        {
            print(i);
        }
        print(" -- ");
        for (i = 5; true; i = i - 1)
        {
            print(i);
            if (i == 1)
                break;
        }
        print(" -- ");
        for (i = 1; i < 10; i = i + 1)
        {
            if (i == 2 || i == 4 || i == 6 || i == 8)
                continue;
            print(i);
        }
        print("\n");
    }


    function testIncDec()
    {
        print("Expect '01021 | -1 | 4'\n ");
        var = 0;
        print(var);


        var++;
        print(var);


        var--;
        print(var);


        var++;var++;
        print(var);


        var--;
        print(var);


        print(" | ");


        var--;var--;
        print(var);


        print(" | ");


        val = 1;
        val = val + val++;
        print(val);


        print("\n");
    }


    function testSwitch(value)
    {
        switch (value)
        {
            case 3:
                print("three, ");
            case 2:
                {
                    print("two, ");
                }
            case 1:
                {
                    print("one\n");
                    break;
                }
            default:
            {
                print("other\n");
            }
        }
    }


    function main()
    {
        testWhile();
        testFor();
        testIncDec();


        print("Expect 'one'\n ");
        testSwitch(1);
        print("Expect 'two, one'\n ");
        testSwitch(2);
        print("Expect 'three, two, one'\n ");
        testSwitch(3);
        print("Expect 'other'\n ");
        testSwitch(4);
    }


## Here's the usage from C++:

    FileInput file (name);
    Lexer lexer (file);
    Parser parser (lexer);
    std::shared_ptr<AST> ast = parser.parse_program ();
    Environment env = Environment ();
    Compiler::Compile(env, ast);
    Interpreter interpreter(env);
    interpreter.execute ();

## Here's how you export a C++ function:

    //exported function prototype
    std::shared_ptr<Object> printFunc(Environment env, std::vector<std::shared_ptr<Object>> args);

    //call this before Compiler::Compile(env, ast);
    env.exportFunction("print", printFunc);

