#pragma once

#include <vector>

#include "Types.h"

namespace Signal
{
	class Object;
	class Scope;

	enum OpCode
	{
		OP_PUSH,	// Push object onto top of stack
		OP_POP,		// Pop object back from top of stack
		OP_NIL,		// Push Nil onto stack

		OP_CALL,	// Call a function
		OP_MCALL,	// Call a member function
		OP_RETURN,	// Return from a function call, a return value is on top of the stack
		OP_ECALL,	// Call exported function

		OP_SET,		// Sets variable to object on top of the stack
		OP_DEF,     // Defines a variable and sets it to object on top of the stack
		OP_REF,		// References a variable to the stack

		OP_BR,		// Branch unconditionally
		OP_BRT,		// If top of the stack has True then branch
		OP_BRF,		// If top of the stack has False then branch

		OP_ADD,		// Add two objects from top of stack, and push the answer to the stack
		OP_SUB,		// Subtract two objects from top of stack, and push the answer to the stack
		OP_MUL,		// Multiply two objects from top of stack, and push the answer to the stack
		OP_DIV,		// Divide two objects from top of stack, and push the answer to the stack
		OP_NEG,		// Negate the top of the stack

		OP_INC,		// Increment the object on top of the stack
		OP_DEC,		// Decrement the object on top of the stack

		OP_NOT,		// Logical not the top of the stack
		OP_OR,		// Or two objects from top of stack, and push the answer to the stack
		OP_AND,		// And two objects from top of stack, and push the answer to the stack
		OP_EQEQ,	// Compare two objects from top of stack, and push the boolean answer to the stack
		OP_NEQ,		// Compare two objects from top of stack, and push the boolean answer to the stack
		OP_LT,		// Compare two objects from top of stack, and push the boolean answer to the stack
		OP_GT,		// Compare two objects from top of stack, and push the boolean answer to the stack
		OP_LTE,		// Compare two objects from top of stack, and push the boolean answer to the stack
		OP_GTE		// Compare two objects from top of stack, and push the boolean answer to the stack
	};

	struct Instruction
	{
		Instruction (OpCode op)
		:
			m_op (op)
		{}

		Instruction (OpCode op, uint32_t arg)
		:
			m_op  (op),
			m_arg (arg)
		{}

		Instruction (OpCode op, std::shared_ptr<Object> object)
		:
			m_op	 (op),
			m_object (object)
		{}

		Instruction (OpCode op, std::shared_ptr<Scope> scope)
		:
			m_op	(op),
			m_scope (scope)
		{}

		Instruction (OpCode op, std::shared_ptr<Scope> scope, std::shared_ptr<Object> object)
		:
			m_op	 (op),
			m_scope  (scope),
			m_object (object)
		{}

		Instruction (OpCode op, uint32_t arg, std::shared_ptr<Object> object)
		:
			m_op	 (op),
			m_arg	 (arg),
			m_object (object)
		{}

		OpCode   m_op;
		uint32_t m_arg;

		std::shared_ptr<Scope>  m_scope;
		std::shared_ptr<Object> m_object;
	};

	class CodeBlock
	{
		public:

		CodeBlock () {}

		void write(OpCode op)
		{
			m_instructions.push_back (Instruction (op));
		}

		void write(OpCode op, uint32_t arg)
		{
			m_instructions.push_back (Instruction (op, arg));
		}

		void write(OpCode op, std::shared_ptr<Object> object)
		{
			m_instructions.push_back (Instruction (op, object));
		}

		void write(OpCode op, std::shared_ptr<Scope> scope)
		{
			m_instructions.push_back (Instruction (op, scope));
		}

		void write(OpCode op, std::shared_ptr<Scope> scope, std::shared_ptr<Object> object)
		{
			m_instructions.push_back (Instruction (op, scope, object));
		}

		void write(OpCode op, uint32_t arg, std::shared_ptr<Object> object)
		{
			m_instructions.push_back (Instruction (op, arg, object));
		}

		Instruction& operator[] (uint32_t i)
		{ 
			return m_instructions[i]; 
		}

		uint32_t count () const
		{
			return m_instructions.size ();
		}

		private:

		std::vector<Instruction> m_instructions;
	};
}