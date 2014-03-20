#include <iostream>

#include "Compiler.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Enviroment.h"

using namespace Signal;


std::shared_ptr<Object> printFunc(Environment env, std::vector<std::shared_ptr<Object>> args)
{
	if (args.size() != 1)
		throw Error ("print() : Print takes 1 parameter.");
	std::cout << args[0]->toString();

	return nullptr;
}


int main (int argc, char* argv[])
{
	try
	{
		std::cout << "Signal v0.1 - Jeremic" << std::endl << std::endl;
		std::string name = "";

		/*if (argc == 2)*/ {
			name = "C:\\Users\\DarkstaR\\Documents\\Visual Studio 2010\\Projects\\signal\\Debug\\test.sig";//argv[1];

			FileInput file (name);
			Lexer lexer (file);
			Parser parser (lexer);
			std::shared_ptr<AST> ast = parser.parse_program ();
			Environment env = Environment ();

			env.exportFunction("print", printFunc);

			Compiler::Compile(env, ast);
			Interpreter interpreter(env);
			interpreter.execute ();
		}
	}
	catch (Error& error)
	{
		if (error.hasPositionInfo()) {
			std::cout << "Error on line " << error.getLine() << " around character " << error.getCharacter() << " : " << error.getError();
		} else {
			std::cout << error.getError();
		}
		if (error.hasInternalInfo())
			std::cout << std::endl << "  (" << error.getInternalLine() << ") " << error.getInternalFile();
	}

	return 0;
}